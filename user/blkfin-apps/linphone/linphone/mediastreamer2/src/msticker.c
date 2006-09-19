/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/



#include "mediastreamer2/msticker.h"


void * ms_ticker_run(void *s);

void ms_ticker_start(MSTicker *s){
	s->run=TRUE;
	ms_thread_create(&s->thread,NULL,ms_ticker_run,s);
}


void ms_ticker_init(MSTicker *ticker)
{
	ms_mutex_init(&ticker->lock,NULL);
	ms_cond_init(&ticker->cond,NULL);
	ticker->execution_list=NULL;
	ticker->ticks=1;
	ticker->time=0;
	ticker->interval=10;
	ticker->run=FALSE;
	ticker->exec_id=0;
	ms_ticker_start(ticker);
}

MSTicker *ms_ticker_new(){
	MSTicker *obj=ms_new(MSTicker,1);
	ms_ticker_init(obj);
	return obj;
}

void ms_ticker_stop(MSTicker *s){
	ms_mutex_lock(&s->lock);
	s->run=FALSE;
	ms_cond_wait(&s->cond,&s->lock);
	ms_mutex_unlock(&s->lock);
	ms_thread_join(s->thread,NULL);
}


void ms_ticker_uninit(MSTicker *ticker)
{
	ms_ticker_stop(ticker);
	ms_mutex_destroy(&ticker->lock);
	ms_cond_destroy(&ticker->cond);
}

void ms_ticker_destroy(MSTicker *ticker){
	ms_ticker_uninit(ticker);
	ms_free(ticker);
}

static void find_filters(MSList **filters, MSFilter *f ){
	int i;
	MSQueue *link;
	if (f==NULL) ms_fatal("Bad graph.");
	/*ms_message("seeing %s, seen=%i",f->desc->name,f->seen);*/
	if (f->seen){
		return;
	}
	f->seen=TRUE;
	*filters=ms_list_append(*filters,f);
	/* go upstream */
	for(i=0;i<f->desc->ninputs;i++){
		link=f->inputs[i];
		if (link!=NULL) find_filters(filters,link->prev.filter);
	}
	/* go downstream */
	for(i=0;i<f->desc->noutputs;i++){
		link=f->outputs[i];
		if (link!=NULL) find_filters(filters,link->next.filter);
	}
}

static MSList *get_sources(MSList *filters){
	MSList *sources=NULL;
	MSFilter *f;
	for(;filters!=NULL;filters=filters->next){
		f=(MSFilter*)filters->data;
		if (f->desc->ninputs==0){
			sources=ms_list_append(sources,f);
		}
	}
	return sources;
}

int ms_ticker_attach(MSTicker *ticker,MSFilter *f)
{
	MSList *sources=NULL;
	MSList *filters=NULL;
	MSList *it;
	
	if (f->ticker!=NULL) {
		ms_message("Filter %s is already being scheduled; nothing to do.",f->desc->name);
		return 0;
	}

	find_filters(&filters,f);
	sources=get_sources(filters);
	if (sources==NULL){
		ms_fatal("No sources found around filter %s",f->desc->name);
		ms_list_free(filters);
		return -1;
	}
	/*run preprocess on each filter: */
	for(it=filters;it!=NULL;it=it->next)
		ms_filter_preprocess((MSFilter*)it->data,ticker);
	ms_mutex_lock(&ticker->lock);
	ticker->execution_list=ms_list_concat(ticker->execution_list,sources);
	ms_mutex_unlock(&ticker->lock);
	ms_list_free(filters);
	return 0;
}



int ms_ticker_detach(MSTicker *ticker,MSFilter *f)
{	
	MSList *sources=NULL;
	MSList *filters=NULL;
	MSList *it;

	if (f->ticker==NULL) {
		ms_message("Filter %s is not scheduled; nothing to do.",f->desc->name);
		return 0;
	}

	find_filters(&filters,f);
	sources=get_sources(filters);
	if (sources==NULL){
		ms_mutex_unlock(&ticker->lock);
		ms_fatal("No sources found around filter %s",f->desc->name);
		return -1;
	}
	ms_mutex_lock(&ticker->lock);
	for(it=sources;it!=NULL;it=ms_list_next(it)){
		ticker->execution_list=ms_list_remove(ticker->execution_list,it->data);
	}
	ms_mutex_unlock(&ticker->lock);
	ms_list_for_each(filters,(void (*)(void*))ms_filter_postprocess);
	ms_list_free(filters);
	ms_list_free(sources);
	return 0;
}


static bool_t filter_can_process(MSFilter *f, int tick){
	/* look if filters before this one have run */
	int i;
	MSQueue *l;
	for(i=0;i<f->desc->ninputs;i++){
		l=f->inputs[i];
		if (l!=NULL){
			if (l->prev.filter->last_tick!=tick) return FALSE;
		}
	}
	return TRUE;
}

static void call_process(MSFilter *f){
	bool_t process_done=FALSE;
	if (f->desc->ninputs==0){
		ms_filter_process(f);
	}else{
		while (ms_filter_inputs_have_data(f)) {
			if (process_done){
				ms_warning("Re-scheduling filter %s: all data should be consumed in one process call, so fix it.",f->desc->name);
			}
			ms_filter_process(f);
			process_done=TRUE;
		}
	}
}

static void run_graph(MSFilter *f, MSTicker *s, MSList **unschedulable, bool_t force_schedule){
	int i;
	MSQueue *l;
	if (f->last_tick!=s->ticks ){
		if (filter_can_process(f,s->ticks) || force_schedule) {
			/* this is a candidate */
			f->last_tick=s->ticks;
			call_process(f);	
			/* now recurse to next filters */		
			for(i=0;i<f->desc->noutputs;i++){
				l=f->outputs[i];
				if (l!=NULL){
					run_graph(l->next.filter,s,unschedulable, force_schedule);
				}
			}
		}else{
			/* this filter has not all inputs that have been filled by filters before it. */
			*unschedulable=ms_list_prepend(*unschedulable,f);
		}
	}
}

static void run_graphs(MSTicker *s, MSList *execution_list, bool_t force_schedule){
	MSList *it;
	MSList *unschedulable=NULL;
	for(it=execution_list;it!=NULL;it=it->next){
		run_graph((MSFilter*)it->data,s,&unschedulable,force_schedule);
	}
	/* filters that are part of a loop haven't been called in process() because one of their input refers to a filter that could not be scheduled (because they could not be scheduled themselves)... Do you understand ?*/
	/* we resolve this by simply assuming that they must be called anyway 
	for the loop to run correctly*/
	/* we just recall run_graphs on them, as if they were source filters */
	if (unschedulable!=NULL) {
		run_graphs(s,unschedulable,TRUE);
		ms_list_free(unschedulable);
	}
}

#ifdef __MACH__
#include <sys/types.h>
#include <sys/timeb.h>
#endif

#ifdef WIN32

#include <sys/types.h>
#include <sys/timeb.h>
/**
 * timespec structure
 * @struct timespec
 */
  struct timespec
  {
    long tv_sec;
    long tv_nsec;
  };
#endif

static uint64_t get_cur_time(){
	struct timespec ts;
#ifdef WIN32
    struct _timeb time_val;

    _ftime (&time_val);
    ts.tv_sec = time_val.time;
    ts.tv_nsec = time_val.millitm * 1000000;
#elif __MACH__
    struct timeb time_val;

    ftime (&time_val);
    ts.tv_sec = time_val.time;
    ts.tv_nsec = time_val.millitm * 1000000;
#else
	if (clock_gettime(CLOCK_REALTIME,&ts)<0){
		ms_fatal("clock_gettime() doesn't work: %s",strerror(errno));
	}
#endif
	return (ts.tv_sec*1000LL) + (ts.tv_nsec/1000000LL);
}

static void sleepMs(int ms){
#ifdef WIN32
    Sleep(ms);
#else
	struct timespec ts;
	ts.tv_sec=0;
	ts.tv_nsec=ms*1000000LL;
	nanosleep(&ts,NULL);
#endif
}

void * ms_ticker_run(void *arg)
{
	MSTicker *s=(MSTicker*)arg;
	uint64_t realtime;
	uint64_t orig=get_cur_time();
	int lastlate=0;
	s->ticks=1;
	ms_mutex_lock(&s->lock);
	while(s->run){
		s->ticks++;
		run_graphs(s,s->execution_list,FALSE);
		ms_mutex_unlock(&s->lock);
		s->time+=s->interval;
		while(1){
			int64_t diff;
			realtime=get_cur_time()-orig;
			diff=s->time-realtime;
			//ms_error("Waiting %i miliseconds.",diff);
			if (diff>0){
				/* sleep until next tick */
				sleepMs(diff);
			}else{
				int late=-diff;
				if (late>s->interval*5 && late>lastlate){
					ms_warning("We are late of %d miliseconds.",late);
				}
				lastlate=late;
				break; /*exit the while loop */
			}
		}
		ms_mutex_lock(&s->lock);
	}
	ms_cond_signal(&s->cond);
	ms_mutex_unlock(&s->lock);
	ms_thread_exit(NULL);
    return NULL;
}

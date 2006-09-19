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

#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msvideo.h"

#include <ffmpeg/avcodec.h>

extern void ms_ffmpeg_check_init();

int ms_pix_fmt_to_ffmpeg(MSPixFmt fmt){
	switch(fmt){
		case MS_RGB24:
			return PIX_FMT_RGB24;
		case MS_YUV420P:
			return PIX_FMT_YUV420P;
		default:
			ms_fatal("format not supported.");
			return -1;
	}
	return -1;
}

MSPixFmt ffmpeg_pix_fmt_to_ms(int fmt){
	switch(fmt){
		case PIX_FMT_RGB24:
			return MS_RGB24;
		case PIX_FMT_YUV420P:
			return MS_YUV420P;
		default:
			ms_fatal("format not supported.");
			return 0;
	}
	return 0;
}


typedef struct PixConvState{
	mblk_t *yuv_msg;
	MSVideoSize size;
	enum PixelFormat in_fmt;
	enum PixelFormat out_fmt;
}PixConvState;

static void pixconv_init(MSFilter *f){
	PixConvState *s=ms_new(PixConvState,1);
	s->yuv_msg=NULL;
	s->size=MS_VIDEO_SIZE_CIF;
	s->in_fmt=PIX_FMT_YUV420P;
	s->out_fmt=PIX_FMT_YUV420P;
	f->data=s;
	ms_ffmpeg_check_init();
}

static void pixconv_uninit(MSFilter *f){
	PixConvState *s=(PixConvState*)f->data;
	if (s->yuv_msg!=NULL) freemsg(s->yuv_msg);
	ms_free(s);
}

static mblk_t * pixconv_alloc_mblk(PixConvState *s){
	if (s->yuv_msg!=NULL){
		int ref=s->yuv_msg->b_datap->db_ref;
		if (ref==1){
			return dupmsg(s->yuv_msg);
		}else{
			/*the last msg is still referenced by somebody else*/
			ms_message("Somebody still retaining yuv buffer (ref=%i)",ref);
			freemsg(s->yuv_msg);
			s->yuv_msg=NULL;
		}
	}
	s->yuv_msg=allocb(avpicture_get_size(s->out_fmt,s->size.width,s->size.height),0);
	s->yuv_msg->b_wptr=s->yuv_msg->b_datap->db_lim;
	return dupmsg(s->yuv_msg);
}

static void pixconv_process(MSFilter *f){
	mblk_t *im,*om;
	PixConvState *s=(PixConvState*)f->data;
	AVPicture orig,dest;

	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		if (s->in_fmt==s->out_fmt){
			om=im;
		}else{
			om=pixconv_alloc_mblk(s);
			avpicture_fill(&orig,(uint8_t*)im->b_rptr,s->in_fmt,s->size.width,s->size.height);
			avpicture_fill(&dest,(uint8_t*)om->b_rptr,s->out_fmt,s->size.width,s->size.height);
			if (img_convert(&dest,s->out_fmt,&orig,s->in_fmt,s->size.width,s->size.height) < 0) {
				freemsg(om);
				om=NULL;
			}
			freemsg(im);
		}
		if (om!=NULL) ms_queue_put(f->outputs[0],om);
	}
}

static int pixconv_set_vsize(MSFilter *f, void*arg){
	PixConvState *s=(PixConvState*)f->data;
	s->size=*(MSVideoSize*)arg;
	return 0;
}

static int pixconv_set_pixfmt(MSFilter *f, void *arg){
	MSPixFmt fmt=*(MSPixFmt*)arg;
	PixConvState *s=(PixConvState*)f->data;
	s->in_fmt=ms_pix_fmt_to_ffmpeg(fmt);
	return 0;
}

static MSFilterMethod methods[]={
	{	MS_FILTER_SET_VIDEO_SIZE, pixconv_set_vsize	},
	{	MS_FILTER_SET_PIX_FMT,	pixconv_set_pixfmt	},
	{	0	,	NULL }
};

MSFilterDesc ms_pix_conv_desc={
	.id=MS_PIX_CONV_ID,
	.name="MSPixConv",
	.text="A pixel format converter",
	.category=MS_FILTER_OTHER,
	.ninputs=1,
	.noutputs=1,
	.init=pixconv_init,
	.process=pixconv_process,
	.uninit=pixconv_uninit,
	.methods=methods
};

MS_FILTER_DESC_EXPORT(ms_pix_conv_desc)


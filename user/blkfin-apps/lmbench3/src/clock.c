/*
 * clock.c
 *
 * calculate the minimum timing loop length that gives us significant results
 */
#include "bench.h"

char	*id = "$Id: clock.c 1516 2005-11-16 09:47:07Z sonicz $";
char	*revision = "$Revision: 1516 $";

main()
{
	uint64	enough;
	double	t_overhead, l_overhead;

	enough = compute_enough(15);
	printf("ENOUGH=%lu\n", (unsigned long)enough); fflush(stdout);
	t_overhead = timing_overhead(enough);
	printf("TIMING_OVERHEAD=%f\n", t_overhead); fflush(stdout);
	l_overhead = loop_overhead(enough, t_overhead);
	printf("LOOP_OVERHEAD=%f\n", l_overhead);
	printf("# version [%s]\n", revision);
	exit(0);
}

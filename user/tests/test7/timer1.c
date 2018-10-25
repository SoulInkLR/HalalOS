#include "sysapi.h"

int main(void *arg)
{
        (void)arg;

        unsigned long quartz;
        unsigned long ticks;
        unsigned long dur;
        int i;
	(void) dur;
        clock_settings(&quartz, &ticks);
	dur = (quartz + ticks) / ticks;
        printf(" 2");
	printf(" %ld %ld  \n",dur, current_clock());
        for (i = 4; i < 8; i++) {
	  wait_clock(100);
	  //wait_clock(current_clock() + dur);
                printf(" %d", i);
        }
        return 0;
}


/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"

void wake_irq (unsigned long irqid);

static unsigned long irq  = 0;


#ifdef SFS_WANTED
#define SYS_MAX_TICKS_IN_SEC    50 /*Since tick timer runs for 20ms: 1 sec = 1000ms (20ms * 50) */
#define TICK_TIMER_GRANULARITY  20  /*20 milli secs*/
#else
#define SYS_MAX_TICKS_IN_SEC    100 /*Since tick timer runs for 10ms: 1 sec = 1000ms (10ms * 100) */
#define TICK_TIMER_GRANULARITY  10  /*10 milli secs*/
#endif

#define MILLISEC_2_NANOSEC(msec)  msec * 1000 * 1000

static int btmhlftask_id = 0;
static int task_id = 0;

unsigned int tm_get_ticks_per_second (void) 
{
	return SYS_MAX_TICKS_IN_SEC;
}

#ifndef SFS_WANTED
size_t alloc_size = 0;

void *tm_calloc(size_t nmemb, size_t size)
{
	alloc_size += size;

	return calloc (nmemb, size);
}

void * tm_malloc (size_t size)
{
	return tm_calloc (1, size);
}

void tm_free (void *p , size_t size)
{
	alloc_size -= size;
	free (p);
}

void tick_clock (void *unused)
{
	struct timespec     req = {0, MILLISEC_2_NANOSEC
			          (TICK_TIMER_GRANULARITY)};
	register clock_t    start, end;
	register int        tick = 0;

	for (;;) {

		start = times (NULL);
		nanosleep (&req, NULL);
		end = times (NULL);
		
		tick = end - start;

		if (tick <= 0)
			tick = 1;

		while (tick--) {
			update_times (); 
		}
	}
}

void tick_service (void *unused) 
{
	int evt = 0;

	while (1) {
		evt_rx (btmhlftask_id, &evt, TMR_SERVE_TIMERS);

		if (evt & TMR_SERVE_TIMERS)
			btm_hlf ();
	}
}
#endif
static inline void timer_rq_init (void)
{
	INIT_LIST_HEAD (&expd_tmrs);
	tmrrq.count = 0;
}

int init_timer_mgr (void)
{
	int i = TIMER_WHEEL;

	timer_rq_init ();

#ifdef SFS_WANTED
	irq = softirq_create (btm_hlf, 0);
#else
	if (task_create ("TMRBHF", 99, TSK_SCHED_RR, 32000,
	  		  tick_service, NULL, NULL, &btmhlftask_id) == TSK_FAILURE) {
		return FAILURE;
	}

	if (task_create ("TMRTHF", 99, TSK_SCHED_RR, 32000,
			  tick_clock, NULL, NULL, &task_id) == TSK_FAILURE) {

		return FAILURE;
	}
#endif
	while (--i >= 0) {
		create_sync_lock (&tmrrq.root[i].lock);
	}

	return SUCCESS;
}


void service_timers (void)
{
#ifdef SFS_WANTED
	wake_irq (irq);
	softirq_wakeup ();
#else
        if (tm_process_tick_and_update_timers ()) {
		evt_snd (btmhlftask_id, TMR_SERVE_TIMERS);
	}
#endif
}

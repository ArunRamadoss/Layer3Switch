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

#define MAX_TIMERS 2000

static inline void timer_rq_init (void);
static void calc_time (APP_TIMER_T * ptmr);
static TIMER_T * alloc_timer (void);
static int alloc_timer_id (void);


/************* Private Variable Declaration *********************/
static int indx = 0;
/****************************************************************/

/************* Global Variable Declaration *********************/
struct active_timers  tmrrq;
unsigned  int clk[TIMER_WHEEL];
/****************************************************************/

int shut_timer_mgr (void)
{
	return SUCCESS;
}

static inline void update_wheel (TIMER_T *p, int wheel)
{
	p->wheel = wheel;
}

int find_tmr_slot_and_place (APP_TIMER_T * ptmr)
{
	unsigned int ct = ptmr->ctime;
	unsigned int t = 0;
	int wheel = -1;

	if (GET_HRS(ct, t)) 
		wheel = HRS;
	else if (GET_MINS (ct, t)) 
		wheel = MIN;
	else if (GET_SECS (ct, t)) 
		wheel = SEC;
	else if (GET_TICK (ct, t)) 
		wheel = TICK;
	
	if (wheel >= 0) {
		ptmr->rt = (clk[wheel] + t);
		update_wheel (ptmr->timer, wheel);
		ptmr->timer->is_running = 1;
		timer_add (ptmr, &tmrrq.root[wheel], QUERY_TIMER_EXPIRY);
	}
	return 0;
}

static void calc_time (APP_TIMER_T * ptmr)
{
	unsigned int tick = ptmr->timer->rmt;
	unsigned int secs = tick / tm_get_ticks_per_second ();
	unsigned int mins = secs / 60;
	unsigned int hrs =  mins / 60;

	SET_TICK (ptmr->ctime,  tick);
	SET_SECS (ptmr->ctime,  secs);
	SET_MINS (ptmr->ctime,  mins);
	SET_HRS  (ptmr->ctime,  hrs);

	ptmr->timer->exp = tick + clk[TICK];
}

TIMER_T * start_timer (unsigned int ticks, void *data, void (*handler) (void *), int flags)
{
	TIMER_T  *new = NULL;
	APP_TIMER_T *apptimer = NULL;
	int idx = 0;

	if ( !(idx = alloc_timer_id ())  || 
             !(new = alloc_timer ())) {
		return -1;
	}

	new->idx = idx;
	new->rmt = ticks;
	new->data = data;
	new->time_out_handler = handler;

	if (flags)
		new->flags = flags;
	else
		new->flags = TIMER_ONCE;

	apptimer = tm_calloc (1, sizeof(APP_TIMER_T));

	if (!apptimer) {
		free_timer (new);
		return -1;
	}

	apptimer->timer = new;

	new->apptimer = apptimer;

	INIT_LIST_HEAD (&apptimer->elist);

	calc_time (apptimer);

	find_tmr_slot_and_place (apptimer);

	INC_TIMER_COUNT ();

	return new;
}

int setup_timer (TIMER_T **p, void (*handler) (void *), void *data)
{
	TIMER_T  *new = NULL;
	int idx = 0;

	if ( !(idx = alloc_timer_id ())  || 
             !(new = alloc_timer ())) {
		return -1;
	}

	new->idx = idx;
	new->rmt = 0;
	new->data = data;
	new->time_out_handler = handler;

	new->flags = TIMER_FOREVER;

	update_wheel (new, WAIT_TIMERS);

	*p = new;

	return 0;
}

int mod_timer (TIMER_T *p, unsigned int secs)
{
	APP_TIMER_T *apptimer = NULL;

	if (!p)
		return -1;

	apptimer = tm_calloc (1, sizeof(APP_TIMER_T));

	if (!apptimer) {
		return -1;
	}

	INIT_LIST_HEAD (&apptimer->elist);

	p->rmt = secs * tm_get_ticks_per_second ();

	apptimer->timer = p;

	p->apptimer = apptimer;

	calc_time (apptimer);

	find_tmr_slot_and_place (apptimer);

	INC_TIMER_COUNT ();

	return 0;
}

int timer_restart  (TIMER_T *p)
{
	APP_TIMER_T *apptimer = NULL;

	if (!p)
		return -1;

	apptimer = tm_calloc (1, sizeof(APP_TIMER_T));

	if (!apptimer) {
		return -1;
	}

	INIT_LIST_HEAD (&apptimer->elist);

	calc_time (apptimer);

	find_tmr_slot_and_place (apptimer);

	INC_TIMER_COUNT ();

	return 0;
}


static int alloc_timer_id (void)
{
	if (TIMER_COUNT() > MAX_TIMERS) {
		return 0;
	}
	return ++indx;
}

int stop_timer (TIMER_T *p)
{
	timer_del (p->apptimer, &tmrrq.root[p->wheel]);

	p->is_running = 0;

	free (p->apptimer);

	DEC_TIMER_COUNT ();

	return 0;
}

int del_timer (TIMER_T *p)
{
	if (p->apptimer && p->is_running)
		stop_timer (p);
}

static inline TIMER_T * alloc_timer (void)
{
	return tm_calloc (1, sizeof(TIMER_T));
}

void free_timer (TIMER_T *p) 
{
	tm_free (p, sizeof(*p));
}

static inline int timers_pending_for_service (void)
{
	if (TIMER_COUNT ()) {
		if (rb_first (&tmrrq.root[TICK_TIMERS])) {
			goto service_req;
		}
		if (IS_NXT_SEC_HAPPEND) {
			if (rb_first (&tmrrq.root[SECS_TIMERS]))
				goto service_req;
		}
		if (IS_NXT_MIN_HAPPEND) {
			if (rb_first (&tmrrq.root[MIN_TIMERS])) 
				goto service_req;
		}
		if (IS_NXT_HR_HAPPEND) {
			if (rb_first (&tmrrq.root[HRS_TIMERS])) 
				goto service_req;
		}
	}
	return 0;
service_req:	
        return 1;
}

void update_times ()
{
	if (!(++clk[TICK] % tm_get_ticks_per_second ())) {

		if (!((++clk[SEC]) % 60) && !((++clk[MIN]) % 60)) {
			++clk[HRS];
		}
	}
	if (timers_pending_for_service ()) {
		service_timers ();
	}
}

unsigned int get_secs (void)
{
	return clk[SEC];
}
unsigned int get_ticks (void)
{
	return clk[TICK];
}

unsigned int get_mins (void)
{
	return clk[MIN];
}
unsigned int get_hrs (void)
{
	return clk[HRS];
}
unsigned int get_timers_count (void)
{
	return TIMER_COUNT();
}

void show_uptime (void)
{
	printf ("Uptime  %d hrs %d mins %d secs %d ticks\n",get_hrs(), 
		 get_mins() % 60, get_secs() % 60, get_ticks() % tm_get_ticks_per_second ());
}
void tm_test_timers_latency ()
{
	dump_task_info ();
	start_timer (1 * tm_get_ticks_per_second (), NULL, tm_test_timers_latency , 0);
}

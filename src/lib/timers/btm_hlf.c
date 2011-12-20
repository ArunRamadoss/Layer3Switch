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

struct list_head expd_tmrs;

void btm_hlf (void)
{
#ifdef SFS_WANTED
	tm_process_tick_and_update_timers ();
#endif
	if (!list_empty (&expd_tmrs)) {
		process_expd_timers ();
	}
}

void process_expd_timers (void)
{
	APP_TIMER_T		  *ptmr = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *next = NULL;
	struct list_head  *head = &expd_tmrs;

	list_for_each_safe (pnode, next, head) {

		ptmr = list_entry (pnode, APP_TIMER_T, elist);


		if (ptmr->timer->time_out_handler) {
			ptmr->timer->time_out_handler (ptmr->timer->data);
		}

		list_del (&ptmr->elist);

		ptmr->timer->apptimer = NULL;

		if (ptmr->timer->flags & TIMER_ONCE) {
			free_timer (ptmr->timer);
		} 
		else if (ptmr->timer->flags & TIMER_REPEAT) {
			timer_restart (ptmr->timer);
		}

		free (ptmr);
	}
}

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

APP_TIMER_T * timer_tree_walk (struct rb_root  *root, unsigned int key, char flag);

#define IS_TMR_EXPD(ptmr)               !ptmr->ctime 

static inline void timer_expiry_action (APP_TIMER_T * ptmr)
{
	if (IS_TMR_EXPD (ptmr)) {
		DEC_TIMER_COUNT ();
		ptmr->timer->is_running = 0;
		list_add_tail (&ptmr->elist, &expd_tmrs);
	}
	else
		find_tmr_slot_and_place (ptmr);
}

static inline void clear_wheel_timer (APP_TIMER_T *p)
{
	unsigned int wheel_mask [TIMER_WHEEL] = {0xFFFFFC00 , 0xFFFF03FF, 
					         0xFFC0FFFF, 0x3FFFFF};
	p->ctime &= wheel_mask[p->timer->wheel];
}

static inline int process_timers (struct rb_root *root, unsigned int expires)
{
	APP_TIMER_T *ptmr = NULL;
	int     expd_count = 0;

	while ((ptmr = timer_tree_walk (root, expires, QUERY_TIMER_EXPIRY))) {
		expd_count++;
		clear_wheel_timer (ptmr);
		timer_del (ptmr, root);
		timer_expiry_action (ptmr);
	}
	return expd_count;
}

int tm_process_tick_and_update_timers (void)
{
	int timers_exp = 0;

	timers_exp = process_timers (&tmrrq.root[TICK_TIMERS], clk[TICK_TIMERS]);

	if (IS_NXT_SEC_HAPPEND) {
		timers_exp += process_timers (&tmrrq.root[SECS_TIMERS], clk[SECS_TIMERS]);
		if (IS_NXT_MIN_HAPPEND) {
			timers_exp += process_timers (&tmrrq.root[MIN_TIMERS], clk[MIN_TIMERS]);
			if (IS_NXT_HR_HAPPEND) {
				timers_exp += process_timers (&tmrrq.root[HRS_TIMERS], clk[HRS_TIMERS]);
			}
		}
	}

	return timers_exp;
}

void timer_add (APP_TIMER_T *n, struct rb_root *root, int flag)
{
        unsigned int cmp = 0, dest = 0;
        struct rb_node **link = NULL;
        struct rb_node *parent = NULL;
        APP_TIMER_T  *x = NULL;

	cmp = n->rt;

	sync_lock (&root->lock);

        link = &root->rb_node;

        while (*link) {

                parent = *link;

                x = rb_entry(parent, APP_TIMER_T, rlist);

		dest = x->rt;

                if (cmp < dest)
                        link = &(*link)->rb_left;
                else
                        link = &(*link)->rb_right;
        }

        rb_link_node(&n->rlist, parent, link);
        rb_insert_color(&n->rlist, root);

	sync_unlock (&root->lock);
}

void timer_del (APP_TIMER_T *n, struct rb_root *root)
{
	sync_lock (&root->lock);
        rb_erase (&n->rlist, root);
        RB_CLEAR_NODE(&n->rlist);
	sync_unlock (&root->lock);
}

APP_TIMER_T * timer_tree_walk (struct rb_root  *root, unsigned int key, char flag)
{
	struct rb_node **p = NULL;
	struct rb_node *parent = NULL;
	APP_TIMER_T * timer = NULL;
	unsigned int cmp = 0;

	sync_lock (&root->lock);

	p = &root->rb_node;

	while (*p) {

		parent = *p;

		timer = rb_entry (parent, APP_TIMER_T, rlist);

		cmp = timer->rt;

		if (key < cmp)
			p = &(*p)->rb_left;
		else if (key > cmp) 
			p = &(*p)->rb_right;
		else {
			sync_unlock (&root->lock);
			return timer;
		}
	}
	sync_unlock (&root->lock);
	return NULL;
}

int timer_pending (TIMER_T *p)
{
        if (!p)
                return 0;

        return p->is_running;
}

unsigned int timer_get_remaining_time (TIMER_T *p)
{
	int t = 0;

        if (!p || !p->is_running) {
                return 0;
        }

        t = p->exp - get_ticks();

        if (t < 0) {
                printf ("\nTIMERS : Oopss negative remainiting time %s\n",__FUNCTION__);
                t = 0;
        }

        return t;
}

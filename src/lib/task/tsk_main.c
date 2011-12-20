/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: tsk_main.c,v 1.6 2011/01/16 20:00:18 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"

int                 g_tsks_crtd = 0;

LIST_HEAD(tsk_hd);

tmtaskid_t          curtskid;

#define istsk_selftsk(ptsk)    (ptsk->task_id  == tsk_selfid ())

/**
 *  task_create   -   creates task
 *  @
 *  @
 */

retval_t task_create (char tskname[], int tsk_prio, int sched_alg, int stk_size,
	              void *(*start_routine) (void *), void (*exit_routine) (),
		      void *arg, tmtaskid_t * rettskid)
{
	retval_t            Ret_val = TSK_FAILURE;
	tmtask_t           *ptsk_info = NULL;

	Ret_val = validate_tsk_params (tskname, sched_alg, stk_size, start_routine);

	if (Ret_val != TSK_FAILURE)
	{

		ptsk_info = (tmtask_t *) tsk_alloc (sizeof (tmtask_t), 1);

		if (!ptsk_info)
			return TSK_FAILURE;

		fill_tsk_info (tskname, tsk_prio, sched_alg, stk_size,
				start_routine, exit_routine, arg, ptsk_info);

		Ret_val = init_tsk (ptsk_info);

		if (Ret_val != TSK_FAILURE)
		{
			++g_tsks_crtd;

			return start_task (ptsk_info, rettskid);
		}
	}
	return Ret_val;
}

retval_t init_tsk (tmtask_t * ptskinfo)
{
	init_tsk_attr (ptskinfo);

	init_tsk_mtx_and_cond (ptskinfo);

	return TSK_SUCCESS;
}

void * tsk_wrap (void *ptskarg)
{
	tmtask_t           *ptsk = (tmtask_t *) ptskarg;

	ptsk->tsk_pid = get_tsk_pid ();

	ptsk->tsk_strt_tk = times (NULL);

	ptsk->tsk_state = TSK_RUNNING;

	tsk_node_add (ptsk);

	ptsk->start_routine (ptsk->tskarg);

	return NULL;
}

retval_t task_delete (char tskname[], tmtaskid_t tskid)
{
	tmtask_t           *ptskinfo = get_tsk_info (tskname, tskid);

	if (!ptskinfo)
		return TSK_FAILURE;

	if (istsk_selftsk (ptskinfo))
		return TSK_FAILURE;

	deinit_tsk (ptskinfo);

	tsk_node_del (ptskinfo);

	if (ptskinfo->exit_routine)
		ptskinfo->exit_routine ();

	tsk_cancel (ptskinfo->task_id);

	tsk_dealloc ((void *) ptskinfo);

	return TSK_SUCCESS;
}

retval_t deinit_tsk (tmtask_t * ptskinfo)
{
	deinit_tsk_attr (ptskinfo);

	deinit_tsk_mtx_and_cond (ptskinfo);

	return TSK_SUCCESS;
}

void fill_tsk_info (char *tskname, int tsk_prio, int sched_alg, int stk_size,
               void *(*start_routine) (void *), void (*exit_routine) (),
               void *arg, tmtask_t * ptskinfo)
{
    memcpy (ptskinfo->task_name, tskname, MAX_TASK_NAME);
    ptskinfo->tsk_state = TSK_NOT_CREATED;
    ptskinfo->curr_evt = NONE;
    ptskinfo->prev_evt = NONE;
    ptskinfo->prio = tsk_prio;
    ptskinfo->schedalgo = sched_alg;
    ptskinfo->stksze = stk_size;
    ptskinfo->start_routine = start_routine;
    ptskinfo->exit_routine = exit_routine;
    ptskinfo->task_id = NO_TASK_ID;
    ptskinfo->tskarg = arg;
}

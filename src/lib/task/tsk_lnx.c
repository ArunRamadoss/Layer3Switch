/*
 *  Authors:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: tsk_lnx.c,v 1.4 2011/01/16 20:00:18 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"
#include <unistd.h>
#include <sys/syscall.h>

void * tsk_wrap (void *ptskarg);

retval_t
deinit_tsk_attr (tmtask_t * ptskinfo)
{
    pthread_attr_init (&ptskinfo->tsk_attr);

    return TSK_SUCCESS;
}

retval_t
deinit_tsk_mtx_and_cond (tmtask_t * ptskinfo)
{
    pthread_cond_destroy (&ptskinfo->tsk_cnd);

    pthread_mutex_destroy (&ptskinfo->tsk_mtx);

    pthread_mutex_destroy (&ptskinfo->evt_mtx);

    return TSK_SUCCESS;
}

retval_t
start_task (tmtask_t * ptskinfo, tmtaskid_t * ptskid)
{
    if (pthread_create (ptskid, &ptskinfo->tsk_attr, tsk_wrap,
                        (void *) ptskinfo))
    {
        return TSK_FAILURE;
    }
    ptskinfo->task_id = *ptskid;
    return TSK_SUCCESS;
}

tmtaskid_t
tsk_selfid ()
{
    return pthread_self ();
}

void
tsk_cancel (tmtaskid_t task_id)
{
    pthread_cancel (task_id);
}

retval_t
init_tsk_mtx_and_cond (tmtask_t * ptskinfo)
{
    pthread_cond_init (&ptskinfo->tsk_cnd, NULL);

    pthread_mutex_init (&ptskinfo->tsk_mtx, NULL);

    pthread_mutex_init (&ptskinfo->evt_mtx, NULL);

    return TSK_SUCCESS;

}

retval_t
init_tsk_attr (tmtask_t * ptskinfo)
{
    struct sched_param  param;

    pthread_attr_init (&ptskinfo->tsk_attr);

    pthread_attr_setstacksize (&ptskinfo->tsk_attr, ptskinfo->stksze);

    pthread_attr_setschedpolicy (&ptskinfo->tsk_attr, ptskinfo->schedalgo);
#if 0
    param.sched_priority = ptskinfo->prio;

    pthread_attr_setschedparam (&ptskinfo->tsk_attr, &param);

    switch (getuid ())
    {
        case 0:                /* For Root User */
            pthread_attr_setinheritsched (&ptskinfo->tsk_attr,
                                          PTHREAD_EXPLICIT_SCHED);
            break;
        default:                /*For Other Users */
            pthread_attr_setinheritsched (&ptskinfo->tsk_attr,
                                          PTHREAD_INHERIT_SCHED);
            break;
    }
#endif
    return TSK_SUCCESS;
}

void
tsk_delay (int secs, int nsecs)
{
    struct timespec     delay_tmr;

    delay_tmr.tv_sec = secs;

    delay_tmr.tv_nsec = nsecs;

    nanosleep (&delay_tmr, NULL);
}

void
tsk_sleep (int secs)
{
    sleep (secs);
}

void
tsk_mdelay (int msecs)
{
    usleep (msecs * 1000);
}

pid_t
get_tsk_pid ()
{
    return syscall (SYS_gettid);
}

int
evt_rx (tmtaskid_t tskid, int *pevent, int event)
{
    tmtask_t           *tskinfo = get_tsk_info_frm_id (tskid);

    if (!tskinfo)
        return TSK_FAILURE;

    pthread_mutex_lock (&tskinfo->evt_mtx);

    while (1)
    {
        if (tskinfo->curr_evt & event)
        {
            *pevent = tskinfo->curr_evt;
            tskinfo->curr_evt &= 0;
            pthread_mutex_unlock (&tskinfo->evt_mtx);
            return TSK_SUCCESS;
        }
        pthread_cond_wait (&tskinfo->tsk_cnd, &tskinfo->evt_mtx);
    }
}

void
evt_snd (tmtaskid_t tskid, int event)
{
    tmtask_t           *tskinfo = get_tsk_info_frm_id (tskid);

    pthread_mutex_lock (&tskinfo->evt_mtx);

    tskinfo->prev_evt = tskinfo->curr_evt;
    tskinfo->curr_evt |= event;

    pthread_cond_signal (&tskinfo->tsk_cnd);
    pthread_mutex_unlock (&tskinfo->evt_mtx);
}

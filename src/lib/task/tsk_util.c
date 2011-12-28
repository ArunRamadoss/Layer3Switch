/*
 *  Authors:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: tsk_util.c,v 1.6 2011/01/16 20:00:18 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"

extern struct list_head      tsk_hd;

retval_t
validate_tsk_params (char *tskname, int sched_alg, int stk_size,
                     void *(*start_routine) (void *))
{

#if 0
    if (g_tsks_crtd == MAX_TSKS)
    {
        return TSK_FAILURE;
    }
    if (((tskname && !str_len_validate (tskname, MAX_TASK_NAME))
         || printf ("-ERROR- :  Invalid Task Name")) || (start_routine
                                                         ||
                                                         printf
                                                         ("-ERROR- :  No Start Routine Specified")))
        return TSK_FAILURE;
    if (sched_alg != TSK_SCHED_RR &&
        sched_alg != TSK_SCHED_FIFO && sched_alg != TSK_SCHED_OTHER)
    {
        printf ("-ERROR- : Invalid Scheduling Algorithm\n");
        return TSK_FAILURE;
    }
#endif

    if (stk_size < MIN_THREAD_STACK_SIZE)
    {
        printf ("-ERROR- : Invalid Stack Size, Minimum stack size is : %d ",
                MIN_THREAD_STACK_SIZE);
        return TSK_FAILURE;
    }
    return TSK_SUCCESS;
}

int
my_str_len (char *str)
{
    if (!str)
        return 0;
    return strlen (str);
}

int
str_len_validate (char *tskname, int len)
{
    int                 strln = my_str_len (tskname);

    if (strln > 0 && strln <= len)
        return 1;
    return 0;
}

tmtask_t * get_tsk_info (char *tskname, tmtaskid_t tskid)
{
    register struct list_head *node;

    list_for_each (node, (&tsk_hd))
    {
        if (!(strcmp (((tmtask_t *) node)->task_name, tskname)) &&
            (((tmtask_t *) node)->task_id == tskid))
            return (tmtask_t *) node;
    }
    return NULL;
}

void dump_task_info ()
{
    char               *tsk_sched[] = { "OTHER", "FIFO", "Round Robin" };
    char               *tsk_state[] =
        { "NOT_CREATED", "TSK_RUNNING", "TSK_SLEEPING", "TSK_SUSPENDED" };
    char               *tsk_sched_inherit[] =
        { "PTHREAD_INHERIT_SCHED", "PTHREAD_EXPLICIT_SCHED" };
    register struct list_head *node = NULL;
    register tmtask_t  *tskinfo = NULL;


    printf
        ("\n  Task Name      TaskID       StackSize(KB)  TaskState       TaskPrio        Scheduling Alg    TASK PID \n");
    printf
        ("\r  ---------    ----------     ----------    ------------    ------------     ----------------  ---------\n");


    list_for_each (node, (&tsk_hd))
    {
        tskinfo = (tmtask_t *) node;
        printf
            ("%10s     %#x    %8d      %10s     %9d      %15s     %6d\n",
             tskinfo->task_name, tskinfo->task_id, tskinfo->stksze / 1024, 
             tsk_state[tskinfo->tsk_state],
             tskinfo->prio, tsk_sched[tskinfo->schedalgo], 
	     tskinfo->tsk_pid);
    }
}

/*replace after mm*/
void * tsk_alloc (int size, int cnt)
{
    return calloc (cnt, size);
}

void tsk_dealloc (void *dptr)
{
    if (dptr)
        free (dptr);
    dptr = NULL;
}

void tsk_node_add (tmtask_t * ptskinfo)
{
    list_add (&ptskinfo->tsk_node, &tsk_hd);
}

void tsk_node_del (tmtask_t * ptskinfo)
{
    list_del (&ptskinfo->tsk_node);
}

tmtaskid_t tsk_get_tskid (char *tskname)
{
    register struct list_head *node;

    list_for_each (node, (&tsk_hd))
    {
        if (!strcmp (((tmtask_t *) node)->task_name, tskname))
            return ((tmtask_t *) node)->task_id;
    }
    return -1;
}

unsigned long tick_start (void)
{
	get_ticks ();
}

void tick_end (unsigned long *p, unsigned long start)
{
	*p += (get_ticks () - start);
}

char               *
get_tsk_name (tmtaskid_t tskid)
{
    register struct list_head *node;

    list_for_each (node, (&tsk_hd))
    {
        if (((tmtask_t *) node)->task_id == tskid)
            return ((tmtask_t *) node)->task_name;
    }
    return "INVALID_TASK_ID";
}

tmtask_t           *
get_tsk_info_frm_id (tmtaskid_t tskid)
{
    register struct list_head *node;

    list_for_each (node, (&tsk_hd))
    {
        if (((tmtask_t *) node)->task_id == tskid)
            return (tmtask_t *) node;
    }
    return NULL;
}

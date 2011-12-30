/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: task.h,v 1.6 2011/01/26 20:14:18 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */

#ifndef TASK_H
#define TASK_H
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "list.h"

#define  MIN_THREAD_STACK_SIZE   20000  /*should be atleast PTHREAD_STACK_MIN (bits/local_lim.h)*/
#define  MAX_TASK_NAME       6
#define  MAX_TSKS            100

#define  NONE          0
#define  NO_TASK_ID    1
#define  TSK_SUCCESS 1
#define  TSK_FAILURE 0

typedef sem_t               sync_lock_t;
typedef pthread_t           tmtaskid_t;
typedef time_t              tmclktk_t;
typedef pthread_mutex_t     tskmtx_t;
typedef pthread_cond_t      tskcnd_t;
typedef pthread_attr_t      tskattr_t;
typedef long int retval_t;

enum
{
    TSK_SCHED_OTHER = 0,
    TSK_SCHED_FIFO,
    TSK_SCHED_RR
}tsk_sched_alg;

enum {
    TSK_NOT_CREATED = 0, 
    TSK_RUNNING,
    TSK_SLEEPING,
    TSK_SUSPENDED
}TASK_STATE;  

enum {
    TSK_RESUME = 4
}TASK_SIG;

struct pstat
{
	long unsigned int utime;
	long unsigned int stime;
	long unsigned int tcpu;
};


typedef struct __task_tm__ {
    struct list_head     tsk_node;
    char            task_name[MAX_TASK_NAME + 2];
    tmtaskid_t       task_id;
#if 0
    tmsemid_t       sem_id; /*Currently not used */
#endif
    tmclktk_t       tsk_strt_tk;
#ifndef SFS_WANTED
    tskmtx_t        tsk_mtx;
    tskmtx_t        evt_mtx;
    tskcnd_t        tsk_cnd;
#endif
    tskattr_t       tsk_attr;
    int             tsk_pid;
    int             tsk_state;
    int 	    stksze;
    int             prio;
    int             schedalgo;
    int             curr_evt;
    int             prev_evt;
    int             sig;
    struct pstat    cpu_stats;
    void            *(*start_routine)(void*);
    void            (*exit_routine)();
    void            *tskarg;
}tmtask_t;

extern int                 g_tsks_crtd;
extern tmtask_t            gtskinfo[];

retval_t init_tsk (tmtask_t *);
retval_t start_tsk(tmtask_t *, tmtaskid_t *);
retval_t deinit_tsk (tmtask_t *);
void fill_tsk_info (char *tskname, int tsk_prio, int sched_alg, int stk_size,
               void *(*start_routine) (void *), void (*exit_routine) (),
               void *arg, tmtask_t * ptskinfo);
retval_t validate_tsk_params (char *tskname, int sched_alg, int stk_size,
                     void *(*start_routine) (void *));
void tsk_dealloc (void *dptr);
void * tsk_alloc (int size, int cnt);
void tsk_node_add (tmtask_t * ptskinfo);
void tsk_node_del (tmtask_t * ptskinfo);
tmtaskid_t tsk_get_tskid (char *tskname);
retval_t deinit_tsk_attr (tmtask_t * ptskinfo);
retval_t deinit_tsk_mtx_and_cond (tmtask_t * ptskinfo);
retval_t start_task (tmtask_t * ptskinfo, tmtaskid_t * ptskid);
tmtaskid_t tsk_selfid ();
void tsk_cancel (tmtaskid_t task_id);
retval_t init_tsk_mtx_and_cond (tmtask_t * ptskinfo);
retval_t init_tsk_attr (tmtask_t * ptskinfo);
tmtask_t * get_tsk_info (char *tskname, tmtaskid_t tskid);

inline unsigned int tm_get_ticks_per_second (void);
void *tm_calloc(size_t nmemb, size_t size);
void * tm_malloc (size_t size);
void tm_free (void *p , size_t size);
inline unsigned int tm_get_ticks_per_second (void); 
inline unsigned int tm_convert_msec_to_tick (unsigned int msecs);
#endif

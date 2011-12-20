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
#include "sched_sfs.h"

#define MIN_THREAD_STACK_SIZE   20000  /*should be atleast PTHREAD_STACK_MIN (bits/local_lim.h)*/


#ifndef SFS_WANTED
typedef sem_t               sync_lock_t;
typedef pthread_t           tmtaskid_t;
typedef time_t              tmclktk_t;
typedef pthread_mutex_t     tskmtx_t;
typedef pthread_cond_t      tskcnd_t;
typedef pthread_attr_t      tskattr_t;
#else
typedef unsigned long      tmtaskid_t;
typedef unsigned long      tmclktk_t;
struct attr1 {
	int stksze;
	int prio;
	int schedalgo;
};
typedef struct attr1    tskattr_t;
#endif

typedef long int retval_t;

#define TSK_SUCCESS 1
#define TSK_FAILURE 0

enum
{
    TSK_SCHED_OTHER = 0,
    TSK_SCHED_FIFO,
    TSK_SCHED_RR
}tsk_sched_alg;

int softirq_create (void (*action) (void), int flags);
inline unsigned int tm_get_ticks_per_second (void);
void *tm_calloc(size_t nmemb, size_t size);
void * tm_malloc (size_t size);
void tm_free (void *p , size_t size);
void softirq_wakeup (void);
inline unsigned int tm_get_ticks_per_second (void); 
inline unsigned int tm_convert_msec_to_tick (unsigned int msecs);
#endif

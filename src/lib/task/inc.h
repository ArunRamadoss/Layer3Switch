#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef SFS_WANTED
#include <pthread.h>
#include <semaphore.h>
#endif
#include <sys/types.h>
#include <sys/times.h>
#include "list.h"
#include "task.h"

#define   MAX_TASK_NAME       6
#define   MAX_TSKS            100

#define  NONE          0
#define  NO_TASK_ID    1


enum {
    TSK_NOT_CREATED = 0, 
    TSK_RUNNING,
    TSK_SLEEPING,
    TSK_SUSPENDED
}TASK_STATE;  

enum {
    TSK_RESUME = 4
}TASK_SIG;


typedef struct __task_tm__ {
    struct list_head     tsk_node;
    char            task_name[MAX_TASK_NAME + 1];
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

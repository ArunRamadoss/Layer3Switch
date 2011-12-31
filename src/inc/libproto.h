/*
 * This file contatins all library funtions prototypes
 */
#ifndef _LIB_PROTO_H
#define _LIB_PROTO_H
#include "task.h"
#include "hashlib.h"
/*TASK*/
retval_t task_create (const char tskname[], int tsk_prio, int sched_alg, int stk_size,
	              void *(*start_routine) (void *), void (*exit_routine) (),
		      void *arg, tmtaskid_t * rettskid);
tmtaskid_t tsk_selfid ();
void tsk_delay (int secs, int nsecs);
void tsk_sleep (int secs);
void tsk_mdelay (int msecs);
int evt_rx (tmtaskid_t tskid, int *pevent, int event);
void evt_snd (tmtaskid_t tskid, int event);
retval_t task_delete (char tskname[], tmtaskid_t tskid);
void tsk_cancel (tmtaskid_t task_id);
char * get_tsk_name (tmtaskid_t tskid);
unsigned long tick_start (void);
void tick_end (unsigned long *p, unsigned long start);


/*MEM pool*/
int mem_pool_create (const char *name, size_t size, int n_blks, int flags);
int mem_pool_delete (int pool_id);
void * alloc_block (int memid);
int free_blk (int memid, void *addr);
void *tm_calloc(size_t nmemb, size_t size);
void * tm_malloc (size_t size);
void tm_free (void *p , size_t size);
unsigned int tm_get_ticks_per_second (void); 


/*TIMERS*/

int    setup_timer (void **p, void (*handler) (void *), void *data);
int    mod_timer   (void *p, unsigned int secs);
int    del_timer   (void *p);
int    stop_timer  (void *p);
void * start_timer (unsigned int ticks, void *data, void (*handler) (void *), int flags);
int timer_pending (void *p);
unsigned int timer_get_remaining_time (void *p);
unsigned int get_secs (void);
unsigned int get_ticks (void);
unsigned int get_mins (void);
unsigned int get_hrs (void);

/*HASH*/
struct hash_table * create_hash_table (const char *, int ,int (*cmp)(const uint8_t *, const uint8_t *),
				        int  (*index_gen)(unsigned char *), int ); 
void destroy_hash_table (HASH_TABLE *htbl, void (*free_data) (void *));
void * hash_tbl_lookup (unsigned char *, struct hash_table *);
int  hash_tbl_add (uint8_t *, struct hash_table *, void *);
int  hash_tbl_delete (unsigned char *, struct hash_table *, void (*free_data)(void *));
int  hash_walk (struct hash_table *, void (*callback)(void *));
uint32_t jhash_1word(uint32_t a, uint32_t initval);

/*MSG Q*/

int msg_create_Q (const char *name, int maxmsg, int size);
int msg_rcv (int qid, char **msg, int size);
int msg_send (int qid, void *msg, int size);

/*LOCK*/
int create_sync_lock (sync_lock_t *slock);
int sync_lock (sync_lock_t *slock);
int sync_unlock (sync_lock_t *slock);

uint32_t ip_2_uint32 (uint8_t *ipaddress, int byte_order);
void uint32_2_ipstring (uint32_t ipAddress, uint8_t *addr);

void send_packet (void *buf, uint16_t port, int len);
#endif

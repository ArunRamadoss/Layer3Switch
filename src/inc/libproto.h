/*
 * This file contatins all library funtions prototypes
 */


/*MEM pool*/


int mem_pool_create (char *name, size_t size, int n_blks, int flags);
int mem_pool_delete (int pool_id);
void * alloc_block (int memid);
int free_blk (int memid, void *addr);

/*TIMERS*/

int setup_timer (void **p, void (*handler) (void *), void *data);
int mod_timer (void *p, unsigned int secs);
int del_timer (void *p);
int stop_timer (void *p);
void * start_timer (unsigned int ticks, void *data, void (*handler) (unsigned long), int flags);

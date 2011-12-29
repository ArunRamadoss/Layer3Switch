/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: mem_main.c,v 1.3 2011/01/23 10:34:04 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include "list.h"

#define MAX_POOL_NAME         8
#define MACHINE_ALIGNEMENT    (sizeof(unsigned long))

#define MEM_ALIGNED(bytes)    (bytes % MACHINE_ALIGNEMENT) 

#define in_range(start, end, size, value) ((value >= start)  && \
					  ((value + size)  <= end))

#define is_not_valid_block_offset(offset ,size)  (offset % size)

#define COMPUTE_ADDR_BLOCK(saddr, cnt, size)	(saddr + (cnt * size))

#define debug_mem_pool(fmt) printf ("MEM_POOL_MGR: %s", fmt);

struct mem_info {
	struct list_head n;
	void  *  saddr,  *  eaddr;
	void   **addr_blks;
	int      memid;
	char     pool_name[MAX_POOL_NAME];
	int      nblks;
	int      useblks;
	int      fblks;
	size_t   size;
};


void * alloc_mem (size_t size);
static inline void free_mem(void *mem);
struct mem_info * get_next_free_mcb (int *memid);
struct mem_info * get_mem_info (int memid);
int build_free_mem_blk_list (struct mem_info *m);
int add_to_mcb (struct mem_info *m);
void *tm_calloc(size_t nmemb, size_t size);

static struct list_head hd_mcb;

int mem_init (void)
{
	INIT_LIST_HEAD (&hd_mcb);
	return 0;
}

int mem_pool_create (char *name, size_t size, int n_blks, int flags)
{
	unsigned int bytes = size * n_blks;

	struct mem_info   *mcb = NULL;

	int          align = MEM_ALIGNED (bytes);

	int memid = -1;

	if (align) {
		/*Requested memory is not aligned ,so aligned it 
 		  as per machine aligenment*/
		align += ((MACHINE_ALIGNEMENT - align));
		bytes +=  align;
		warn ("Memory is not aligned");
	}

	mcb = get_next_free_mcb (&memid); /*XXX:Dynamic or Static - What to do*/

	if (!mcb) {
		debug_mem_pool ("-ERR- : No Free Memory Blks\n");
		return -1;
	}

	mcb->size = size;
	mcb->fblks = n_blks; 
	mcb->useblks = 0;
	mcb->nblks = n_blks;
	mcb->memid = memid;
	
	INIT_LIST_HEAD (&mcb->n);

	if (!(mcb->saddr = alloc_mem (bytes))) {
		debug_mem_pool ("-ERR- : Insufficent Memory\n");
		return -1;
	}

 	/*Compute the end */
	mcb->eaddr = mcb->saddr + bytes;

	/*build the free list as a dll to access the mem blk O(1)*/
	build_free_mem_blk_list (mcb);

	/*Add to main control block*/
	add_to_mcb (mcb);

	/*Return non-neg mem pool id*/
	return mcb->memid;
}	

int mem_pool_delete (int pool_id)
{
	struct mem_info *p = get_mem_info (pool_id);
	if (!p) {
		return -1;
	}

	free_mem (p->addr_blks);
	free_mem (p->saddr);
	free_mem (p);

	return 0;
}	

int add_to_mcb (struct mem_info *m)
{
	list_add_tail (&m->n, &hd_mcb);
	return 0;
}

int build_free_mem_blk_list (struct mem_info *m) 
{
	int i = 0;
	char **mblk = NULL;

	mblk = alloc_mem (sizeof(void *) * m->nblks);

	if (!mblk) {
		debug_mem_pool ("-E- Out of Memory");
		return -1;
	}

	while (i < m->nblks) {
		/*Compute the addr of the new block */
		mblk[i] = COMPUTE_ADDR_BLOCK (m->saddr, i, m->size); 
		++i;
	}
	m->addr_blks = (void **)mblk;

	return 0;
}

struct mem_info * get_next_free_mcb (int *memid)
{
	/*XXX: Re-implement this function*/
	static int i = 0;
	struct mem_info *p = alloc_mem (sizeof(struct mem_info));

	if (!p) {
		return NULL;
	}
	*memid = ++i;

	return p;
}

struct mem_info * get_mem_info (int memid)
{
	struct list_head *head = &hd_mcb;
	struct list_head *p = NULL;
	struct mem_info  *pmem = NULL;

	list_for_each (p, head) {
		pmem = list_entry (p, struct mem_info, n);
		if (pmem->memid == memid)
			return pmem;
	}
	return NULL;
}

void * alloc_block (int memid)
{
	struct mem_info *p = get_mem_info (memid);
	int i = 0;
	void *retaddr = NULL;

	if (!p->fblks) {
		return NULL;
	}
	while (i < p->nblks) {
		if (!p->addr_blks[i])  {
			i++;
			continue;
		}
		retaddr = p->addr_blks[i];
		p->addr_blks[i] = NULL;
		p->fblks--;
		p->useblks++;
		return retaddr;
	}
	return NULL;	
}

int free_blk (int memid, void *addr)
{
	int  idx = -1;
	int offset = -1;
	struct mem_info *p = get_mem_info (memid);

	if (!p) {
		debug_mem_pool ("-ERR- : Invalid POOL ID\n");
		return -1;
	}

	if (!in_range (p->saddr, p->eaddr, p->size, addr)) 
		goto invalid_address;

	offset = ((unsigned long)addr - (unsigned long)p->saddr);

	if (is_not_valid_block_offset (offset , p->size)) 
		goto invalid_address;

	idx = offset / p->size;

	if (!p->addr_blks[idx]) {
		p->addr_blks[idx] = addr;
		p->fblks++;
		p->useblks--;
		return 0;
	}

invalid_address:
	debug_mem_pool ("-ERR- : Trying to free invaild address\n");
	return -1;
}

/* alloc_mem  is the final routine to allocate memory*/
void * alloc_mem (size_t size)
{
	return (void *) tm_calloc (1, size);
}

static inline void free_mem(void *mem)
{
	free (mem);
}

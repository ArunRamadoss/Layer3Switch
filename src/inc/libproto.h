/*
 * This file contatins all library funtions prototypes
 */


/*MEM pool*/
int mem_pool_create (const char *name, size_t size, int n_blks, int flags);
int mem_pool_delete (int pool_id);
void * alloc_block (int memid);
int free_blk (int memid, void *addr);

/*TIMERS*/

int    setup_timer (void **p, void (*handler) (void *), void *data);
int    mod_timer   (void *p, unsigned int secs);
int    del_timer   (void *p);
int    stop_timer  (void *p);
void * start_timer (unsigned int ticks, void *data, void (*handler) (unsigned long), int flags);
int timer_pending (void *p);
unsigned int timer_get_remaining_time (void *p);
unsigned int get_secs (void);
unsigned int get_ticks (void);
unsigned int get_mins (void);
unsigned int get_hrs (void);

/*HASH*/
struct hash_table * create_hash_table (const char *, int ,unsigned (*cmp)(const uint8_t *, const uint8_t *),
				        int  (*index_gen)(unsigned char *), int ); 
void  destroy_hash_table (void *); 
void * hash_tbl_lookup (unsigned char *, struct hash_table *);
int  hash_tbl_add (uint8_t *, struct hash_table *, void *);
int  hash_tbl_delete (unsigned char *, struct hash_table *, void (*free_data)(void *));
int  hash_walk (struct hash_table *, void (*callback)(void *));
uint32_t jhash_1word(uint32_t a, uint32_t initval);

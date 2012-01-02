#include "common_types.h"
#include "bridge.h"
#include "cli.h"

#define   TRUE     1
#define   FALSE    0

enum FDB_STATUS {OTHER = 1, INVALID, LEARNED, SELF, MGMT};

#define MAX_FDB_ENTRIES   1024
#define FDB_HASH_BITS 8
#define FDB_HASH_SIZE (1024)
#define DEFAULT_AGING_TIME_OUT   5 * 60  /*in secs*/
#define debug_fdb(fmt)    printf ("FDB: %s",fmt)

typedef struct fdb_entry {
        MACADDRESS   mac_addr;
	uint16_t     vlan_id;
        int32_t      port;
        uint32_t     status;
	int32_t      is_static;
	uint32_t     expiry;
}fdb_t;

/*************** Prototypes **************************/
void aging_timer_expired (void *);
int fdb_mac_hash(unsigned char *mac);
static int fdb_delete_entry (MACADDRESS mac, int32_t portno);
static int fdb_add_entry (MACADDRESS mac, int32_t portno, int is_static);
static fdb_t * fdb_lookup (MACADDRESS mac, int32_t port_no);
int show_mac_table (void);
int fdb_init (void); 
int mac_address_update (MACADDRESS , int32_t , uint16_t );
int stp_is_mac_learning_allowed (int);
void display_fdb_entry (void *);
void delete_age_out_entires (void *);
static void fdb_free_entry (void *fdb);
/*********************************************************/

/******************* Static Dec  **********************/
static void    * fdb_hash_table = NULL;
static uint32_t fdb_salt = 0;
static int fdb_pool_id = -1;
static TIMER_ID ageing_timer;
/******************************************************/

static tbridge_group_t tbridge;

int fdb_init (void)
{
	tbridge.dot1dTpAgingTime = DEFAULT_AGING_TIME_OUT;

	tbridge.dot1dTpLearnedEntryDiscards = 0;

	fdb_hash_table = create_hash_table ("FDBH", FDB_HASH_SIZE,
					    compare_ether_addr,
					    fdb_mac_hash, sizeof(MACADDRESS));
	if (!fdb_hash_table) {
		debug_fdb ("Hash Table Creation failed ...\n");
		return -1;
	}

	fdb_pool_id = mem_pool_create ("FDB", MAX_FDB_ENTRIES * sizeof(fdb_t), 
                                       MAX_FDB_ENTRIES, 0);
	if (fdb_pool_id < 0) {
		debug_fdb ("Mem pool creation failed !\n");
		destroy_hash_table (fdb_hash_table, fdb_free_entry);
		return -1;
	}

	fdb_salt = (uint32_t) times (NULL);

	setup_timer (&ageing_timer, aging_timer_expired, NULL);

	mod_timer (ageing_timer, tbridge.dot1dTpAgingTime);

	return 0;
}

int mac_address_update (MACADDRESS mac_addr, int32_t port_no, uint16_t vlan_id)
{
	fdb_t *p = NULL;

	if (!stp_is_mac_learning_allowed (port_no)) {
		return 0;
	}

	p = fdb_lookup (mac_addr, port_no);

	if (p) {
		if (p->is_static) { /*Mac-Address is Static one*/
			return 0;
		}/*Dynamic Entry*/
		if (p->port != port_no) {
			/*Station moved on to different port*/
			p->port = port_no;
		}
		p->expiry = (get_secs () + tbridge.dot1dTpAgingTime); 

	} else { /*Add new entry in the fdb table*/
		if (fdb_add_entry (mac_addr, port_no, FALSE)) {
			++tbridge.dot1dTpLearnedEntryDiscards;
			return -1;
		}
	}
	return 0;
}

static fdb_t * fdb_lookup (MACADDRESS mac, int32_t port_no)
{
        fdb_t *fdb = hash_tbl_lookup (mac.addr, fdb_hash_table);

	if (!fdb)
		return NULL;

	return fdb;
}

static int fdb_add_entry (MACADDRESS mac, int32_t portno, int is_static)
{
	fdb_t * nfdb =  alloc_block (fdb_pool_id);

	if (!nfdb) {
		return -1;
	}

	memcpy (nfdb->mac_addr.addr, mac.addr, sizeof(MACADDRESS));
	nfdb->port = portno;
	nfdb->status = LEARNED;
	nfdb->is_static = is_static;
	nfdb->expiry = (get_secs () + tbridge.dot1dTpAgingTime); 

	return hash_tbl_add (nfdb->mac_addr.addr, fdb_hash_table, (void *)nfdb);
}

static void fdb_free_entry (void *fdb)
{
	fdb_t *ffdb = (fdb_t *)fdb;
	free_blk (fdb_pool_id, ffdb);
}

static int fdb_delete_entry (MACADDRESS mac, int32_t portno)
{
	return hash_tbl_delete (mac.addr, fdb_hash_table, fdb_free_entry);
}


/*XXX: Picked from Linux Kernel*/
int fdb_mac_hash(unsigned char *mac)
{
        /* use 1 byte of OUI cnd 3 bytes of NIC */
        uint32_t key = (uint32_t )(*(uint32_t *)(mac + 2));
        return jhash_1word (key, fdb_salt) & (FDB_HASH_SIZE - 1);
}

void delete_age_out_entires (void *data)
{
	fdb_t *p = (fdb_t *)data;

	if (p->expiry <= get_secs ()) {
		fdb_delete_entry (p->mac_addr, p->port);
	}
}

void aging_timer_expired (void *unused)
{
	hash_walk (fdb_hash_table, delete_age_out_entires); 
	mod_timer (ageing_timer, tbridge.dot1dTpAgingTime);
}

void display_fdb_entry (void *data)
{
	unsigned char *mac = NULL;
	fdb_t *p = (fdb_t *)data;

	mac = p->mac_addr.addr;

        printf ("  %-4d    %02x:%02x:%02x:%02x:%02x:%02x    %-10s    %-4d\n", p->port,
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], p->is_static?"static": "dynamic",
		((p->expiry - get_secs()) < 0)?0: (p->expiry - get_secs()));

}

int show_mac_table (void)
{
	printf ("  %-6s   %-10s         %-10s   %-10s\n",
	        "port", "mac address", "type", "aging(secs)");
	printf ("  %-6s   %-10s         %-10s   %-10s\n",
		"----", "-----------", "------","-------");
	hash_walk (fdb_hash_table, display_fdb_entry); 

	printf ("Mac Entries Dropped: %d\n", tbridge.dot1dTpLearnedEntryDiscards);
	
	return 0;
}

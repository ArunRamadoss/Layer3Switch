#include "common_types.h"
#include "list.h"
#include "vlan.h"

#define debug_vlan printf

static struct list_head vlan_db;
static int vlan_mem_id = -1;

/******************************************************************/
int vlan_init (void);
struct vlan_static_entry * get_vlan_entry (uint16_t vlan_id);
int vlan_create_default_vlan (int vlan_id);
int show_vlan (void);
/*****************************************************************/

int vlan_init (void)
{
	INIT_LIST_HEAD (&vlan_db);

	vlan_mem_id = mem_pool_create ("VLNDB", MAX_VLAN_IDS * 
				       sizeof(struct vlan_static_entry), 
                                       MAX_VLAN_IDS, 0);
	if (vlan_mem_id < 0) {
		debug_vlan ("Mem pool creation failed !\n");
		return -1;
	}

	if (vlan_create_default_vlan (VLAN_DEFAULT_VLAN_ID)  < 0) {
		debug_vlan ("-ERR- : Default vlan creation failed\n");
		return -1;
	}

	return 0;
}

int vlan_create_default_vlan (int vlan_id)
{
	int i = 0;

	if (vlan_create_vlan (vlan_id)  < 0) {
		return -1;
	}

	for (i = 0; i < MAX_PORTS ; i++) {
		vlan_add_port (vlan_id, i + 1,TAGGED);
	}

	vlan_spanning_tree_enable_on_vlan (vlan_id, MODE_STP);

	return 0;
}

int vlan_create_vlan (uint16_t vlan_id)
{
	struct vlan_static_entry *new_vlan = get_vlan_entry (vlan_id);

	if (VLAN_IS_NOT_IN_RANGE (vlan_id)) {
		return -1;
	}

	if (new_vlan && (new_vlan->vlan_id == vlan_id))  {
		return 0;
	}

	new_vlan = alloc_block (vlan_mem_id);

	if (!new_vlan)
		return -1;

	new_vlan->vlan_id = vlan_id;
	sprintf (new_vlan->Name, "%s%d", "vlan",vlan_id);
	memset (new_vlan->egress_ports, 0, sizeof (PORTLIST));
	memset (new_vlan->forbidden_egress_ports, 0, sizeof(PORTLIST));
	memset (new_vlan->untagged_ports, 0, sizeof(PORTLIST));

	new_vlan->row_status = ROW_STATE_CREATE_WAIT;

	INIT_LIST_HEAD (&new_vlan->next);
	INIT_LIST_HEAD (&new_vlan->port_vlan_stats);

	list_add_tail (&new_vlan->next, &vlan_db);

	return 0;
}

int vlan_add_port (int vlan_id, int port_no , int type)
{
	struct vlan_static_entry *p = get_vlan_entry (vlan_id);

	if (!p) {
		return -1;
	} 
	switch (type) {
		case TAGGED: 
			SET_PORT_LIST (p->egress_ports, port_no);
			break;
		case UNTAGGED:
			SET_PORT_LIST (p->untagged_ports, port_no);
			break;
		case FORBIDDEN:
			printf ("Currently not supporting this \n");
			break;

	}
#if 0
	if (p->stp_enabled) 
#endif
	{
		vlan_create_stp_port (p, port_no);
	}

	return 0;
}

int vlan_del_port (int vlan_id, int port_no, int type)
{
	struct vlan_static_entry *p = get_vlan_entry (vlan_id);

	if (!p) {
		return -1;
	} 

	switch (type) {
		case TAGGED: 
			RESET_PORT_LIST (p->egress_ports, port_no);
			break;
		case UNTAGGED:
			RESET_PORT_LIST (p->untagged_ports, port_no);
			break;
		case FORBIDDEN:
			printf ("Currently not supporting this \n");
			break;

	}
	return 0;
}

struct vlan_static_entry * get_vlan_entry (uint16_t vlan_id)
{
	struct vlan_static_entry *p = NULL;

	list_for_each_entry(p, &vlan_db, next) {
		if (p->vlan_id == vlan_id)
			return p;
	}
	
	return NULL;
}

int show_vlan (void)
{
	struct vlan_static_entry *p = NULL;

	int count = 0;

	printf (" Vlans \n");
	printf ("--------\n");
	printf ("Vlan Id \n");
	printf ("--------\n"); 
	list_for_each_entry(p, &vlan_db, next) {
		printf ("%u\n", p->vlan_id);
		count++;
	}
	printf ("Total Vlans : %d\n",count);
	return 0;
}

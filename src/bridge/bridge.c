#include "common_types.h"
#include "bridge.h"

/***********************Prototypes**********************/
int bridge_init (void);
void bridge_base_init (void);
int fdb_init (void);
int stp_init (void);
int rstp_init (void);
int stpmgr_init (void);
int vlan_init (void);
void get_bridge_mac_address (char *);
/*******************************************************/

extern char switch_mac[];
bridge_group_t   this_bridge;
port_entry_t     this_bridge_ports[MAX_PORTS];

int bridge_init (void)
{
	bridge_base_init ();
	
	fdb_init ();

	stp_init ();

	rstp_init ();

	stpmgr_init ();

	vlan_init ();

	return 0;
}

void bridge_base_init (void)
{
	int i = MAX_PORTS;
	int j = 0;

	for (j = 0; j < 6; j++)
	  this_bridge.addr.addr[j] = switch_mac[j];

	this_bridge.nports = MAX_PORTS;
	this_bridge.bridge_type = 1; /*Currently Un-known?*/

	while (i-- < 0) {
		this_bridge_ports[i].Port = i + 1;
		this_bridge_ports[i].PortIfIndex = i + 1;
		this_bridge_ports[i].PortCircuit = 0;
		this_bridge_ports[i].PortDelayExceededDiscards = 0;
		this_bridge_ports[i].PortMtuExceededDiscards = 0;
	}
}

void get_bridge_mac_address (char *addr)
{
	int j = 0;
	for (j = 0; j < 6; j++)
	 	addr[j] = this_bridge.addr.addr[j];
}

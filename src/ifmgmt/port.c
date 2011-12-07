#include "common_types.h"
#include "cli.h"
#include "ifmgmt.h"

port_t port_cdb[MAX_PORTS];

int port_init (void)
{
	int idx = MAX_PORTS;

	while (idx--) {
		port_cdb[idx].ifIndex = idx;
		sprintf ((char *)port_cdb[idx].ifDescr, "%s%d","port",idx);
		port_cdb[idx].ifType = 0;
		port_cdb[idx].ifMtu = 1500;
		port_cdb[idx].ifSpeed = 10;
		port_cdb[idx].ifAdminStatus = IF_DOWN;
		port_cdb[idx].ifOperStatus = IF_DOWN;
		port_cdb[idx].ifLastChange = 0;
		port_cdb[idx].ifInOctets = 0;
		port_cdb[idx].ifInUcastPkts = 0;
		port_cdb[idx].ifInDiscards = 0;
		port_cdb[idx].ifInErrors = 0;
		port_cdb[idx].ifInUnknownProtos = 0;
		port_cdb[idx].ifOutOctets = 0;
		port_cdb[idx].ifOutUcastPkts = 0;
		port_cdb[idx].ifOutDiscards = 0;
		port_cdb[idx].ifOutErrors = 0;
		port_cdb[idx].pstp_info = NULL;
		read_port_mac_address (idx, &port_cdb[idx].ifPhysAddress); 
	}
	port_cli_cmds_init ();
}

void send_interface_enable_or_disable (int port , int state)
{
	int vlanid = 1;
	stp_send_event (state, port, vlanid);
}

int get_port_oper_state (int port)
{
	return port_cdb[port - 1].ifOperStatus;
}

int get_port_mac_address (int port, char *mac)
{
	memcpy (mac, port_cdb[port-1].ifPhysAddress.addr, 6);
}

/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "common_types.h"
#include "cli.h"
#include "ifmgmt.h"
#include "ip.h"


void set_ip_address (uint32_t ifindex, uint32_t ipaddress, uint32_t ipmask);

void cli_set_ip (void *args[])
{
	int port = cli_get_port ();

	if (!args[0] || !args[1])
		return;

	set_ip_address (port, ip_2_uint32(args[0], 0), ip_2_uint32 (args[1], 0));
}

void cli_show_ip_interface (void)
{
	int i = 0;
	while (i < MAX_PORTS) {
		if (ip_port[i].Status) {
			char *State[2] = {"UP", "DOWN"};
			uint8_t addr[4];
			printf ("\n%s is administratively %s, line protocol is %s\n", IF_DESCR(i), State[IF_ADMIN_STATUS(i) - 1],
				State[IF_OPER_STATUS(i) - 1]);
			printf  ("Internet address is ");
		 	uint32_2_ipstring (ip_port[i].Addr, &addr);
			printf("%u.%u.%u.%u", addr[0], addr[1],addr[2],addr[3]);
		 	uint32_2_ipstring (ip_port[i].AddrMask, &addr);
			printf  (", subnet mask is ");
			printf("%u.%u.%u.%u\n", addr[0], addr[1],addr[2],addr[3]);
		}
		i++;
	}	
}

void ipv4_cli_init (void)
{
	install_cmd_handler ("ip addresss <address> <netmask>", "Configures IP address on Interface", 
   			      cli_set_ip, "ip address <IPADDR> <IPADDR>", INTERFACE_MODE);

	install_cmd_handler ("show ip interface", "Displays IP Information", 
   			      cli_show_ip_interface, NULL, USER_EXEC_MODE);
}

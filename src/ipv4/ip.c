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
#include "ip.h"

struct ip_addr_entry ip_port[MAX_PORTS];

int init_ip_interface_table ()
{
	int i = 0;

	memset (ip_port, 0, sizeof (ip_port));
	
	while (i < MAX_PORTS) 
	{
		ip_port[i].IfIndex = i + 1;
		i++;
	}

	return 0;
}


int ip_init (void)
{
	init_ip_interface_table ();

	ipv4_cli_init ();
}

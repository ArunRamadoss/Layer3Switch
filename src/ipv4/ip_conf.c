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
        
extern struct ip_addr_entry ip_port[];

int set_ip_address (uint32_t ifindex, uint32_t ipaddress, uint32_t ipmask)
{
	ip_port[ifindex].Addr = ipaddress;
	ip_port[ifindex].AddrMask = ipmask;
	ip_port[ifindex].Status = 1;
	return 0;
}

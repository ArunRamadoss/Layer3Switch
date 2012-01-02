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
#include "cparser.h"
#include "cparser_tree.h"


cparser_result_t cparser_cmd_if_ip_address_addr_mask(cparser_context_t *context,
    uint32_t *addr_ptr,
    uint32_t *mask_ptr)
{
	int port = cli_get_port ();

	if (!set_ip_address (cli_get_port (), ntohl(*addr_ptr), ntohl(*mask_ptr)))
		return CPARSER_OK;
	return CPARSER_NOT_OK;
}


cparser_result_t cparser_cmd_show_ip_interface(cparser_context_t *context)
{
	int i = 0;
	while (i < MAX_PORTS) {
		if (ip_port[i].Status) {
			const char *State[2] = {"UP", "DOWN"};
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

	return CPARSER_OK;
}

#ifndef NT_H
#define NT_H
/* 
 *  Description: Network Trouble shooter 
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <locale.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <libintl.h>
#include <netdb.h>
#include "list.h"


enum GET_IF {
	GET_IF_BY_NAME = 1, 
	GET_IF_BY_IPADDR,
	GET_IF_BY_IFINDEX
};


struct if_info {
	struct list_head nxt_if;
	char		if_name[IFNAMSIZ];
	struct in_addr	ipv4_netmask;
	struct in_addr	ipv4_address;
	int		sock_fd;
	int		if_idx;
	int		admin_state;	/*IFF_UP or IFF_DOWN*/
	int		oper_state;   /*IFF_RUNNING*/
	int 		flags;
};


struct if_info * get_next_if_info (struct if_info *p);
int make_if_up (struct if_info *p);
void display_interface_info (void);
int  read_interfaces (void);
int rtnl_init(void);
int nts_debug (char *fmt, ...);
void set_debug_enable (void);
int resolver_init (void);
int ping_me (struct in_addr );
int check_ns_state (void);
int resolve_hostname (const char *hostname);
int ping_setup (void);
int try_to_resolve_host (void);
struct if_info * get_if (void *key, uint8_t key_type);


#endif  /* NT_H */

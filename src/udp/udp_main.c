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
#include "udp_ctrl.h"

#define MAX_UDP_CTRL_BLOCKS      256

static int udp_pool_id = -1;


int udp_init (void)
{
	udp_pool_id = mem_pool_create ("UDP", MAX_UDP_CTRL_BLOCKS * sizeof(udpctrlblk_t), 
                                       MAX_UDP_CTRL_BLOCKS, 0);
	if (udp_pool_id < 0) {
		return -1;
	}

	return 0;
}

unsigned long open_udp_sock (int family)
{
	udpctrlblk_t *new = NULL;

	new =  alloc_block (udp_pool_id);

	if (!new) 
		return 0;

	memset (new, 0, sizeof(new));

	new->family = family;

	return (unsigned long)new;
}

int sock_v4bind (unsigned long sockblk, uint32_t ipaddr, uint16_t port)
{
	udpctrlblk_t * p = (udpctrlblk_t *)sockblk;

	if (!p)
		return -1;

	p->ipaddr = ipaddr;
	p->sport = port;
	return 0;
}

int sock_recvfrom (unsigned long sockblk, uint8_t **data, size_t datalen)
{

}

int sock_sendto (unsigned long sockblk, uint8_t *data, size_t datalen, uint32_t to_addr, uint16_t to_port)
{
	/*Construc the UDP packet and send it via send_pkt)*/
}

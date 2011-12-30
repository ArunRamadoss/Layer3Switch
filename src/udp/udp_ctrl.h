/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "list.h"

struct udp_data
{
	void *buf;
	struct list_head next;
};

typedef struct udp_ctrl_blk 
{
	struct list_head   next;
	struct list_head   sock_pkt_hd;
	uint32_t           ipaddr;
	uint32_t           ripaddr;
	int32_t            sock;
	uint32_t	   family;
	uint16_t           sport;
	uint16_t           dport;
}udpctrlblk_t;


/*
 *	Spanning tree protocol; BPDU handling
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stp_info.h"

int llc_mac_hdr_init (char *pkt, const void *daddr, const void *saddr, int type, int len)
{
        struct ether_hdr *eth = (struct ether_hdr *)pkt;

        if (type != ETH_P_802_3 && type != ETH_P_802_2)
                eth->type = htons(type);
        else
                eth->type = htons(len);

        if (!saddr)
                saddr = 0;

        memcpy(eth->smac.addr, saddr, ETH_ALEN);

        if (daddr) {
                memcpy(eth->dmac.addr, daddr, ETH_ALEN);
                return 0;
        }

        return -1;
}

void llc_pdu_header_init(uint8_t *pkt, uint8_t type, uint8_t ssap, uint8_t dsap, uint8_t cr)
{
        const int hlen = type == LLC_PDU_TYPE_U ? 3 : 4;
        struct llc_pdu_un *pdu = (struct llc_pdu_un *)(pkt + sizeof(MACHDR));
        pdu->dsap = dsap;
        pdu->ssap = ssap;
        pdu->ssap |= cr;
}

void llc_pdu_init_as_ui_cmd(char *pkt)
{
        struct llc_pdu_un *pdu = (struct llc_pdu_un *)(pkt + sizeof(MACHDR));
        pdu->ctrl_1  = LLC_PDU_TYPE_U;
        pdu->ctrl_1 |= LLC_1_PDU_CMD_UI;
}

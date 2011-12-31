#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "common_types.h"
#include "ifmgmt.h"

#define IS_MAC_UCAST_MAC(addr)  (!(addr[0] & 0xFF))

void process_pkt (void  *pkt, int len, uint16_t port);
void send_pkt (void *buf, int len, uint16_t dport);
int stp_rcv_bpdu (void *pkt, int port, int vlanid, int len);
int mac_address_update (MACADDRESS mac_addr, int32_t port_no, uint16_t vlan_id);
int is_dest_stp_group_address (MACADDRESS mac);

void process_pkt (void  *pkt, int len, uint16_t port)
{
	struct ether_hdr *p = (struct ether_hdr *)pkt;
	int vlanid = 1;

	if ((IF_OPER_STATUS(port) == IF_DOWN) ||
	    (IF_ADMIN_STATUS(port) == IF_DOWN)) {
		free (pkt);
		return;
	}

	if (IS_MAC_UCAST_MAC (p->smac.addr)) {
	 	mac_address_update (p->smac, (uint32_t)port, 1);
#if 0
		if (unknown_umac (p->dmac)) {
			/*flood the packet*/
		}
#endif
	} 
	if (is_dest_stp_group_address (p->dmac)) {
		stp_rcv_bpdu (pkt, port, vlanid, len);
		return;
	}
}

void send_pkt (void *buf, int len, uint16_t dport)
{
}

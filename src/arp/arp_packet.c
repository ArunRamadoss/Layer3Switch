/*
 * arp.c
 *
 *  Created on: Jun 24, 2010
 *      Author: fify
 */
#include <string.h>

#include "arp.h"

/*
 * Hardware type: Ethernet
 * Protocol type: ARP
 * Hardware address length: 6 (MAC address)
 * Protocol address length: 4 (IP protocol)
 * Operation: request OR reply
 * Sender MAC Address: ?
 * Sender IP Address: ?
 * Target MAC Address: ?
 * Target IP Address: ?
 */
void build_arp_hdr(struct arphdr *pkt, __be16 ar_op,
                struct mac_addr *sender_mac, __be32 sender_ip,
                struct mac_addr *target_mac, __be32 target_ip)
{
        memcpy(pkt->ar_sha, (char *)sender_mac, sizeof(struct mac_addr));
        memcpy(pkt->ar_sip, (char *)&sender_ip, sizeof(sender_ip));
        memcpy(pkt->ar_tha, (char *)target_mac, sizeof(struct mac_addr));
        memcpy(pkt->ar_tip, (char *)&target_ip, sizeof(target_ip));

        pkt->ar_op = htons(ar_op);

        pkt->ar_hrd = htons(ETH_P_802_3);
        pkt->ar_pro = htons(ETH_P_IP);
        pkt->ar_hln = ETH_ALEN;
        pkt->ar_pln = sizeof(__be32);
}

void build_arp_request(struct arp_packet *pkt,
                struct mac_addr *sender_mac, __be32 sender_ip,
                struct mac_addr *target_mac, __be32 target_ip)
{
        build_eth_hdr(&pkt->m_eth, target_mac, sender_mac, ETH_P_ARP);
        build_arp_hdr(&pkt->m_arp, ARPOP_REQUEST, sender_mac, sender_ip, target_mac, target_ip);
}

void build_arp_reply(struct arp_packet *pkt,
                struct mac_addr *sender_mac, __be32 sender_ip,
                struct mac_addr *target_mac, __be32 target_ip)
{
        build_eth_hdr(&pkt->m_eth, target_mac, sender_mac, ETH_P_ARP);
        build_arp_hdr(&pkt->m_arp, ARPOP_REPLY, sender_mac, sender_ip, target_mac, target_ip);
}

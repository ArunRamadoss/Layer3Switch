/*
 * arp.h
 *
 *  Created on: Jun 23, 2010
 *      Author: fify
 * Copied from QEMU-KVM
 */

#ifndef ARP_H_
#define ARP_H_

#define HWTYPE_ETHERNET    1

#define ARP_REQUEST        1
#define ARP_REPLY          2

#define ARP_MAXAGE         120         // 120 * 10 seconds = 20 minutes
#define ARP_TIMER_INTERVAL 10000       // The ARP cache is checked every 10 seconds

#define MAX_XMIT_DELAY     1000        // Maximum delay for packets in millisecs     

#pragma pack(push)
#pragma pack(1)

struct arp_hdr 
{
	MACHDR         ethhdr;           // Ethernet header
	unsigned short hwtype;           // Hardware type
	unsigned short proto;            // Protocol type
	unsigned short _hwlen_protolen;  // Protocol address length
	unsigned short opcode;           // Opcode
	MACHDR         shwaddr;         // Source hardware address
	struct ip_addr sipaddr;          // Source protocol address
	MACHDR         dhwaddr;         // Target hardware address
	struct ip_addr dipaddr;          // Target protocol address
};

struct ethip_hdr 
{
	MACHDR eth;
	struct ip_hdr ip;
};

#pragma pack(pop)



struct arp_packet
{
        struct ethhdr m_eth;
        struct arphdr m_arp;
        unsigned char m_padding[18];
};

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
                struct mac_addr *target_mac, __be32 target_ip);

void build_arp_request(struct arp_packet *pkt,
                struct mac_addr *sender_mac, __be32 sender_ip,
                struct mac_addr *target_mac, __be32 target_ip);

void build_arp_reply(struct arp_packet *pkt,
                struct mac_addr *sender_mac, __be32 sender_ip,
                struct mac_addr *target_mac, __be32 target_ip);

#endif /* ARP_H_ */

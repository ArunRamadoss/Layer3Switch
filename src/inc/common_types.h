#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <fcntl.h>
#include <linux/ip.h>
#include <arpa/inet.h>

#define MAX_PORT_NAME 8

#define ETH_ALEN        6               /* Octets in one ethernet addr   */
#define ETH_HLEN        14              /* Total octets in header.       */
#define ETH_ZLEN        60              /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN    1500            /* Max. octets in payload        */
#define ETH_FRAME_LEN   1514            /* Max. octets in frame sans FCS */
#define ETH_FCS_LEN     4               /* Octets in the FCS             */

#define ETH_P_802_3     0x0001          /* Dummy type for 802.3 frames  */
#define ETH_P_AX25      0x0002          /* Dummy protocol id for AX.25  */
#define ETH_P_ALL       0x0003          /* Every packet (be careful!!!) */
#define ETH_P_802_2     0x0004          /* 802.2 frames                 */
#define ETH_P_SNAP      0x0005          /* Internal only                */
#define ETH_P_DDCMP     0x0006          /* DEC DDCMP: Internal only     */
#define ETH_P_WAN_PPP   0x0007          /* Dummy type for WAN PPP frames*/
#define ETH_P_PPP_MP    0x0008          /* Dummy type for PPP MP frames */
#define ETH_P_LOCALTALK 0x0009          /* Localtalk pseudo type        */
#define ETH_P_CAN       0x000C          /* Controller Area Network      */
#define ETH_P_PPPTALK   0x0010          /* Dummy type for Atalk over PPP*/
#define ETH_P_TR_802_2  0x0011          /* 802.2 frames                 */
#define ETH_P_MOBITEX   0x0015          /* Mobitex (kaz@cafe.net)       */
#define ETH_P_CONTROL   0x0016          /* Card specific control frames */
#define ETH_P_IRDA      0x0017          /* Linux-IrDA                   */
#define ETH_P_ECONET    0x0018          /* Acorn Econet                 */
#define ETH_P_HDLC      0x0019          /* HDLC frames                  */
#define ETH_P_ARCNET    0x001A          /* 1A for ArcNet :-)            */
#define ETH_P_DSA       0x001B          /* Distributed Switch Arch.     */

/* Ethernet protocol ID's */
#define ETHERTYPE_PUP           0x0200          /* Xerox PUP */
#define ETHERTYPE_SPRITE        0x0500          /* Sprite */
#define ETHERTYPE_IP            0x0800          /* IP */
#define ETHERTYPE_ARP           0x0806          /* Address resolution */
#define ETHERTYPE_REVARP        0x8035          /* Reverse ARP */
#define ETHERTYPE_AT            0x809B          /* AppleTalk protocol */
#define ETHERTYPE_AARP          0x80F3          /* AppleTalk ARP */
#define ETHERTYPE_VLAN          0x8100          /* IEEE 802.1Q VLAN tagging */
#define ETHERTYPE_IPX           0x8137          /* IPX */
#define ETHERTYPE_IPV6          0x86dd          /* IP protocol version 6 */
#define ETHERTYPE_LOOPBACK      0x9000          /* used to test interfaces */


#define ETHER_ADDR_LEN  ETH_ALEN                 /* size of ethernet addr */
#define ETHER_TYPE_LEN  2                        /* bytes in type field */
#define ETHER_CRC_LEN   4                        /* bytes in CRC field */
#define ETHER_HDR_LEN   ETH_HLEN                 /* total octets in header */
#define ETHER_MIN_LEN   (ETH_ZLEN + ETHER_CRC_LEN) /* min packet length */
#define ETHER_MAX_LEN   (ETH_FRAME_LEN + ETHER_CRC_LEN) /* max packet length */


#define MAX_PORTS  12 

#define LLC_PDU_TYPE_I  0       /* first bit */
#define LLC_PDU_TYPE_S  1       /* first two bits */
#define LLC_PDU_TYPE_U  3       /* first two bits */
#define LLC_1_PDU_CMD_UI       0x00     /* Type 1 cmds/rsps */


/* LLC SAP types. */
#define LLC_SAP_NULL    0x00            /* NULL SAP.                    */
#define LLC_SAP_LLC     0x02            /* LLC Sublayer Management.     */
#define LLC_SAP_SNA     0x04            /* SNA Path Control.            */
#define LLC_SAP_PNM     0x0E            /* Proway Network Management.   */
#define LLC_SAP_IP      0x06            /* TCP/IP.                      */
#define LLC_SAP_BSPAN   0x42            /* Bridge Spanning Tree Proto   */
#define LLC_SAP_MMS     0x4E            /* Manufacturing Message Srv.   */
#define LLC_SAP_8208    0x7E            /* ISO 8208                     */
#define LLC_SAP_3COM    0x80            /* 3COM.                        */
#define LLC_SAP_PRO     0x8E            /* Proway Active Station List   */
#define LLC_SAP_SNAP    0xAA            /* SNAP.                        */
#define LLC_SAP_BANYAN  0xBC            /* Banyan.                      */
#define LLC_SAP_IPX     0xE0            /* IPX/SPX.                     */
#define LLC_SAP_NETBEUI 0xF0            /* NetBEUI.                     */
#define LLC_SAP_LANMGR  0xF4            /* LanManager.                  */
#define LLC_SAP_IMPL    0xF8            /* IMPL                         */
#define LLC_SAP_DISC    0xFC            /* Discovery                    */
#define LLC_SAP_OSI     0xFE            /* OSI Network Layers.          */
#define LLC_SAP_LAR     0xDC            /* LAN Address Resolution       */
#define LLC_SAP_RM      0xD4            /* Resource Management          */
#define LLC_SAP_GLOBAL  0xFF            /* Global SAP.                  */

    
/* ARP protocol HARDWARE identifiers. */
#define ARPHRD_NETROM   0               /* from KA9Q: NET/ROM pseudo    */
#define ARPHRD_ETHER    1               /* Ethernet 10Mbps              */
#define ARPHRD_EETHER   2               /* Experimental Ethernet        */
#define ARPHRD_AX25     3               /* AX.25 Level 2                */
#define ARPHRD_PRONET   4               /* PROnet token ring            */
#define ARPHRD_CHAOS    5               /* Chaosnet                     */
#define ARPHRD_IEEE802  6               /* IEEE 802.2 Ethernet/TR/TB    */
#define ARPHRD_ARCNET   7               /* ARCnet                       */
#define ARPHRD_APPLETLK 8               /* APPLEtalk                    */
#define ARPHRD_DLCI     15              /* Frame Relay DLCI             */
#define ARPHRD_ATM      19              /* ATM                          */
#define ARPHRD_METRICOM 23              /* Metricom STRIP (new IANA id) */
#define ARPHRD_IEEE1394 24              /* IEEE 1394 IPv4 - RFC 2734    */
#define ARPHRD_EUI64    27              /* EUI-64                       */
#define ARPHRD_INFINIBAND 32            /* InfiniBand                   */

/* ARP protocol opcodes. */
#define ARPOP_REQUEST   1               /* ARP request                  */
#define ARPOP_REPLY     2               /* ARP reply                    */
#define ARPOP_RREQUEST  3               /* RARP request                 */
#define ARPOP_RREPLY    4               /* RARP reply                   */
#define ARPOP_InREQUEST 8               /* InARP request                */
#define ARPOP_InREPLY   9               /* InARP reply                  */
#define ARPOP_NAK       10              /* (ATM)ARP NAK                 */


typedef struct mac_addr
{       
        unsigned char   addr[6];
}MACADDRESS;

struct ether_hdr
{
	MACADDRESS  dmac;      /* destination eth addr */
	MACADDRESS  smac;      /* source ether addr    */
	uint16_t type;                 /* packet type ID field */
};

struct ether_header
{
  u_int8_t  ether_dhost[ETH_ALEN];      /* destination eth addr */
  u_int8_t  ether_shost[ETH_ALEN];      /* source ether addr    */
  u_int16_t ether_type;                 /* packet type ID field */
} __attribute__ ((__packed__));


typedef struct bridge_id
{       
        uint16_t   prio;
        unsigned char   addr[6];
}BRIDGEID;

/* Un-numbered PDU format (3 bytes in length) */
struct llc_pdu_un {
        uint8_t dsap;
        uint8_t ssap;
        uint8_t ctrl_1;
};


typedef uint16_t PORTID;
typedef uint32_t PORT;
typedef uint32_t TIMEOUT;


enum {
	TRUE = 1,
	FALSE = 0
};

enum {
	IF_UP = 1,
	IF_DOWN = 2
};

typedef int TRUTH;
typedef int TIMESTMP;
typedef void * TIMER_ID;

typedef struct mac_hdr {
  MACADDRESS dest;
  MACADDRESS src;
  uint16_t  len8023;
} MACHDR;

typedef struct eth_hdr {
  uint8_t   dsap;
  uint8_t   ssap;
  uint8_t   llc;
} ETHHDR;

typedef enum {
  P2P_FORCE_TRUE,
  P2P_FORCE_FALSE,
  P2P_AUTO,
} ADMIN_P2P_T;

#define S_DISABLED 0
#define S_LISTENING 1
#define S_DISCARDING 1
#define S_LEARNING 2
#define S_FORWARDING 3
#define S_BLOCKING 4

#define STP_ENABLED 1
#define STP_DISABLED 0

#define ADMIN_PORT_PATH_COST_AUTO   0

#define MAX_BITS_PER_BYTE 8
typedef char PORTLIST [MAX_PORTS / MAX_BITS_PER_BYTE];

#define VLAN_INVALID_ID  0X7FFF
#define VLAN_DEFAULT_VLAN_ID 1

#define MAX_VLAN_NAME  8
#define MAX_VLAN_BITS 12
#define MAX_VLAN_IDS  (1 << MAX_VLAN_BITS) 

enum ROWSTATUS  {
	ROW_STATE_CREATE_WAIT = 5
};

#define VLAN_IS_NOT_IN_RANGE(vid)  (vid <= 0 || vid > 4096)

#define TAGGED 1
#define UNTAGGED 2
#define FORBIDDEN 3


#define MODE_STP 1
#define MODE_RSTP 2


enum STP_DEF_VALUES {
	STP_DEF_PRIORITY = 32768,
	STP_DEF_MAX_AGE = 20,
	STP_DEF_HELLO_TIME = 2,
	STP_DEF_FWD_DELAY = 15,
	STP_DEF_HOLD_COUNT = 6,
	STP_DEF_PORT_PRIO = 128,
	STP_DEF_DESG_COST = 20000,
	STP_DEF_PATH_COST = 20000
};

static const uint8_t br_group_address[ETH_ALEN] = { 0x01, 0x80, 0xc2, 
                                              0x00, 0x00, 0x00 };

int get_port_mac_address (uint32_t port, uint8_t *mac);
int  compare_ether_addr(const uint8_t *addr1, const uint8_t *addr2);
int get_port_oper_state (uint32_t port);
inline int bridge_timer_relation (int fdelay, int max_age, int hello);

#include "libproto.h"
#endif

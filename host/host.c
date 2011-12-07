#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>      
#include <sys/stat.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <getopt.h>
#include <netdb.h>

#define MAX_PORT_NAME 8

#define ETH_ALEN        6

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


;


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
typedef int TIMER_ID;

#define TM_VIRT_PORT 9000
#define BASE_HOST_PORT 10
#define TM_INST_PORT  TM_VIRT_PORT  + BASE_HOST_PORT + tm_inst
#define MAX_MTU 1500

void vlink_processing_task (void *unused);
void tx_pkt (void *buf,  int len);
extern void dump_task_info (void);

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
typedef struct inst {
	uint8_t inst;
	uint8_t port;
}inst_t;
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
#pragma pack(pop)   /* restore original alignment from stack */

static int32_t  sockid = 0;
static uint32_t tm_virt_ip = 0;
static uint32_t tm_ifindex = 0;
static uint32_t tm_inst = 0;
static char     tm_ifname[IFNAMSIZ] = "lo";


void send_packet (void *buf, uint16_t port, int len)
{
	char pkt[MAX_MTU];

	memset (pkt, 0, MAX_MTU);

	pkt[0] = tm_inst;
	pkt[1] = (uint8_t)port;

	memcpy (&pkt[2], buf, len);

	tx_pkt (pkt, len + sizeof(inst_t));
}

void tx_pkt (void *buf, int len) 
{
	struct sockaddr_in dest_addr;

	memset (&dest_addr, 0, sizeof(dest_addr));

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(TM_VIRT_PORT);
	dest_addr.sin_addr.s_addr = htonl (tm_virt_ip);

	if (sendto (sockid, buf, len, 0,(struct sockaddr *)&dest_addr, 
				sizeof(dest_addr)) < 0) {
		perror ("-ERR- SENDTO: ");
	}
	return;
}


int ifname_info (char *ifname)
{
	struct ifreq	ifr;
	int		fd	= socket(AF_INET, SOCK_DGRAM, 0);
	uint32_t	addr	= 0;

	if (fd < 0) 	
		return (-1);

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

	if (ioctl(fd, SIOCGIFADDR, (char *)&ifr) == 0) {
		struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
		tm_virt_ip = ntohl(sin->sin_addr.s_addr);
	}

	if (ioctl(fd, SIOCGIFINDEX, (char *)&ifr) == 0)
		tm_ifindex = ifr.ifr_ifindex;

	close(fd);

	if (!tm_virt_ip)
		return -1;

	return 0;
}

int create_communication_channel (void)
{
	struct sockaddr_in si_me;

	memset((char *) &si_me, 0, sizeof(si_me));

	if ((sockid =socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror ("SOCKET");
		return -1;
	}

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(TM_INST_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockid, &si_me, sizeof(si_me)) < 0) {
		perror ("BIND");
		return -1;
	}
	return  0;
}
int parse_cmdline (int argc, char *argv[])
{
	tm_inst = atoi(argv[1]);

	ifname_info (tm_ifname);

	create_communication_channel ();	
}

int main (int argc, char **argv)
{
	int tid = 0;

	argc = parse_cmdline (argc, argv );

	if( argc < 0 ) {
		return -1;
	}

	create_communication_channel ();

	printf ("######## Host Started #######\n");

	while (1) {
		build_and_send_simple_packet ();
	}
}

int build_and_send_simple_packet (void)
{
	char pkt[MAX_MTU];

	static int i = 0;

	struct ether_hdr *eth = (struct ether_hdr *)pkt;

	uint8_t mac_addr[6] = {0x0,0x1,0x2,0x3,0x4,0x11};
	uint8_t dmac_addr[6] = {0x0,0x1,0x2,0x3,0x4,0x11};

	while (1) {
		int j = 0;
		for (j = 1; j <=12;j++) {
			while (1) {

				memset (pkt, 0, sizeof(pkt));

				if (i == 50) {
					sleep (1);
					i = 0;
					break;
				}
				mac_addr[1] = j;
				mac_addr[2] = (uint8_t)i;

				memcpy(eth->smac.addr, mac_addr, ETH_ALEN);

				memcpy(eth->dmac.addr, dmac_addr, ETH_ALEN);

				memset (pkt + sizeof(struct ether_hdr), 1, MAX_MTU - 
						sizeof(struct ether_hdr));

				i++;

				send_packet (pkt, j,MAX_MTU);
			}
		}
	}
}

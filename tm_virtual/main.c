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
#include "list.h"


#define VERSION "0.1"

#define TM_VIRT_PORT 9000
#define tm_inst_port(inst)  TM_VIRT_PORT  + inst

void vlink_processing_task (void *unused);
void tx_pkt (void *buf, int dest, int len);
extern void dump_task_info (void);
int open_message_queue (uint8_t inst);

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
typedef struct mac_addr
{       
        unsigned char   addr[6];
}MACADDRESS;


 struct mac_hdr {
  MACADDRESS dest;
  MACADDRESS src;
} MACHDR;

struct eth_hdr {
  uint16_t  len8023;
  uint8_t   dsap;
  uint8_t   ssap;
  uint8_t   llc;
} ETHHDR;


typedef struct inst {
	uint8_t inst;
	uint8_t port;
}inst_t;

typedef struct v_link {
	struct list_head next;
	int    sock_fd[2];
	inst_t inst[2];
	uint32_t pkt_count;
}vlink_t;
#pragma pack(pop)   /* restore original alignment from stack */

static struct   list_head vlink;

static int32_t  sockid = 0;
static uint32_t tm_virt_ip = 0;
static uint32_t tm_ifindex = 0;
static char     tm_ifname[IFNAMSIZ] = "lo";

int show_vlinks (char *args[], int count)
{
	vlink_t *vnode = NULL;
	struct list_head *p = NULL;
	int i = 0;

	fprintf (stdout, "V-Links\n");

	list_for_each (p, &vlink) {
		vnode = list_entry (p, vlink_t, next);
			fprintf (stdout, "%d  [inst %d]p%-d <-----> p%-d[inst %d] "
                                 "Up , Packets : %u\n", 
                                 i, vnode->inst[0].inst, vnode->inst[0].port, 
				 vnode->inst[1].inst, vnode->inst[1].port, 
                                 vnode->pkt_count);
			i++;
	}
	return 0;
}


int destroy_virtual_link (char *args[], int count)
{
	vlink_t new, *vnode = NULL;
	struct list_head *p = NULL, *n = NULL;
	inst_t inst[2];

	inst[0].inst = atoi (args[0]);
	inst[0].port = atoi (args[1]);
	inst[1].inst = atoi (args[2]);
	inst[1].port = atoi (args[3]);

	list_for_each_safe (p, n, &vlink) {
		vnode = list_entry (p, vlink_t, next);
		if ((vnode->inst[0].inst == inst[0].inst) &&
		    (vnode->inst[0].port == inst[0].port) &&
		    (vnode->inst[1].inst == inst[1].inst) &&
                    (vnode->inst[1].port == inst[1].port)) {
			list_del (&vnode->next);
			fprintf (stdout, "V-Link Deleted Successfully [inst %d]p%-d <-----> p%-d[inst %d] \n", 
		 		 inst[0].inst, inst[0].port, inst[1].inst, inst[1].port);
			free (vnode);
			return 0;
		}
	}
	fprintf (stdout, "Invalid V-Link\n");
	return -1;
}

int create_virtual_link (char *args[], int count)
{
	vlink_t *p =NULL;
	uint8_t inst[2];
	uint8_t port[2];

	inst[0] = atoi (args[0]);
	port[0] = atoi (args[1]);
	inst[1] = atoi (args[2]);
	port[1] = atoi (args[3]);

	if (!(p = (vlink_t *) malloc (sizeof(vlink_t)))) {
		printf ("Out of Memory\n");
		return -1;
	}

	INIT_LIST_HEAD (&p->next);
	p->inst[0].inst = inst[0];
	p->inst[0].port = port[0];
	p->inst[1].inst = inst[1];
	p->inst[1].port = port[1];

	list_add_tail (&p->next, &vlink);

	fprintf (stdout, "New V-Link Created Successfully [inst %d]p%-d <-----> p%-d[inst %d] \n", 
		 inst[0], port[0], inst[1], port[1]);

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
        si_me.sin_port = htons(TM_VIRT_PORT);
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sockid, &si_me, sizeof(si_me)) < 0) {
                perror ("BIND");
                return -1;
        }
        return  0;
}


void vlink_processing_task (void *unused)
{
	int len = 0;

	char pkt_buf[ETH_FRAME_LEN];

	memset (pkt_buf, 0, sizeof(pkt_buf));

	while (1) {
		len = rcv_pkt (pkt_buf);
		if (len < 0) {
			continue;
		}
		process_pkt (pkt_buf, len);
	}
}

int process_pkt (char *buf, int len)
{
	vlink_t *vnode = NULL;
	struct list_head *p = NULL, *n = NULL;
	inst_t *pkt_hdr = NULL;
	int tx = 0;
	int dest = 0;

	pkt_hdr = (inst_t *)buf;

#ifdef DBG
	printf ("Pakcet Rx'd of len %d from inst %d on port %d\n\n", len, pkt_hdr->inst, pkt_hdr->port);
#endif

	list_for_each_safe (p, n, &vlink) {

		int inst = -1;
		vnode = list_entry (p, vlink_t, next);
		if (!memcmp (&vnode->inst[0], pkt_hdr, sizeof(inst_t))) {
			inst = 1;
			goto tx_pkt;
		} else if (!memcmp (&vnode->inst[1], pkt_hdr, sizeof(inst_t))) {
			inst = 0;
			goto tx_pkt;
		} else {
#ifdef DBG
			printf ("Pkt  is dropped\n");
#endif
			continue;
		}
tx_pkt:
		if (tx = 1) { /*XXX:It is = only not == */
			dest = tm_inst_port (vnode->inst[inst].inst);

			memcpy (&pkt_hdr->inst, &vnode->inst[inst], sizeof(inst_t));

			vnode->pkt_count++;

			tx_pkt (buf, dest, len);
		}
	}
}

void tx_pkt (void *buf, int dest, int len) 
{
	struct sockaddr_in dest_addr;

	memset (&dest_addr, 0, sizeof(dest_addr));

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(dest);
	dest_addr.sin_addr.s_addr = htonl (tm_virt_ip);

	if (sendto (sockid, buf, len, 0,(struct sockaddr *)&dest_addr, 
				sizeof(dest_addr)) < 0) {
		perror ("-ERR- SENDTO: ");
	}
	return;
}


int parse_cmdline (int argc, char *argv[])
{
	if (ifname_info (tm_ifname) < 0) {
		fprintf(stderr, "no interface found!\n" );
		return -1;
	}
	return 0;
}

void usage (void)
{
	fprintf( stderr, "TM Virtual Manager %s\n", VERSION);
	fprintf( stderr, "Usage: tm_virtual i ifname\n" );
	fprintf( stderr, "i ifname: the interface name to run on\n" );
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
	
	memcpy (tm_ifname, ifname, IFNAMSIZ);

	close(fd);

	if (!tm_virt_ip)
		return -1;

	return 0;
}

int rcv_pkt (void *buf)
{
	int len = 0;
	len = tm_recvfrom (sockid, buf, ETH_FRAME_LEN, 0, NULL, NULL);
         return len;
}
void switch_to_os_threading (void);
int main (int argc, char **argv)
{
	int tid = 0;

	argc = parse_cmdline (argc, argv );

	if( argc < 0 ) {
		return -1;
	}

	tmlib_init ();

	create_communication_channel ();

	INIT_LIST_HEAD (&vlink);

	task_create ("VLNK", 3, 3, 32000, vlink_processing_task, NULL, NULL, &tid);

	create_cmdline_interface ("tm_virtual");

	set_curr_priv_level (1);

	write_string ("######## TM Virtual Started #######\n");

	install_cmd_handler ("vlink inst <no> port <no> inst <no> port <no>", 
			"Creates virtual link between two instance", 
			create_virtual_link, NULL, 1);

	install_cmd_handler ("no vlink inst <no> port <no> inst <no> port <no>", 
			"Creates virtual link between two instance", 
			destroy_virtual_link, NULL, 1);

#if 0
        install_cmd_handler ("task", "dumps task info", dump_task_info, NULL, 1);
#endif

        install_cmd_handler ("show vlink", "Displays all virtual links", show_vlinks, NULL, 1);

	while (1) {
		sleep_forever ();
	}
}

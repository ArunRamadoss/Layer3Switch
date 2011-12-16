/*==========================[ (c) JME SOFT  ]===================================
FILE        : [vrrp.c]
CREATED     : 00/02/02 12:54:37		LAST SAVE    : 00/10/04 22:11:39
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================
- This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version
  2 of the License, or (at your option) any later version.
==============================================================================*/

/* system include */
#include <stdio.h>
#include <assert.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/errno.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>

/* local include */
#include "vrrpd.h"
#include "ipaddr.h"

int ip_id = 0;	/* to have my own ip_id creates collision with kernel ip->id
		** but it should be ok because the packets are unlikely to be
		** fragmented (they are non routable and small) */
		/* WORK: this packet isnt routed, i can check the outgoing MTU
		** to warn the user only if the outoing mtu is too small */
static char vrrp_hwaddr[6];	// WORK: lame hardcoded for ethernet
static vrrp_rt	glob_vsrv;	/* a global because used in the signal handler*/

static char	PidDir[FILENAME_MAX+1];


/****************************************************************
 NAME	: get_pid_name				00/10/04 21:06:44
 AIM	: 
 REMARK	:
****************************************************************/
static char *pidfile_get_name( vrrp_rt *vsrv )
{
	static char pidfile[FILENAME_MAX+1];
	snprintf( pidfile, sizeof(pidfile), "%s/" VRRP_PID_FORMAT
					, PidDir
					, vsrv->vif.ifname 
					, vsrv->vrid );
	return pidfile;
}

/****************************************************************
 NAME	: pidfile_write				00/10/04 21:12:26
 AIM	: 
 REMARK	: write the pid file
****************************************************************/
static int pidfile_write( vrrp_rt *vsrv )
{
	char	*name	= pidfile_get_name(vsrv);
	FILE	*fOut	= fopen( name, "w" );
	if( !fOut ){
		fprintf( stderr, "Can't open %s (errno %d %s)\n", name 
						, errno
						, strerror(errno) 
						);
		return -1;
	}
	fprintf( fOut, "%d\n", getpid() );
	fclose( fOut );
	return(0);
}

/****************************************************************
 NAME	: pidfile_rm				00/10/04 21:12:26
 AIM	: 
 REMARK	:
****************************************************************/
static void pidfile_rm( vrrp_rt *vsrv )
{
	unlink( pidfile_get_name(vsrv) );
}

/****************************************************************
 NAME	: pidfile_exist				00/10/04 21:12:26
 AIM	: return 0 if there is no valid pid in the pidfile or no pidfile
 REMARK	: 
****************************************************************/
static int pidfile_exist( vrrp_rt *vsrv )
{
	char	*name	= pidfile_get_name(vsrv);
	FILE	*fIn	= fopen( name, "r" );
	pid_t	pid;
	/* if there is no file */
	if( !fIn )		return 0;
	fscanf( fIn, "%d", &pid );
	fclose( fIn );
	/* if there is no process, remove the stale file */
	if( kill( pid, 0 ) ){
		fprintf(stderr, "Remove a stale pid file %s\n", name );
		pidfile_rm( vsrv );
		return 0;
	}
	/* if the kill suceed, return an error */
	return -1;
}



/****************************************************************
 NAME	: in_csum				00/05/10 20:12:20
 AIM	: compute a IP checksum
 REMARK	: from kuznet's iputils
****************************************************************/
static u_short in_csum( u_short *addr, int len, u_short csum)
{
	register int nleft = len;
	const u_short *w = addr;
	register u_short answer;
	register int sum = csum;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1)
		sum += htons(*(u_char *)w << 8);

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}


/****************************************************************
 NAME	: get_dev_from_ip			00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static uint32_t ifname_to_ip( char *ifname )
{
	struct ifreq	ifr;
	int		fd	= socket(AF_INET, SOCK_DGRAM, 0);
	uint32_t	addr	= 0;
	if (fd < 0) 	return (-1);
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFADDR, (char *)&ifr) == 0) {
		struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
		addr = ntohl(sin->sin_addr.s_addr);
	}
	close(fd);
	return addr;
}

/****************************************************************
 NAME	: get_dev_from_ip			00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static int ifname_to_idx( char *ifname )
{
	struct ifreq	ifr;
	int		fd	= socket(AF_INET, SOCK_DGRAM, 0);
	int		ifindex = -1;
	if (fd < 0) 	return (-1);
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFINDEX, (char *)&ifr) == 0)
		ifindex = ifr.ifr_ifindex;
	close(fd);
	return ifindex;
}

/****************************************************************
 NAME	: rcvhwaddr_op				00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static int rcvhwaddr_op( char *ifname, char *addr, int addrlen, int addF )
{
	struct ifreq	ifr;
	int		fd	= socket(AF_INET, SOCK_DGRAM, 0);
	int		ret;
	if (fd < 0) 	return (-1);
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	memcpy( ifr.ifr_hwaddr.sa_data, addr, addrlen );
	ifr.ifr_hwaddr.sa_family = AF_UNSPEC;
	ret = ioctl(fd, addF ? SIOCADDMULTI : SIOCDELMULTI, (char *)&ifr);
	if( ret ){
		printf("Can't %s on %s. errno=%d\n"
			, addF ? "SIOCADDMULTI" : "SIOCDELMULTI"
			, ifname, errno );
	}
	close(fd);
	return ret;
}

/****************************************************************
 NAME	: hwaddr_set				00/02/08 06:51:32
 AIM	:
 REMARK	: linux refuse to change the hwaddress if the interface is up
****************************************************************/
static int hwaddr_set( char *ifname, char *addr, int addrlen )
{
	struct ifreq	ifr;
	int		fd	= socket(AF_INET, SOCK_DGRAM, 0);
	int		ret;
	unsigned long	flags;
	if (fd < 0) 	return (-1);
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	/* get the flags */
	ret = ioctl(fd, SIOCGIFFLAGS, (char *)&ifr);
	if( ret )	goto end;
	flags = ifr.ifr_flags;
	/* set the interface down */
	ifr.ifr_flags &= ~IFF_UP;
	ret = ioctl(fd, SIOCSIFFLAGS, (char *)&ifr);
	if( ret )	goto end;
	/* change the hwaddr */
	memcpy( ifr.ifr_hwaddr.sa_data, addr, addrlen );
	ifr.ifr_hwaddr.sa_family = AF_UNIX;
	ret = ioctl(fd, SIOCSIFHWADDR, (char *)&ifr);
	if( ret )	goto end;
	/* set the interface up */
	ifr.ifr_flags = flags;
	ret = ioctl(fd, SIOCSIFFLAGS, (char *)&ifr);
	if( ret )	goto end;
end:;
if( ret )	printf("error errno=%d\n",errno);

	close(fd);
	return ret;
}

/****************************************************************
 NAME	: hwaddr_get				00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static int hwaddr_get( char *ifname, char *addr, int addrlen )
{
	struct ifreq	ifr;
	int		fd	= socket(AF_INET, SOCK_DGRAM, 0);
	int		ret;
	if (fd < 0) 	return (-1);
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	ret = ioctl(fd, SIOCGIFHWADDR, (char *)&ifr);
	memcpy( addr, ifr.ifr_hwaddr.sa_data, addrlen );
//printf("%x:%x:%x:%x:%x:%x\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5] );
	close(fd);
	return ret;
}


/****************************************************************
 NAME	: ipaddr_ops				00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static int ipaddr_ops( vrrp_rt *vsrv, int addF )
{
	int	i, err	= 0;
	int	ifidx	= ifname_to_idx( vsrv->vif.ifname );
	struct in_addr in;

	for( i = 0; i < vsrv->naddr; i++ ){
		vip_addr	*vadd = &vsrv->vaddr[i];
		if( !addF && !vadd->deletable ) 	continue;

		if( ipaddr_op( ifidx , vadd->addr, addF)){
			err = 1;
			vadd->deletable = 0;
			in.s_addr = htonl(vadd->addr);
			VRRP_LOG(("cant %s the address %s to %s\n"
						, addF ? "set" : "remove"
						, inet_ntoa(in)
						, vsrv->vif.ifname));
		}else{
			vadd->deletable = 1;
		}
	}
	return err;
}

/****************************************************************
 NAME	: vrrp_dlthd_len			00/02/02 15:16:23
 AIM	: return the vrrp header size in byte
 REMARK	:
****************************************************************/
static int vrrp_dlt_len( vrrp_rt *rt )
{
	return ETHER_HDR_LEN;	/* hardcoded for ethernet */
}

/****************************************************************
 NAME	: vrrp_iphdr_len			00/02/02 15:16:23
 AIM	: return the ip  header size in byte
 REMARK	:
****************************************************************/
static int vrrp_iphdr_len( vrrp_rt *vsrv )
{
	return sizeof( struct iphdr );
}

/****************************************************************
 NAME	: vrrp_hd_len				00/02/02 15:16:23
 AIM	: return the vrrp header size in byte
 REMARK	:
****************************************************************/
static int vrrp_hd_len( vrrp_rt *vsrv )
{
	return sizeof( vrrp_pkt ) + vsrv->naddr*sizeof(uint32_t)
						+ VRRP_AUTH_LEN;
}

/****************************************************************
 NAME	: vrrp_in_chk				00/02/02 12:54:54
 AIM	: check a incoming packet. return 0 if the pkt is valid, != 0 else
 REMARK	: rfc2338.7.1
****************************************************************/
static int vrrp_in_chk( vrrp_rt *vsrv, struct iphdr *ip )
{
	int		ihl = ip->ihl << 2;
	vrrp_pkt *	hd = (vrrp_pkt *)((char *)ip + ihl);
	vrrp_if 	*vif	= &vsrv->vif;
	/* MUST verify that the IP TTL is 255 */
	if( ip->ttl != VRRP_IP_TTL ) {
		VRRP_LOG(("invalid ttl. %d and expect %d", ip->ttl,VRRP_IP_TTL));
		return 1;
	}
	/* MUST verify the VRRP version */
	if( (hd->vers_type >> 4) != VRRP_VERSION ){
		VRRP_LOG(("invalid version. %d and expect %d"
			, (hd->vers_type >> 4), VRRP_VERSION));
		return 1;
	}
	/* MUST verify that the received packet length is greater than or
	** equal to the VRRP header */
	if( (ntohs(ip->tot_len)-ihl) <= sizeof(vrrp_pkt) ){
		VRRP_LOG(("ip payload too short. %d and expect at least %d"
			, ntohs(ip->tot_len)-ihl, sizeof(vrrp_pkt) ));
		return 1;
	}
	/* WORK: MUST verify the VRRP checksum */
	if( in_csum( (u_short*)hd, vrrp_hd_len(vsrv), 0) ){
		VRRP_LOG(("Invalid vrrp checksum" ));
		return 1;
	}
/* MUST perform authentication specified by Auth Type */
 	/* check the authentication type */
	if( vif->auth_type != hd->auth_type ){		
		VRRP_LOG(("receive a %d auth, expecting %d!", vif->auth_type
							, hd->auth_type));
		return 1;
	}
	/* check the authentication if it is a passwd */
	if( hd->auth_type != VRRP_AUTH_PASS ){
		char	*pw	= (char *)ip + ntohs(ip->tot_len)
				  -sizeof(vif->auth_data);
		if( memcmp( pw, vif->auth_data, sizeof(vif->auth_data)) ){
			VRRP_LOG(("receive an invalid passwd!"));
			return 1;
		}
	}

	/* MUST verify that the VRID is valid on the receiving interface */
	if( vsrv->vrid != hd->vrid ){
		return 1;
	}

	/* MAY verify that the IP address(es) associated with the VRID are
	** valid */
	/* WORK: currently we don't */

	/* MUST verify that the Adver Interval in the packet is the same as
	** the locally configured for this virtual router */
	if( vsrv->adver_int/VRRP_TIMER_HZ != hd->adver_int ){
		VRRP_LOG(("advertissement interval mismatch mine=%d rcved=%d"
			, vsrv->adver_int, hd->adver_int ));
		return 1;
	}

	return 0;
}

/****************************************************************
 NAME	: vrrp_build_dlt			00/02/02 14:39:18
 AIM	:
 REMARK	: rfc2338.7.3
****************************************************************/
static void vrrp_build_dlt( vrrp_rt *vsrv, char *buffer, int buflen )
{
	/* hardcoded for ethernet */
	struct ether_header *	eth = (struct ether_header *)buffer;
	/* destination address --rfc1122.6.4*/
	eth->ether_dhost[0]	= 0x01;
	eth->ether_dhost[1]	= 0x00;
	eth->ether_dhost[2]	= 0x5E;
	eth->ether_dhost[3]	= (INADDR_VRRP_GROUP >> 16) & 0x7F;
	eth->ether_dhost[4]	= (INADDR_VRRP_GROUP >>  8) & 0xFF;
	eth->ether_dhost[5]	=  INADDR_VRRP_GROUP        & 0xFF;
	/* source address --rfc2338.7.3 */
	memcpy( eth->ether_shost, vrrp_hwaddr, sizeof(vrrp_hwaddr));
	/* type */
	eth->ether_type		= htons( ETHERTYPE_IP );
}

/****************************************************************
 NAME	: vrrp_build_ip				00/02/02 14:39:18
 AIM	: build a ip packet
 REMARK	:
****************************************************************/
static void vrrp_build_ip( vrrp_rt *vsrv, char *buffer, int buflen )
{
	struct iphdr * ip = (struct iphdr *)(buffer);
	ip->ihl		= 5;
	ip->version	= 4;
	ip->tos		= 0;
	ip->tot_len	= ip->ihl*4 + vrrp_hd_len( vsrv );
	ip->tot_len	= htons(ip->tot_len);
	ip->id		= ++ip_id;
	ip->frag_off	= 0;
	ip->ttl		= VRRP_IP_TTL;
	ip->protocol	= IPPROTO_VRRP;
	ip->saddr	= htonl(vsrv->vif.ipaddr);
	ip->daddr	= htonl(INADDR_VRRP_GROUP);
	/* checksum must be done last */
	ip->check	= in_csum( (u_short*)ip, ip->ihl*4, 0 );
}

/****************************************************************
 NAME	: vrrp_build_vrrp			00/02/02 14:39:18
 AIM	:
 REMARK	:
****************************************************************/
static int vrrp_build_vrrp( vrrp_rt *vsrv, int prio, char *buffer, int buflen )
{
	int	i;
	vrrp_if	 *vif	= &vsrv->vif;
	vrrp_pkt *hd	= (vrrp_pkt *)buffer;
	uint32_t *iparr	= (uint32_t *)((char *)hd+sizeof(*hd));
	
	hd->vers_type	= (VRRP_VERSION<<4) | VRRP_PKT_ADVERT;
	hd->vrid	= vsrv->vrid;
	hd->priority	= prio;
	hd->naddr	= vsrv->naddr;
	hd->auth_type	= vsrv->vif.auth_type;
	hd->adver_int	= vsrv->adver_int/VRRP_TIMER_HZ;
	/* copy the ip addresses */
	for( i = 0; i < vsrv->naddr; i++ ){
		iparr[i] = htonl( vsrv->vaddr[i].addr );
	}
	hd->chksum	= in_csum( (u_short*)hd, vrrp_hd_len(vsrv), 0);
	/* copy the passwd if the authentication is VRRP_AH_PASS */
	if( vif->auth_type == VRRP_AUTH_PASS ){
		char	*pw	= (char *)hd+sizeof(*hd)+vsrv->naddr*4;
		memcpy( pw, vif->auth_data, sizeof(vif->auth_data));
	}
	return(0);
}

/****************************************************************
 NAME	: vrrp_set_ptk				00/02/02 13:33:32
 AIM	: build a advertissement packet
 REMARK	:
****************************************************************/
static void vrrp_build_pkt( vrrp_rt *vsrv, int prio, char *buffer, int buflen )
{
//	printf("dltlen=%d iplen=%d", vrrp_dlt_len(vsrv), vrrp_iphdr_len(vsrv) );
	/* build the ethernet header */
	vrrp_build_dlt( vsrv, buffer, buflen );
	buffer += vrrp_dlt_len(vsrv);
	buflen -= vrrp_dlt_len(vsrv);
	/* build the ip header */
	vrrp_build_ip( vsrv, buffer, buflen );
	buffer += vrrp_iphdr_len(vsrv);
	buflen -= vrrp_iphdr_len(vsrv);
	/* build the vrrp header */
	vrrp_build_vrrp( vsrv, prio, buffer, buflen );
}

/****************************************************************
 NAME	: vrrp_send_pkt				00/02/06 16:37:10
 AIM	:
 REMARK	:
****************************************************************/
static int vrrp_send_pkt( vrrp_rt *vsrv, char *buffer, int buflen )
{
	struct sockaddr from;
	int	len;
	int	fd = socket(PF_PACKET, SOCK_PACKET, 0x300); /* 0x300 is magic */
// WORK:
	if( fd < 0 ){
		perror( "socket" );
		return -1;
	}
	/* build the address */
	memset( &from, 0 , sizeof(from));
	strcpy( from.sa_data, vsrv->vif.ifname );
	/* send the data */
	len = sendto( fd, buffer, buflen, 0, &from, sizeof(from) );
//printf("len=%d\n",len);
	close( fd );
	return len;
}

/****************************************************************
 NAME	: vrrp_send_adv				00/02/06 16:31:24
 AIM	:
 REMARK	:
****************************************************************/
static int vrrp_send_adv( vrrp_rt *vsrv, int prio )
{
	int	buflen, ret;
	char *	buffer;
#if 0	/* just for debug */
	struct in_addr in;
	in.s_addr = htonl(vsrv->vif.ipaddr);
	printf("send an advertissement on %s\n",inet_ntoa(in) );
#endif
	/* alloc the memory */
	buflen = vrrp_dlt_len(vsrv) + vrrp_iphdr_len(vsrv) + vrrp_hd_len(vsrv);
	buffer = calloc( buflen, 1 );
	assert( buffer );
	/* build the packet  */
	vrrp_build_pkt( vsrv, prio, buffer, buflen );
	/* send it */
	ret = vrrp_send_pkt( vsrv, buffer, buflen );
	/* build the memory */
	free( buffer );
	return ret;
}


/****************************************************************
 NAME	: usage					00/02/06 08:50:28
 AIM	: display the usage
 REMARK	:
****************************************************************/
static void usage( void )
{
	fprintf( stderr, "vrrpd version %s\n", VRRPD_VERSION );
	fprintf( stderr, "Usage: vrrpd -i ifname -v vrid [-f piddir] [-s] [-a auth] [-p prio] [-nh] ipaddr\n" );
	fprintf( stderr, "  -h       : display this short inlined help\n" );
	fprintf( stderr, "  -n       : Dont handle the virtual mac address\n" );
	fprintf( stderr, "  -i ifname: the interface name to run on\n" );
	fprintf( stderr, "  -v vrid  : the id of the virtual server [1-255]\n" );
	fprintf( stderr, "  -s       : Switch the preemption mode (%s by default)\n"
				, VRRP_PREEMPT_DFL? "Enabled" : "Disabled" );
	fprintf( stderr, "  -a auth  : (not yet implemented) set the authentification type\n" );
	fprintf( stderr, "             auth=(none|pass/hexkey|ah/hexkey) hexkey=0x[0-9a-fA-F]+\n");
	fprintf( stderr, "  -p prio  : Set the priority of this host in the virtual server (dfl: %d)\n"
							, VRRP_PRIO_DFL );
	fprintf( stderr, "  -f piddir: specify the directory where the pid file is stored (dfl: %s)\n"
							, VRRP_PIDDIR_DFL );
	fprintf( stderr, "  -d delay : Set the advertisement interval (in sec) (dfl: %d)\n"
							, VRRP_ADVER_DFL );
	fprintf( stderr, "  ipaddr   : the ip address(es) of the virtual server\n" );
}

/****************************************************************
 NAME	: parse_authopt				00/09/26 22:01:17
 AIM	: 
 REMARK	: parse the authentication option from the user
****************************************************************/
static int parse_authopt(vrrp_rt *vsrv, char *str)
{
 	int	len	= strlen(str);
	vrrp_if *vif	= &vsrv->vif;

	vif->auth_type = VRRP_AUTH_NONE;
	/* epxlicitly no authentication */
	if( !strcmp(str,"none") )	return 0;
	/* below the min len */
 	if( len < 5 )		return -1;
	/* check the type of authentication */
 	if( !strncmp(str, "ah/0x", 5 ) ){
		vif->auth_type = VRRP_AUTH_AH;	
		return -1;	/* WORK: not yet implemented */
	}else if( !strncmp(str, "pw/0x", 5 ) ){
		int	i,j;
		vif->auth_type = VRRP_AUTH_PASS;
		memset( vif->auth_data, 0, sizeof(vif->auth_data) );
		/* parse the key */
		for( i=5,j=0; j < sizeof(vif->auth_data)*2 && i<len ;i++,j++ ){
			/* sanity check */
			if( !isxdigit(str[i]) )	return -1;
			str[i] = toupper(str[i]);
			if( str[i] >= 'A' )	str[i] -= 'A' - 10;
			else			str[i] -= '0';
			vif->auth_data[j/2] |= str[i] << (j&1 ? 0 : 4 );
//WORK:			printf(" j=%d c=%d 0x%x\n",j,str[i],vif->auth_data[j/2]);
		}
		if( i != len )	return -1;
	}else{
		return -1;
	}
	return(0);
}


/****************************************************************
 NAME	: parse_cmdline				00/02/06 09:09:11
 AIM	:
 REMARK	:
****************************************************************/
static int parse_cmdline( vrrp_rt *vsrv, int argc, char *argv[] )
{
	vrrp_if *vif = &vsrv->vif;
	int	c;
	while( 1 ){
		c = getopt( argc, argv, "f:si:v:a:p:d:hn" );
		/* if the parsing is completed, exit */
		if( c == EOF )	break;
		switch( c ){
		case 'n':
			vsrv->no_vmac	= 1;
			break;
		case 's':
			vsrv->preempt	= !vsrv->preempt;
			break;
		case 'f':
			snprintf( PidDir, sizeof(PidDir), "%s", optarg );
			break;
		case 'i':
			vif->ifname	= strdup( optarg );
			/* get the ip address */
			vif->ipaddr	= ifname_to_ip( optarg );
			if( !vif->ipaddr ){
				fprintf( stderr, "no interface found!\n" );
				goto err;
			}
			/* get the hwaddr */
			if( hwaddr_get( vif->ifname, vif->hwaddr
					, sizeof(vif->hwaddr)) ){
				fprintf( stderr, "Can't read the hwaddr on this interface!\n" );
				goto err;
			}
//			printf("ifname=%s 0x%x\n",vsrv->vif.ifname,vsrv->vif.ipaddr);
			break;
		case 'v':
			vsrv->vrid = atoi( optarg );
			if( VRRP_IS_BAD_VID(vsrv->vrid) ){
				fprintf( stderr, "bad vrid!\n" );
				goto err;
			}
			vrrp_hwaddr[0] = 0x00;
			vrrp_hwaddr[1] = 0x00;
			vrrp_hwaddr[2] = 0x5E;
			vrrp_hwaddr[3] = 0x00;
			vrrp_hwaddr[4] = 0x01;
			vrrp_hwaddr[5] = vsrv->vrid;
			break;
		case 'a':
			if( parse_authopt( vsrv, optarg ) ){
				fprintf( stderr, "Invalid authentication key!\n" );
				goto err;
			}
			break;
		case 'p':
			vsrv->priority = atoi( optarg );
			if( VRRP_IS_BAD_PRIORITY(vsrv->priority) ){
				fprintf( stderr, "bad priority!\n" );
				goto err;
			}
			break;
		case 'd':
			vsrv->adver_int = atoi( optarg );
			if( VRRP_IS_BAD_ADVERT_INT(vsrv->adver_int) ){
				fprintf( stderr, "bad advert_int!\n" );
				goto err;
			}
			vsrv->adver_int *= VRRP_TIMER_HZ;
			break;
		case 'h':
			usage();
			exit( 1 );			break;
		case ':':	/* missing parameter */
		case '?':	/* unknown option */
		default:
			goto err;
		}
	}
	return optind;
err:;
	usage();
	return -1;
}

/****************************************************************
 NAME	: cfg_add_ipaddr			00/02/06 09:24:08
 AIM	:
 REMARK	:
****************************************************************/
static void cfg_add_ipaddr( vrrp_rt *vsrv, uint32_t ipaddr )
{
	vsrv->naddr++;
	/* alloc the room */
	if( vsrv->vaddr ){
		vsrv->vaddr = realloc( vsrv->vaddr
					, vsrv->naddr*sizeof(*vsrv->vaddr) );
	} else {
		vsrv->vaddr = malloc( sizeof(*vsrv->vaddr) );
	}
	assert( vsrv->vaddr );
	/* store the data */
	vsrv->vaddr[vsrv->naddr-1].addr		= ipaddr;
	vsrv->vaddr[vsrv->naddr-1].deletable	= 0;
}

/****************************************************************
 NAME	: init_virtual_srv			00/02/06 09:18:02
 AIM	:
 REMARK	:
****************************************************************/
static void init_virtual_srv( vrrp_rt *vsrv )
{
	memset( vsrv, 0, sizeof(*vsrv) );
	vsrv->state	= VRRP_STATE_INIT;
	vsrv->priority	= VRRP_PRIO_DFL;
	vsrv->adver_int	= VRRP_ADVER_DFL*VRRP_TIMER_HZ;
	vsrv->preempt	= VRRP_PREEMPT_DFL;
}

/****************************************************************
 NAME	: chk_min_cfg				00/02/06 17:07:45
 AIM	: TRUE if the minimal configuration isnt done
 REMARK	:
****************************************************************/
static int chk_min_cfg( vrrp_rt *vsrv )
{
	if( vsrv->naddr == 0 ){
		fprintf(stderr, "provide at least one ip for the virtual server\n");
		return -1;
	}
	if( vsrv->vrid == 0 ){
		fprintf(stderr, "the virtual id must be set!\n");
		return -1;
	}
	if( vsrv->vif.ipaddr == 0 ){
		fprintf(stderr, "the interface ipaddr must be set!\n");
		return -1;
	}
	/* make vrrp use the native hwaddr and not the virtual one */
	if( vsrv->no_vmac ){
		memcpy( vrrp_hwaddr, vsrv->vif.hwaddr,sizeof(vsrv->vif.hwaddr));
	}
	return(0);
}

/****************************************************************
 NAME	: vrrp_read				00/02/07 00:04:53
 AIM	:
 REMARK	:
****************************************************************/
static int vrrp_read( vrrp_rt *vsrv, char *buf, int buflen )
{
	fd_set		readfds;
	struct timeval	timeout;
	uint32_t	next	= 0xFFFFFFFF;
	int		len	= 0;
	/* cpu the next timer */
	if( VRRP_TIMER_IS_RUNNING( vsrv->adver_timer ) ){
		int32_t	delta = VRRP_TIMER_DELTA(vsrv->adver_timer);
		if( delta < 0 )	delta = 0;
		next = VRRP_MIN( next, delta );
	}else{	/* here vsrv->ms_down_timer is assumed running */
		int32_t	delta = VRRP_TIMER_DELTA(vsrv->ms_down_timer);
		assert( VRRP_TIMER_IS_RUNNING( vsrv->ms_down_timer ) );
		if( delta < 0 )	delta = 0;
		next = VRRP_MIN( next, delta );
	}
	/* setup the select() */
	FD_ZERO( &readfds );
	FD_SET( vsrv->sockfd, &readfds );
	timeout.tv_sec	= next / VRRP_TIMER_HZ;
	timeout.tv_usec = next % VRRP_TIMER_HZ;
//printf( "val %u,%u %u\n", timeout.tv_sec, timeout.tv_usec, next );
	if( select( vsrv->sockfd + 1, &readfds, NULL, NULL, &timeout ) > 0 ){
		len = read( vsrv->sockfd, buf, buflen );
//		printf("packet received (%d bytes)\n",len);
		if( vrrp_in_chk( vsrv, (struct iphdr *)buf ) ){
			printf("bogus packet!\n");
			len = 0;
		}
	}
	return len;
}

/****************************************************************
 NAME	: send_gratuitous_arp			00/05/11 11:56:30
 AIM	:
 REMARK	: rfc0826
	: WORK: ugly because heavily hardcoded for ethernet
****************************************************************/
static int send_gratuitous_arp( vrrp_rt *vsrv, int addr, int vAddrF )
{
struct m_arphdr
  {
    unsigned short int ar_hrd;          /* Format of hardware address.  */
    unsigned short int ar_pro;          /* Format of protocol address.  */
    unsigned char ar_hln;               /* Length of hardware address.  */
    unsigned char ar_pln;               /* Length of protocol address.  */
    unsigned short int ar_op;           /* ARP opcode (command).  */
    /* Ethernet looks like this : This bit is variable sized however...  */
    unsigned char __ar_sha[ETH_ALEN];   /* Sender hardware address.  */
    unsigned char __ar_sip[4];          /* Sender IP address.  */
    unsigned char __ar_tha[ETH_ALEN];   /* Target hardware address.  */
    unsigned char __ar_tip[4];          /* Target IP address.  */
  };
	char	buf[sizeof(struct m_arphdr)+ETHER_HDR_LEN];
	char	buflen	= sizeof(struct m_arphdr)+ETHER_HDR_LEN;
	struct ether_header 	*eth	= (struct ether_header *)buf;
	struct m_arphdr	*arph = (struct m_arphdr *)(buf+vrrp_dlt_len(vsrv));
	char	*hwaddr	= vAddrF ? vrrp_hwaddr : vsrv->vif.hwaddr;
	int	hwlen	= ETH_ALEN;

	/* hardcoded for ethernet */
	memset( eth->ether_dhost, 0xFF, ETH_ALEN );
	memcpy( eth->ether_shost, hwaddr, hwlen );
	eth->ether_type	= htons(ETHERTYPE_ARP);

	/* build the arp payload */
	memset( arph, 0, sizeof( *arph ) );
	arph->ar_hrd	= htons(ARPHRD_ETHER);
	arph->ar_pro	= htons(ETHERTYPE_IP);
	arph->ar_hln	= 6;
	arph->ar_pln	= 4;
	arph->ar_op	= htons(ARPOP_REQUEST);
	memcpy( arph->__ar_sha, hwaddr, hwlen );
	addr = htonl(addr);
	memcpy( arph->__ar_sip, &addr, sizeof(addr) );
	memcpy( arph->__ar_tip, &addr, sizeof(addr) );
	return vrrp_send_pkt( vsrv, buf, buflen );
}


/****************************************************************
 NAME	: state_gotomaster			00/02/07 00:15:26
 AIM	:
 REMARK	: called when the state is now MASTER
****************************************************************/
static void state_goto_master( vrrp_rt *vsrv )
{
	int	i;
	vrrp_if	*vif = &vsrv->vif;
	/* set the VRRP MAC address -- rfc2338.7.3 */
	if( !vsrv->no_vmac ){
		hwaddr_set( vif->ifname, vrrp_hwaddr, sizeof(vrrp_hwaddr) );
		rcvhwaddr_op( vif->ifname, vif->hwaddr, sizeof(vif->hwaddr), 1);
	}
	/* add the ip addresses */
	ipaddr_ops( vsrv, 1 );

	/* send an advertisement */
	vrrp_send_adv( vsrv, vsrv->priority );
	/* send gratuitous arp for each virtual ip */
	for( i = 0; i < vsrv->naddr; i++ )
		send_gratuitous_arp( vsrv, vsrv->vaddr[i].addr, 1 );
	/* init the struct */
	VRRP_TIMER_SET( vsrv->adver_timer, vsrv->adver_int );
	vsrv->state = VRRP_STATE_MAST;
}

/****************************************************************
 NAME	: state_leavemaster			00/02/07 00:15:26
 AIM	:
 REMARK	: called when the state is no more MASTER
****************************************************************/
static void state_leave_master( vrrp_rt *vsrv, int advF )
{
	uint32_t	addr[1024];
	vrrp_if		*vif = &vsrv->vif;
	/* restore the original MAC addresses */
	if( !vsrv->no_vmac ){
		hwaddr_set( vif->ifname, vif->hwaddr, sizeof(vif->hwaddr) );
		rcvhwaddr_op( vif->ifname, vif->hwaddr, sizeof(vif->hwaddr), 0);
	}
	/* remove the ip addresses */
	ipaddr_ops( vsrv, 0 );

	/* if we stop vrrpd, warn the other routers to speed up the recovery */
	if( advF ){
		vrrp_send_adv( vsrv, VRRP_PRIO_STOP );
	}

	/* send gratuitous ARP for all the non-vrrp ip addresses to update
	** the cache of remote hosts using these addresses */
	if( !vsrv->no_vmac ){
		int		i, naddr;
		naddr = ipaddr_list( ifname_to_idx(vif->ifname), addr
				, sizeof(addr)/sizeof(addr[0]) );
		for( i = 0; i < naddr; i++ )
			send_gratuitous_arp( vsrv, addr[i], 0 );
	}
}

/****************************************************************
 NAME	: state_init				00/02/07 00:15:26
 AIM	:
 REMARK	: rfc2338.6.4.1
****************************************************************/
static void state_init( vrrp_rt *vsrv )
{
	if( vsrv->priority == VRRP_PRIO_OWNER 
			|| vsrv->wantstate == VRRP_STATE_MAST ){
		state_goto_master( vsrv );
	} else {
		int delay = 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);
		VRRP_TIMER_SET( vsrv->ms_down_timer, delay );
		vsrv->state = VRRP_STATE_BACK;
	}
}

/****************************************************************
 NAME	: state_back				00/02/07 00:15:26
 AIM	:
 REMARK	: rfc2338.6.4.2
****************************************************************/
static void state_back( vrrp_rt *vsrv )
{
	char		buf[300];	/* WORK: lame ! */
	int		len	= vrrp_read( vsrv, buf, sizeof(buf) );
	struct iphdr	*iph	= (struct iphdr *)buf;
	vrrp_pkt	*hd	= (vrrp_pkt *)((char *)iph + (iph->ihl<<2));
	if( (!len && VRRP_TIMER_EXPIRED(vsrv->ms_down_timer)) 
			|| vsrv->wantstate == VRRP_STATE_MAST ){
		state_goto_master( vsrv );
		return;
	}
	if( !len )	return;
	if ( hd->priority == 0 ) {
		VRRP_TIMER_SET( vsrv->ms_down_timer, VRRP_TIMER_SKEW(vsrv) );
	} else if( !vsrv->preempt || hd->priority >= vsrv->priority ) {
		int delay = 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);
		VRRP_TIMER_SET( vsrv->ms_down_timer, delay );
	}
}

/****************************************************************
 NAME	: state_mast				00/02/07 00:15:26
 AIM	:
 REMARK	: rfc2338.6.4.3
****************************************************************/
static void state_mast( vrrp_rt *vsrv )
{
	char		buf[300];	/* WORK: lame ! */
	int		len	= vrrp_read( vsrv, buf, sizeof(buf) );
	struct iphdr	*iph	= (struct iphdr *)buf;
	vrrp_pkt	*hd	= (vrrp_pkt *)((char *)iph + (iph->ihl<<2));
	if( vsrv->wantstate == VRRP_STATE_BACK ){
		goto be_backup;
	}
	if( !len && VRRP_TIMER_EXPIRED(vsrv->adver_timer) ){
		vrrp_send_adv( vsrv, vsrv->priority );
		VRRP_TIMER_SET(vsrv->adver_timer,vsrv->adver_int);
		return;
	}
	if( !len )	return;
	if( hd->priority == 0 ){
		vrrp_send_adv( vsrv, vsrv->priority );
		VRRP_TIMER_SET(vsrv->adver_timer,vsrv->adver_int);
	}else if( hd->priority > vsrv->priority ||
			(hd->priority == vsrv->priority &&
			ntohl(iph->saddr) > vsrv->vif.ipaddr) ){
		int delay	= 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);
be_backup:
		VRRP_TIMER_SET( vsrv->ms_down_timer, delay );
		VRRP_TIMER_CLR( vsrv->adver_timer );
		state_leave_master( vsrv, 0 );
		vsrv->state	= VRRP_STATE_BACK;
	}
}

/****************************************************************
 NAME	: open_sock				00/02/07 12:40:00
 AIM	: open the socket and join the multicast group.
 REMARK	:
****************************************************************/
static int open_sock( vrrp_rt *vsrv )
{
	struct	ip_mreq req;
	int	ret;
	/* open the socket */
	vsrv->sockfd = socket( AF_INET, SOCK_RAW, IPPROTO_VRRP );
	if( vsrv->sockfd < 0 ){
		int	err = errno;
		VRRP_LOG(("cant open raw socket. errno=%d. (try to run it as root)\n"
						, err));
		return -1;
	}
	/* join the multicast group */
	memset( &req, 0, sizeof (req));
	req.imr_multiaddr.s_addr = htonl(INADDR_VRRP_GROUP);
	req.imr_interface.s_addr = htonl(vsrv->vif.ipaddr);
	ret = setsockopt (vsrv->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			       (char *) &req, sizeof (struct ip_mreq));
	if( ret < 0 ){
		int	err = errno;
		VRRP_LOG(("cant do IP_ADD_MEMBERSHIP errno=%d\n",err));
		return -1;
	}
	return 0;
}


/****************************************************************
 NAME	: signal_end				00/05/10 23:20:36
 AIM	:
 REMARK	:
****************************************************************/
static void signal_end( int nosig )
{
	vrrp_rt	*vsrv = &glob_vsrv;

	/* remove the pid file */
	pidfile_rm( vsrv );
	/* if the deamon is master, leave this state */
	if( vsrv->state == VRRP_STATE_MAST ){
		state_leave_master( vsrv, 1 );
	}
	exit( 0 );
}

/****************************************************************
 NAME	: signal_user				00/05/10 23:20:36
 AIM	:
 REMARK	:
****************************************************************/
static void signal_user( int nosig )
{
	vrrp_rt	*vsrv = &glob_vsrv;

	if( nosig == SIGUSR1 ){
		vsrv->wantstate = VRRP_STATE_MAST;
	}
	if( nosig == SIGUSR2 ){
		vsrv->wantstate = VRRP_STATE_BACK;
	}
	
	/* rearm the signal */
	signal( nosig, signal_user );
}

/****************************************************************
 NAME	: main					00/02/06 08:48:02
 AIM	:
 REMARK	:
****************************************************************/
int main( int argc, char *argv[] )
{
	vrrp_rt	*vsrv = &glob_vsrv;

#if 1	/* for debug only */
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);
#endif
	
	snprintf( PidDir, sizeof(PidDir), "%s", VRRP_PIDDIR_DFL );

	init_virtual_srv(vsrv);
	/* parse the command line */
	argc = parse_cmdline(vsrv,argc, argv );
	if( argc < 0 ) {
		return -1;
	}
	/* add the virtual server ip */
	for( ; argv[argc]; argc++ ){
		uint32_t ipaddr = inet_addr( argv[argc] );
		cfg_add_ipaddr( vsrv, ntohl(ipaddr) );
	}
	/* check if the minimal configuration has been done */
	if( chk_min_cfg( vsrv ) ){
		fprintf(stderr, "try '%s -h' to read the help\n", argv[0]);
		return -1;
	}
	if( open_sock( vsrv ) ){
		return -1;
	}

	/* the init is completed */
	vsrv->initF = 1;

	/* init signal handler */
	signal( SIGINT, signal_end );
	signal( SIGTERM, signal_end );
	signal( SIGUSR1, signal_user );
	signal( SIGUSR2, signal_user );

	/* try to write a pid file */
	if( pidfile_exist( vsrv ) )	return -1;
	pidfile_write( vsrv );

	/* main loop */
	while( 1 ){
		switch( vsrv->state ){
		case VRRP_STATE_INIT:	state_init( vsrv );	break;
		case VRRP_STATE_BACK:	state_back( vsrv );	break;
		case VRRP_STATE_MAST:	state_mast( vsrv );	break;
		}
	}

	return(0);
}


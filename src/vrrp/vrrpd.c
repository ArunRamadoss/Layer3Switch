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

/* local include */
#include "vrrpd.h"

int ip_id = 0;	/* to have my own ip_id creates collision with kernel ip->id
		** but it should be ok because the packets are unlikely to be
		** fragmented (they are non routable and small) */
		/* WORK: this packet isnt routed, i can check the outgoing MTU
		** to warn the user only if the outoing mtu is too small */
static char vrrp_hwaddr[6];	// WORK: lame hardcoded for ethernet

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
 NAME	: rcvhwaddr_op				00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static int rcvhwaddr_op( char *ifname, char *addr, int addrlen, int addF )
{
}


/****************************************************************
 NAME	: ipaddr_ops				00/02/08 06:51:32
 AIM	:
 REMARK	:
****************************************************************/
static int ipaddr_ops( vrrp_rt *vsrv, int addF )
{
	int	i, err	= 0;
	int	ifidx	= -1;
#if 0
	struct in_addr in;
	int	ifidx	= ifname_to_idx( vsrv->vif.ifname );
#endif

	for( i = 0; i < vsrv->naddr; i++ ){
		vip_addr	*vadd = &vsrv->vaddr[i];
		if( !addF && !vadd->deletable ) 	continue;
#if 0
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
#endif
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
	if( vsrv->auth_type != hd->auth_type ){		
		VRRP_LOG(("receive a %d auth, expecting %d!", vsrv->auth_type
							, hd->auth_type));
		return 1;
	}
	/* check the authentication if it is a passwd */
	if( hd->auth_type != VRRP_AUTH_PASS ){
		char	*pw	= (char *)ip + ntohs(ip->tot_len)
				  -sizeof(vsrv->auth_data);
		if( memcmp( pw, vsrv->auth_data, sizeof(vsrv->auth_data)) ){
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
	ip->saddr	= htonl(vsrv->primary_ip);
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
	vrrp_pkt *hd	= (vrrp_pkt *)buffer;
	uint32_t *iparr	= (uint32_t *)((char *)hd+sizeof(*hd));
	
	hd->vers_type	= (VRRP_VERSION<<4) | VRRP_PKT_ADVERT;
	hd->vrid	= vsrv->vrid;
	hd->priority	= prio;
	hd->naddr	= vsrv->naddr;
	hd->auth_type	= vsrv->auth_type;
	hd->adver_int	= vsrv->adver_int/VRRP_TIMER_HZ;
	/* copy the ip addresses */
	for( i = 0; i < vsrv->naddr; i++ ){
		iparr[i] = htonl( vsrv->vaddr[i].addr );
	}
	hd->chksum	= in_csum( (u_short*)hd, vrrp_hd_len(vsrv), 0);
	/* copy the passwd if the authentication is VRRP_AH_PASS */
	if( vsrv->auth_type == VRRP_AUTH_PASS ){
		char	*pw	= (char *)hd+sizeof(*hd)+vsrv->naddr*4;
		memcpy( pw, vsrv->auth_data, sizeof(vsrv->auth_data));
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
	/* build the packet  */
	vrrp_build_pkt( vsrv, prio, buffer, buflen );
	/* send it */
	ret = vrrp_send_pkt( vsrv, buffer, buflen );
	/* build the memory */
	free( buffer );
	return ret;
}


/****************************************************************
 NAME	: parse_authopt				00/09/26 22:01:17
 AIM	: 
 REMARK	: parse the authentication option from the user
****************************************************************/
static int parse_authopt(vrrp_rt *vsrv, char *str)
{
 	int	len	= strlen(str);

	vsrv->auth_type = VRRP_AUTH_NONE;
	/* epxlicitly no authentication */
	if( !strcmp(str,"none") )	return 0;
	/* below the min len */
 	if( len < 5 )		return -1;
	/* check the type of authentication */
 	if( !strncmp(str, "ah/0x", 5 ) ){
		vsrv->auth_type = VRRP_AUTH_AH;	
		return -1;	/* WORK: not yet implemented */
	}else if( !strncmp(str, "pw/0x", 5 ) ){
		int	i,j;
		vsrv->auth_type = VRRP_AUTH_PASS;
		memset( vsrv->auth_data, 0, sizeof(vsrv->auth_data) );
		/* parse the key */
		for( i=5,j=0; j < sizeof(vsrv->auth_data)*2 && i<len ;i++,j++ ){
			/* sanity check */
			if( !isxdigit(str[i]) )	return -1;
			str[i] = toupper(str[i]);
			if( str[i] >= 'A' )	str[i] -= 'A' - 10;
			else			str[i] -= '0';
			vsrv->auth_data[j/2] |= str[i] << (j&1 ? 0 : 4 );
		}
		if( i != len )	return -1;
	}else{
		return -1;
	}
	return(0);
}


/****************************************************************
 NAME	: cfg_add_ipaddr			00/02/06 09:24:08
 AIM	:
 REMARK	:
****************************************************************/
void cfg_add_ipaddr( vrrp_rt *vsrv, uint32_t ipaddr )
{
	vsrv->naddr++;
	/* alloc the room */
	if( vsrv->vaddr ){
		vsrv->vaddr = realloc( vsrv->vaddr
					, vsrv->naddr*sizeof(*vsrv->vaddr) );
	} else {
		vsrv->vaddr = malloc( sizeof(*vsrv->vaddr) );
	}
	/* store the data */
	vsrv->vaddr[vsrv->naddr-1].addr		= ipaddr;
	vsrv->vaddr[vsrv->naddr-1].deletable	= 0;
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
	if( vsrv->primary_ip == 0 ){
		fprintf(stderr, "the interface ipaddr must be set!\n");
		return -1;
	}
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
		if( delta < 0 )	delta = 0;
		next = VRRP_MIN( next, delta );
	}
		len = read( vsrv, buf, buflen );
//		printf("packet received (%d bytes)\n",len);
		if( vrrp_in_chk( vsrv, (struct iphdr *)buf ) ){
			printf("bogus packet!\n");
			len = 0;
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
#if 0
	char	*hwaddr	= vAddrF ? vrrp_hwaddr : vsrv->hwaddr;
#else

	char	*hwaddr	= NULL;
#endif
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
#if 0
	/* set the VRRP MAC address -- rfc2338.7.3 */
	if( !vsrv->no_vmac ){
		hwaddr_set( vif->ifname, vrrp_hwaddr, sizeof(vrrp_hwaddr) );
		rcvhwaddr_op( vif->ifname, vif->hwaddr, sizeof(vif->hwaddr), 1);
	}
#endif
	/* add the ip addresses */
	ipaddr_ops( vsrv, 1 );

	/* send an advertisement */
	vrrp_send_adv( vsrv, vsrv->priority );
	/* send gratuitous arp for each virtual ip */
	for( i = 0; i < vsrv->naddr; i++ )
		send_gratuitous_arp( vsrv, vsrv->vaddr[i].addr, 1 );
	/* init the struct */
	VRRP_TIMER_SET( vsrv->adver_timer, vsrv->adver_int );
	vsrv->oper_state= VRRP_STATE_MAST;
}

/****************************************************************
 NAME	: state_leavemaster			00/02/07 00:15:26
 AIM	:
 REMARK	: called when the state is no more MASTER
****************************************************************/
static void state_leave_master( vrrp_rt *vsrv, int advF )
{
	uint32_t	addr[1024];
#if 0
	/* restore the original MAC addresses */
	if( !vsrv->no_vmac ){
		hwaddr_set( vif->ifname, vif->hwaddr, sizeof(vif->hwaddr) );
		rcvhwaddr_op( vif->ifname, vif->hwaddr, sizeof(vif->hwaddr), 0);
	}
#endif
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
#if 0
		naddr = ipaddr_list( ifname_to_idx(vif->ifname), addr
				, sizeof(addr)/sizeof(addr[0]) );
#endif
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
		vsrv->oper_state= VRRP_STATE_BACK;
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
			ntohl(iph->saddr) > vsrv->primary_ip) ){
		int delay	= 3*vsrv->adver_int + VRRP_TIMER_SKEW(vsrv);
be_backup:
		VRRP_TIMER_SET( vsrv->ms_down_timer, delay );
		VRRP_TIMER_CLR( vsrv->adver_timer );
		state_leave_master( vsrv, 0 );
		vsrv->oper_state	= VRRP_STATE_BACK;
	}
}

/****************************************************************
 NAME	: main					00/02/06 08:48:02
 AIM	:
 REMARK	:
****************************************************************/
int vrrp_state_machine( vrrp_rt *vsrv )
{
	/* the init is completed */
	vsrv->initF = 1;

	/* main loop */
	while( 1 ){
		switch( vsrv->oper_state){
		case VRRP_STATE_INIT:	state_init( vsrv );	break;
		case VRRP_STATE_BACK:	state_back( vsrv );	break;
		case VRRP_STATE_MAST:	state_mast( vsrv );	break;
		}
	}

	return(0);
}

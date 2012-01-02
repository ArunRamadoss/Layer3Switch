/*
 *	Spanning tree protocol; interface code
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

#include "stp_info.h"
#include "ifmgmt.h"

PORTID stp_make_port_id(uint8_t priority, uint16_t port_no);
struct stp_instance * get_this_bridge_entry (uint16_t vlan_id);
static void stp_init_port(struct stp_port_entry *p);
int stp_is_mac_learning_allowed (int port);

extern struct stp_instance stp_global_instance;
extern struct list_head  stp_instance_head;

struct stp_instance * get_this_bridge_entry (uint16_t vlan_id)
{
	struct stp_instance *p = NULL;

	if (vlan_id == VLAN_INVALID_ID) {
		return &stp_global_instance;
	}

	list_for_each_entry(p, &stp_instance_head, next) {
		if (p->vlan_id == vlan_id)
			return p;
	}
	
	return NULL;
}

PORTID stp_make_port_id(uint8_t priority, uint16_t port_no)
{
	return ((uint16_t)priority << STP_PORT_BITS)
		| (port_no & ((1<<STP_PORT_BITS)-1));
}

static void stp_init_port(struct stp_port_entry *p)
{
	p->port_id = stp_make_port_id(p->priority, p->port_no);
	stp_become_designated_port(p);
	p->state = BLOCKING;
	p->topology_change_ack = 0;
	p->config_pending = 0;
	p->is_own_bpdu = 0;
	p->enabled   = STP_ENABLED;
	IF_STP_INFO(p->port_no) = p;
}

void stp_enable_port(struct stp_port_entry *p)
{
	if (get_port_oper_state (p->port_no) == IF_DOWN) {
		return;
	}
	stp_init_port(p);
	stp_port_state_selection(p->br);
}

void stp_disable_port(struct stp_port_entry *p)
{
	struct stp_instance *br = p->br;
	int wasroot;

	wasroot = stp_is_root_bridge(br);
	stp_become_designated_port(p);
	p->state = DISABLED;
	p->topology_change_ack = 0;
	p->config_pending = 0;
	p->is_own_bpdu = 0;

	del_timer(p->message_age_timer);
	del_timer(p->forward_delay_timer);
	del_timer(p->hold_timer);

	stp_configuration_update(br);

	stp_port_state_selection(br);

	if (stp_is_root_bridge(br) && !wasroot)
		stp_become_root_bridge(br);
}

void stp_enable (struct stp_instance *br)
{
	struct stp_port_entry *p;

	br->stp_enabled = STP_ENABLED;

	mod_timer(br->hello_timer,  br->hello_time);

	stp_config_bpdu_generation(br);

	list_for_each_entry(p, &br->port_list, list) {
		stp_enable_port(p);
	}
}

void stp_disable (struct stp_instance *br)
{
	struct stp_port_entry *p, *n;

	list_for_each_entry_safe(p, n, &br->port_list, list) {
		if (p->state != DISABLED)
			stp_disable_port(p);
		list_del (&p->list);
		tm_free (p, sizeof (*p));
	}

	br->topology_change = 0;
	br->topology_change_detected = 0;

	del_timer(br->hello_timer);
	del_timer(br->topology_change_timer);
	del_timer(br->tcn_timer);
}

void stp_change_bridge_id(struct stp_instance *br, const char *addr)
{
	/* should be aligned on 2 bytes for compare_ether_addr() */
	unsigned short oldaddr_aligned[ETH_ALEN >> 1];
	unsigned char *oldaddr = (unsigned char *)oldaddr_aligned;
	struct stp_port_entry *p;
	int wasroot;

	wasroot = stp_is_root_bridge(br);

	memcpy(oldaddr, br->bridge_id.addr, ETH_ALEN);
	memcpy(br->bridge_id.addr, addr, ETH_ALEN);

	list_for_each_entry(p, &br->port_list, list) {
		if (!compare_ether_addr(p->designated_bridge.addr, oldaddr))
			memcpy(p->designated_bridge.addr, addr, ETH_ALEN);

		if (!compare_ether_addr(p->designated_root.addr, oldaddr))
			memcpy(p->designated_root.addr, addr, ETH_ALEN);

	}

	stp_configuration_update(br);
	stp_port_state_selection(br);
	if (stp_is_root_bridge(br) && !wasroot)
		stp_become_root_bridge(br);
}

int stp_set_bridge_priority (uint16_t newprio, uint16_t vlan_id)
{
        struct stp_port_entry  *p = NULL;
	struct stp_instance *br = get_this_bridge_entry (vlan_id);
        int wasroot;

	if (!br)
		return -1;

        wasroot = stp_is_root_bridge(br);

        list_for_each_entry(p, &br->port_list, list) {
                if (p->state != DISABLED &&
                    stp_is_designated_port(p)) {
                        p->designated_bridge.prio = newprio;
                }

        }

        br->bridge_id.prio = newprio;;
        stp_configuration_update(br);
        stp_port_state_selection(br);
        if (stp_is_root_bridge(br) && !wasroot)
                stp_become_root_bridge(br);

	return 0;
}

int stp_set_bridge_hello_time (int hello , uint16_t vlan_id)
{
	struct stp_instance *br = get_this_bridge_entry (vlan_id);

	if (!br)	
	{
		printf("Spanning tree not enabled\n");
		return -1;
	}

	if (hello < STP_MIN_HELLO_TIME || hello > STP_MAX_HELLO_TIME)
	{
		printf ("Invaild Spanning tree Hello time. Valid range %d-%d\n",
			STP_MIN_HELLO_TIME, STP_MAX_HELLO_TIME);
		return -1;
	}
	if (bridge_timer_relation (br->bridge_forward_delay, br->bridge_max_age, hello))
		return -1;

	br->bridge_hello_time = hello;

	if (stp_is_root_bridge (br)) {
		br->hello_time = br->bridge_hello_time;
	}
	
	return 0;
}

int stp_set_bridge_forward_delay (int fwd_dly , uint16_t vlan_id)
{
	struct stp_instance *br = get_this_bridge_entry (vlan_id);

	if (!br)	
	{
		printf("Spanning tree not enabled\n");
		return -1;
	}

	if (fwd_dly < STP_MIN_FORWARD_DELAY || fwd_dly > STP_MAX_FORWARD_DELAY)
	{
		printf ("Invaild Spanning tree Forward Delay. Valid range %d-%d\n",
			STP_MIN_FORWARD_DELAY, STP_MAX_FORWARD_DELAY);
		return -1;
	}

	if (bridge_timer_relation (fwd_dly, br->bridge_max_age, br->bridge_hello_time))
		return -1;

	br->bridge_forward_delay = fwd_dly;

	if (stp_is_root_bridge(br))
		br->forward_delay = fwd_dly;

	return 0;
}

int stp_set_bridge_max_age (int max_age , uint16_t vlan_id)
{
	struct stp_instance *br = get_this_bridge_entry (vlan_id);

	if (!br)	
	{
		printf("Spanning tree not enabled\n");
		return -1;
	}

	if (max_age < STP_MIN_MAX_AGE || max_age > STP_MAX_MAX_AGE)
	{
		printf ("Invaild Spanning tree max age. Valid range %d-%d\n",
			STP_MIN_MAX_AGE, STP_MAX_MAX_AGE);
		return -1;
	}

	if (bridge_timer_relation (br->bridge_forward_delay, max_age, br->bridge_hello_time))
		return -1;

	br->bridge_max_age = max_age;
	if (stp_is_root_bridge(br))
		br->max_age = max_age;

	return 0;
}

int stp_set_bridge_times (int fdly, int maxage, int htime, uint16_t vlan_id)
{
        struct stp_instance *br = get_this_bridge_entry (vlan_id);

        if (!br)
        {
                printf("Spanning tree not enabled\n");
                return -1;
        }

	if (htime < 0)
		htime = br->bridge_hello_time;
	if (fdly < 0)
		fdly = br->bridge_forward_delay;
	if (maxage < 0)
		maxage = br->bridge_max_age;

	if (bridge_timer_relation (fdly, maxage, htime))
		return -1;

	br->bridge_forward_delay = fdly;
	br->bridge_max_age = maxage;
	br->bridge_hello_time = htime;

	if (stp_is_root_bridge(br)) {
		br->max_age = maxage;
		br->forward_delay = fdly;	
		br->hello_time = htime;
	}

	return 0;
}

void stp_set_port_priority (struct stp_port_entry *p, uint8_t newprio)
{
        uint16_t new_port_id ;

	if (!p)
		return;

        new_port_id = stp_make_port_id(newprio, p->port_no);


        if (stp_is_designated_port(p))
                p->designated_port = new_port_id;

        p->port_id = new_port_id;
        p->priority = newprio;
        if (!memcmp(&p->br->bridge_id, &p->designated_bridge, 8) &&
            p->port_id < p->designated_port) {
                stp_become_designated_port(p);
                stp_port_state_selection(p->br);
        }
}

void stp_set_path_cost(struct stp_port_entry *p, uint32_t path_cost)
{
        p->path_cost = path_cost;
        stp_configuration_update(p->br);
        stp_port_state_selection(p->br);
}

int stp_is_mac_learning_allowed (int port)
{
	if (IF_STP_INFO (port)) {
		if ((IF_STP_STATE(port) != LEARNING) && 
		    (IF_STP_STATE(port) != FORWARDING)) {
			/*Don't Learn the mac address when the 
			  port is not in Learning or FWD state*/
			return 0;
		}	
	}
	return 1;
}

int stp_enable_or_disable_port (int port, int state)
{
	struct stp_port_entry *stp_port = stp_get_port_entry (port);

	if (state == IF_UP) {
		stp_enable_port (stp_port);
	} else if (state == IF_DOWN) {
		stp_disable_port (stp_port);
	}
	return 0;
}


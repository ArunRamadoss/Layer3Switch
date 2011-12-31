#include "stp_info.h"

static void stp_encode_bpdu (STP_BPDU_T *cpdu);
static void stp_decode_bpdu (STP_BPDU_T *cpdu);

struct stp_instance stp_global_instance;
struct list_head  stp_instance_head;

/*****************************************************/
int stp_init (void);
static int stp_init_res (void);
int stp_cli_init_cmd (void);
int stp_create_stp_instance (uint16_t vlan_id, struct stp_instance **p);
int stp_delete_stp_instance (struct stp_instance *p);
int  stp_create_port (struct stp_instance *stp_inst, int port);
void stp_port_timer_init(struct stp_port_entry *p);
int  stp_delete_port (struct stp_instance *stp_inst, uint32_t port);
struct stp_port_entry *  stp_get_port_info (struct stp_instance *stp_inst, uint32_t port);
int validate_TC_bpdu (STP_BPDU_T *bpdu);
int validate_config_bpdu (STP_BPDU_T *bpdu);
void stp_transmit_config(struct stp_port_entry *p);
void stp_transmit_tcn(struct stp_instance *br);
int stp_set_state (struct stp_instance *br, int state);
int is_dest_stp_group_address (MACADDRESS mac);
/*****************************************************/

static int stp_init_res (void)
{
	INIT_LIST_HEAD (&stp_instance_head);

	stp_cli_init_cmd ();

	return 0;
}

int stp_init (void)
{
	if (stp_init_res () < 0) {
		return -1;
	}
	return 0;
}

static int stp_init_stp_instance (struct stp_instance  *new, uint16_t vlan_id)
{

	int j = 0;

	INIT_LIST_HEAD (&new->next);
	INIT_LIST_HEAD (&new->port_list);

	new->vlan_id = vlan_id;
	new->protocol_spec = IEEE8021D;
	new->stp_enabled = FALSE;
	new->priority = STP_DEF_PRIORITY;
	new->timesinceTC = 0;
        new->tolpolgy_changes = 0;
        new->designated_root.prio = STP_DEF_PRIORITY;

	for (j = 0; j < 6; j++) {
	  new->designated_root.addr[j] = this_bridge.addr.addr[j]; 
	  new->bridge_id.addr[j] = this_bridge.addr.addr[j];
	}

        new->bridge_id.prio = STP_DEF_PRIORITY;
        new->root_path_cost = 0;
        new->root_port = 0;
        new->max_age = STP_DEF_MAX_AGE;
        new->hello_time = STP_DEF_HELLO_TIME;
        new->hold_time = STP_DEF_HOLD_COUNT;
        new->forward_delay = STP_DEF_FWD_DELAY;
        new->bridge_max_age= STP_DEF_MAX_AGE;
        new->bridge_hello_time = STP_DEF_HELLO_TIME;
        new->bridge_forward_delay = STP_DEF_FWD_DELAY;
        new->topology_change = FALSE;
        new->topology_change_detected = TRUE;

	stp_timer_init (new);

	return 0;
}

int stp_create_stp_instance (uint16_t vlan_id, struct stp_instance **p)
{
	struct stp_instance  *new = NULL;
	struct stp_instance *pinst = NULL;

	list_for_each_entry(pinst, &stp_instance_head, next) {
		if (pinst->vlan_id == vlan_id) {
			*p = pinst;
			return 0;
		}
	}

	if (vlan_id == VLAN_INVALID_ID) {
		new = &stp_global_instance;
	} else {
		new = tm_malloc (sizeof(struct stp_instance));
		if (!new) {
			debug_stp ("STP Instance Creation failed\n");
			return -1;
		}
        }
	
	if (stp_init_stp_instance (new, vlan_id) < 0) {
		debug_stp ("STP Instance Creation failed ");
		tm_free (new, sizeof(struct stp_instance));
		return -1;
	}

	*p = new;

	list_add_tail (&new->next, &stp_instance_head);
	
	return 0;
}

int stp_delete_stp_instance (struct stp_instance *p)
{
	struct stp_port_entry *port = NULL, *next;

	list_del (&p->next);

	list_for_each_entry_safe(port, next, &p->port_list, list) {
		list_del (&port->list);
		tm_free (port, sizeof(*port));
	}

	tm_free (p, sizeof(*p));
	
	return 0;
}

int  stp_create_port (struct stp_instance *stp_inst, int port)
{
	int rval = -1;
	struct stp_port_entry *p = NULL;
	int j  = 0;

	p = tm_malloc (sizeof (struct stp_port_entry));

	if (p) {
		INIT_LIST_HEAD (&p->list);
		p->br = stp_inst;
		p->port_no = port;
		p->priority = STP_DEF_PORT_PRIO;
		p->enabled = FALSE;
		for (j = 0; j < 6; j++) {
			p->designated_root.addr[j] = 
				this_bridge.addr.addr[j];
			p->designated_bridge.addr[j] = 
				this_bridge.addr.addr[j];
		}
		p->designated_cost = STP_DEF_DESG_COST;
		p->designated_port = 0;
		p->fwdtransitions = 0;
		p->path_cost = STP_DEF_PATH_COST;
		stp_port_timer_init (p);
		list_add_tail (&p->list, &p->br->port_list);
		rval = 0;
	}
	return rval;
}	

int  stp_delete_port (struct stp_instance *stp_inst, uint32_t port)
{
	struct stp_port_entry *p = NULL;

	list_for_each_entry(p, &stp_inst->port_list, list) {
		if (p->port_no == port) {
			list_del (&p->list);
			break;
		}
	}
	return 0;
}

struct stp_port_entry *  stp_get_port_info (struct stp_instance *stp_inst, uint32_t port)
{
	struct stp_port_entry *p = NULL;

	list_for_each_entry(p, &stp_inst->port_list, list) {
		if (p->port_no == port) {
			return p;
		}
	}
	return NULL;
}


struct stp_port_entry * stp_get_port_entry (uint16_t port)
{
	struct stp_port_entry *p = NULL;
	struct stp_instance *pinst = NULL;

	list_for_each_entry(pinst, &stp_instance_head, next) {
		list_for_each_entry(p, &pinst->port_list, list) {
			if (p->port_no == port)
				return p;
		}
	}
	return NULL;

}

int validate_TC_bpdu (STP_BPDU_T *bpdu)
{
	return 1;
}

int validate_config_bpdu (STP_BPDU_T *bpdu)
{
	return 1;
}

int stp_process_bpdu (STP_BPDU_T *bpdu, uint16_t port)
{
	struct stp_port_entry *stp_port = stp_get_port_entry (port);

	if (!stp_port) {
		return -1;
	}
	
	if (!stp_port->enabled) {
		return -1;
	}
	
	if (stp_port->state == STP_DISABLED) {
		return -1;
	}

	if (bpdu->type == BPDU_CONFIG_TYPE) {

		if (!validate_config_bpdu (bpdu)) {
			debug_stp ("BPDU is malformed\n");
			return -1;
		}
		stp_decode_bpdu (bpdu);
		stp_received_config_bpdu (stp_port, bpdu);		

	} else if (bpdu->type == BPDU_TC_TYPE) {
		if (!validate_TC_bpdu (bpdu)) {
			debug_stp ("BPDU is malformed\n");
			return -1;
		}
		stp_received_tcn_bpdu (stp_port);		
	}

	return 0;
}

struct stp_port_entry *stp_get_port(struct stp_instance *br, uint16_t port_no)
{
	struct stp_port_entry *p;

	list_for_each_entry (p, &br->port_list, list) {
		if (p->port_no == port_no)
			return p;
	}

	return NULL;
}

static int stp_should_become_root_port(const struct stp_port_entry *p,
		uint16_t root_port)
{
	struct stp_instance *br;
	struct stp_port_entry *rp;
	int t;

	br = p->br;
	if (p->state == DISABLED ||
            stp_is_designated_port(p))
		return 0;

	if (memcmp(&br->bridge_id, &p->designated_root, 8) <= 0)
		return 0;

	if (!root_port)
		return 1;

	rp = stp_get_port(br, root_port);

	t = memcmp(&p->designated_root, &rp->designated_root, 8);
	if (t < 0)
		return 1;
	else if (t > 0)
		return 0;

	if (p->designated_cost + p->path_cost <
			rp->designated_cost + rp->path_cost)
		return 1;
	else if (p->designated_cost + p->path_cost >
			rp->designated_cost + rp->path_cost)
		return 0;

	t = memcmp(&p->designated_bridge, &rp->designated_bridge, 8);
	if (t < 0)
		return 1;
	else if (t > 0)
		return 0;

	if (p->designated_port < rp->designated_port)
		return 1;
	else if (p->designated_port > rp->designated_port)
		return 0;

	if (p->port_id < rp->port_id)
		return 1;

	return 0;
}

static void stp_root_selection(struct stp_instance *br)
{
	struct stp_port_entry *p;
	uint16_t root_port = 0;

	list_for_each_entry(p, &br->port_list, list) {
		/*Don't consider the Port for root selection
  		  which rx'd its own bpdu because of loopback
		  in the bridge
  		 */
		if (p->is_own_bpdu)
			continue;

		if (stp_should_become_root_port(p, root_port))
			root_port = p->port_no;

	}

	br->root_port = root_port;

	if (!root_port) {
		br->designated_root = br->bridge_id;
		br->root_path_cost = 0;
	} else {
		p = stp_get_port(br, root_port);
		br->designated_root = p->designated_root;
		br->root_path_cost = p->designated_cost + p->path_cost;
	}
}

void stp_become_root_bridge(struct stp_instance *br)
{
	br->max_age = br->bridge_max_age;
	br->hello_time = br->bridge_hello_time;
	br->forward_delay = br->bridge_forward_delay;
	stp_topology_change_detection(br);
	del_timer(br->tcn_timer);
	stp_config_bpdu_generation(br);
	mod_timer(br->hello_timer, br->hello_time);
}

void stp_transmit_config(struct stp_port_entry *p)
{
	STP_BPDU_T bpdu;
	struct stp_instance *br;

	if (timer_pending(p->hold_timer)) {
		p->config_pending = 1;
		return;
	}

	br = p->br;

	bpdu.flags |= br->topology_change;
	bpdu.flags |= p->topology_change_ack;
	bpdu.root_id = br->designated_root;
	bpdu.root_path_cost = br->root_path_cost;
	bpdu.bridge_id = br->bridge_id;
	bpdu.port_id = p->port_id;
	if (stp_is_root_bridge(br))
		bpdu.message_age = 0;
	else {
		struct stp_port_entry *root = stp_get_port(br, br->root_port);
		int rmt = (timer_get_remaining_time (root->message_age_timer)) / tm_get_ticks_per_second ();
		bpdu.message_age = br->max_age - rmt + MESSAGE_AGE_INCR;
	}
	bpdu.max_age = br->max_age;
	bpdu.hello_time = br->hello_time;
	bpdu.forward_delay = br->forward_delay;

	if (bpdu.message_age < br->max_age) {
		stp_encode_bpdu (&bpdu);
		stp_send_config_bpdu(p, &bpdu);
		p->topology_change_ack = 0;
		p->config_pending = 0;
/*FIXME*/
#define BR_HOLD_TIME 1
		mod_timer(p->hold_timer, BR_HOLD_TIME);
	}
}

static inline void stp_record_config_information(struct stp_port_entry *p,
						 STP_BPDU_T *bpdu)
{
	p->designated_root = bpdu->root_id;
	p->designated_cost = bpdu->root_path_cost;
	p->designated_bridge = bpdu->bridge_id;
	p->designated_port = bpdu->port_id;
	mod_timer(p->message_age_timer, (p->br->max_age - bpdu->message_age));
}

static inline void stp_record_config_timeout_values(struct stp_instance *br,
						    STP_BPDU_T *bpdu)
{
	br->max_age = bpdu->max_age;
	br->hello_time = bpdu->hello_time;
	br->forward_delay = bpdu->forward_delay;
	br->topology_change = ((bpdu->flags & TC_BIT) == TC_BIT) ;
}

void stp_transmit_tcn(struct stp_instance *br)
{
	stp_send_tcn_bpdu(stp_get_port(br, br->root_port));
}

static int stp_should_become_designated_port(const struct stp_port_entry*p)
{
	struct stp_instance *br;
	int t;

	br = p->br;
	if (stp_is_designated_port(p))
		return 1;

	if (memcmp(&p->designated_root, &br->designated_root, 8))
		return 1;

	if (br->root_path_cost < p->designated_cost)
		return 1;
	else if (br->root_path_cost > p->designated_cost)
		return 0;

	t = memcmp(&br->bridge_id, &p->designated_bridge, 8);
	if (t < 0)
		return 1;
	else if (t > 0)
		return 0;

	if (p->port_id < p->designated_port)
		return 1;

	return 0;
}

static void stp_designated_port_selection(struct stp_instance *br)
{
	struct stp_port_entry *p;

	list_for_each_entry(p, &br->port_list, list) {
		if (p->state != DISABLED &&
	 	    stp_should_become_designated_port(p))
			stp_become_designated_port(p);

	}
}

static int stp_supersedes_port_info(struct stp_port_entry *p, STP_BPDU_T *bpdu)
{
	int t;

	t = memcmp(&bpdu->root_id, &p->designated_root, 8);
	if (t < 0)
		return 1;
	else if (t > 0)
		return 0;

	if (bpdu->root_path_cost < p->designated_cost)
		return 1;
	else if (bpdu->root_path_cost > p->designated_cost)
		return 0;

	t = memcmp(&bpdu->bridge_id, &p->designated_bridge, 8);
	if (t < 0)
		return 1;
	else if (t > 0)
		return 0;

	if (memcmp(&bpdu->bridge_id, &p->br->bridge_id, 8))
		return 1;

	if (bpdu->port_id <= p->designated_port) {
		/*802.1D 9.3.4 Validation of received BPDUs
 		 NOTE 1—If the Bridge Identifier and Port Identifier both 
 		 match the values that would be transmitted in a Configuration
		 BPDU, the BPDU is discarded to prevent processing of the Port’s 
		 own BPDUs;*/
		if (bpdu->port_id == p->designated_port)
			p->is_own_bpdu = 0;
		return 1;
	}


	return 0;
}

static inline void stp_topology_change_acknowledged(struct stp_instance *br)
{
	br->topology_change_detected = 0;
	del_timer(br->tcn_timer);
}

void stp_topology_change_detection(struct stp_instance *br)
{
	int isroot = stp_is_root_bridge(br);

	if (br->stp_enabled != STP_ENABLED)
		return;

	if (isroot) {
		br->topology_change = 1;
		mod_timer(br->topology_change_timer, br->bridge_forward_delay  
						      + br->bridge_max_age);
	} else if (!br->topology_change_detected) {
		stp_transmit_tcn(br);
		mod_timer(br->tcn_timer, br->bridge_hello_time);
	}

	br->topology_change_detected = 1;
}

void stp_config_bpdu_generation(struct stp_instance *br)
{
	struct stp_port_entry *p = NULL;

	list_for_each_entry(p, &br->port_list, list) {
		if ((p->state != DISABLED) &&
		    stp_is_designated_port(p)) {
			stp_transmit_config(p);
		}
	}
}

static inline void stp_reply(struct stp_port_entry *p)
{
	stp_transmit_config(p);
}

void stp_configuration_update(struct stp_instance *br)
{
	stp_root_selection(br);
	stp_designated_port_selection(br);
}

void stp_become_designated_port(struct stp_port_entry *p)
{
	struct stp_instance *br;

	br = p->br;
	p->designated_root = br->designated_root;
	p->designated_cost = br->root_path_cost;
	p->designated_bridge = br->bridge_id;
	p->designated_port = p->port_id;
}


static void stp_make_blocking(struct stp_port_entry *p)
{
	if (p->state != DISABLED &&
			p->state != BLOCKING) {
		if (p->state == FORWARDING ||
				p->state == LEARNING)
			stp_topology_change_detection(p->br);

		p->state = BLOCKING;
		del_timer(p->forward_delay_timer);
	}
}

static void stp_make_forwarding(struct stp_port_entry *p)
{
	struct stp_instance *br = p->br;

	if (p->state != BLOCKING)
		return;

	if (br->forward_delay == 0) {
		p->state = FORWARDING;
		stp_topology_change_detection(br);
		del_timer(p->forward_delay_timer);
	}
	else if (p->br->stp_enabled == STP_ENABLED)
		p->state = LISTENING;
	else
		p->state = LEARNING;

	if (br->forward_delay != 0)
		mod_timer(p->forward_delay_timer,  br->forward_delay);
}

void stp_port_state_selection(struct stp_instance *br)
{
	struct stp_port_entry *p;

	list_for_each_entry(p, &br->port_list, list) {
		if (p->state != DISABLED) {
			if (p->port_no == br->root_port) {
				p->config_pending = 0;
				p->topology_change_ack = 0;
				stp_make_forwarding(p);
			} else if (!p->is_own_bpdu && stp_is_designated_port(p)) {
				del_timer(p->message_age_timer);
				stp_make_forwarding(p);
			} else {
				p->config_pending = 0;
				p->topology_change_ack = 0;
				stp_make_blocking(p);
			}
		}

	}
}

static inline void stp_topology_change_acknowledge(struct stp_port_entry *p)
{
	p->topology_change_ack = 1;
	stp_transmit_config(p);
}

int stp_is_root_bridge(const struct stp_instance *br)
{
        return !memcmp(&br->bridge_id, &br->designated_root, 8);
}

int stp_set_state (struct stp_instance *br, int state)
{
	if (state)
		stp_enable (br);
	else
		stp_disable (br);

      	br->stp_enabled = state;
	return 0;
}


void stp_received_config_bpdu(struct stp_port_entry *p, STP_BPDU_T *bpdu)
{
	struct stp_instance *br = NULL;
	int was_root = 0;

	br = p->br;

	was_root = stp_is_root_bridge(br);

	p->is_own_bpdu = 0;

	if (stp_supersedes_port_info(p, bpdu)) {
		stp_record_config_information(p, bpdu);
		stp_configuration_update(br);
		stp_port_state_selection(br);

		if (!stp_is_root_bridge(br) && was_root) {
			del_timer(br->hello_timer);
			if (br->topology_change_detected) {
				del_timer(br->topology_change_timer);
				stp_transmit_tcn(br);
				mod_timer(br->tcn_timer, br->bridge_hello_time);
			}
		}

		if (p->port_no == br->root_port) {
			stp_record_config_timeout_values(br, bpdu);
			stp_config_bpdu_generation(br);
			if (bpdu->flags & TC_ACK_BIT)
				stp_topology_change_acknowledged(br);
		}
	} else if (stp_is_designated_port(p)) {
		stp_reply(p);
	}
}

void stp_received_tcn_bpdu(struct stp_port_entry *p)
{
	if (stp_is_designated_port(p)) {
		stp_topology_change_detection(p->br);
		stp_topology_change_acknowledge(p);
	}
}

int stp_is_designated_port(const struct stp_port_entry *p)
{
        return !memcmp(&p->designated_bridge, &p->br->bridge_id, 8) &&
                (p->designated_port == p->port_id);
}


static void stp_send_bpdu(struct stp_port_entry *p,  const unsigned char *data,
			 int length)
{
	uint8_t * pkt = NULL;
	uint8_t  smac[6];

	int size = sizeof (MACHDR) + sizeof(ETHHDR);

	pkt = tm_malloc (size + length); 

	if (!pkt)
		return;

	llc_pdu_header_init (pkt , LLC_PDU_TYPE_U, LLC_SAP_BSPAN, LLC_SAP_BSPAN, 
			     LLC_1_PDU_CMD_UI);

	llc_pdu_init_as_ui_cmd(pkt);

	get_port_mac_address (p->port_no, smac);

	llc_mac_hdr_init (pkt, br_group_address, smac, 0x4, length + sizeof(ETHHDR));

	memcpy(pkt + size, data, length);

	send_packet (pkt, p->port_no, length + size);

	tm_free (pkt, size + length);
}

void stp_send_config_bpdu(struct stp_port_entry *p, STP_BPDU_T *bpdu)
{
	unsigned char buf[35];

	if (p->br->stp_enabled != STP_ENABLED)
		return;
	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = BPDU_CONFIG_TYPE;
	buf[4] = bpdu->flags;
	memcpy (&buf[5], &bpdu->root_id.prio, sizeof(uint16_t));
	buf[7] = bpdu->root_id.addr[0];
	buf[8] = bpdu->root_id.addr[1];
	buf[9] = bpdu->root_id.addr[2];
	buf[10] = bpdu->root_id.addr[3];
	buf[11] = bpdu->root_id.addr[4];
	buf[12] = bpdu->root_id.addr[5];
	buf[13] = (bpdu->root_path_cost >> 24) & 0xFF;
	buf[14] = (bpdu->root_path_cost >> 16) & 0xFF;
	buf[15] = (bpdu->root_path_cost >> 8) & 0xFF;
	buf[16] = bpdu->root_path_cost & 0xFF;
	memcpy (&buf[17], &bpdu->bridge_id.prio, sizeof(uint16_t));
	buf[19] = bpdu->bridge_id.addr[0];
	buf[20] = bpdu->bridge_id.addr[1];
	buf[21] = bpdu->bridge_id.addr[2];
	buf[22] = bpdu->bridge_id.addr[3];
	buf[23] = bpdu->bridge_id.addr[4];
	buf[24] = bpdu->bridge_id.addr[5];
	buf[25] = (bpdu->port_id >> 8) & 0xFF;
	buf[26] = bpdu->port_id & 0xFF;

	memcpy (&buf[27], &bpdu->message_age, sizeof(uint16_t));
	memcpy (&buf[29], &bpdu->max_age, sizeof(uint16_t));
	memcpy (&buf[31], &bpdu->hello_time, sizeof(uint16_t));
	memcpy (&buf[33], &bpdu->forward_delay, sizeof(uint16_t));

	stp_send_bpdu(p, buf, 35);
}

void stp_send_tcn_bpdu(struct stp_port_entry *p)
{
	unsigned char buf[4];

	if (!p)
		return;

	if (p->br->stp_enabled != STP_ENABLED)
		return;

	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = BPDU_TC_TYPE;

	stp_send_bpdu(p, buf, 4);
}

unsigned compare_ether_addr(const uint8_t *addr1, const uint8_t *addr2)
{
	const uint16_t *a = (const uint16_t *) addr1;
	const uint16_t *b = (const uint16_t *) addr2;

	return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])) != 0;
}

int is_dest_stp_group_address (MACADDRESS mac)
{
	if (!compare_ether_addr (mac.addr, br_group_address)) {
		return 1;
	}
	return 0;
}

static void stp_encode_bpdu (STP_BPDU_T *cpdu)
{
	cpdu->root_id.prio   = htons(cpdu->root_id.prio);
	cpdu->bridge_id.prio = htons(cpdu->bridge_id.prio);
	cpdu->root_path_cost = htonl(cpdu->root_path_cost);
	cpdu->message_age    = htons(cpdu->message_age);
	cpdu->max_age        = htons(cpdu->max_age);
	cpdu->hello_time     = htons(cpdu->hello_time);
	cpdu->forward_delay  = htons(cpdu->forward_delay);
}

static void stp_decode_bpdu (STP_BPDU_T *cpdu)
{
	cpdu->root_id.prio   = ntohs(cpdu->root_id.prio);
	cpdu->bridge_id.prio = ntohs(cpdu->bridge_id.prio);
	cpdu->root_path_cost = ntohl(cpdu->root_path_cost);
	cpdu->message_age    = ntohs(cpdu->message_age);
	cpdu->max_age        = ntohs(cpdu->max_age);
	cpdu->hello_time     = ntohs(cpdu->hello_time);
	cpdu->forward_delay  = ntohs(cpdu->forward_delay);
	cpdu->port_id        = ntohs(cpdu->port_id);
}

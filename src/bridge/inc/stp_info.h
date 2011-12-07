#include "common_types.h"
#include "list.h"
#include "bridge.h"

#define BPDU_PROTOCOL_ID        0x00
#define BPDU_VERSION_ID         0x00
#define BPDU_TC_TYPE            0x80
#define BPDU_CONFIG_TYPE        0x00
#define TC_BIT     0x01
#define TC_ACK_BIT 0x80

#define DISABLED 0
#define LISTENING 1
#define LEARNING 2
#define FORWARDING 3
#define BLOCKING 4

#define STP_ENABLED 1
#define STP_DISABLED 0

#define MESSAGE_AGE_INCR 1

enum STP_PROTO_SPEC {
	UNKNOWN = 1,
	DEClB100 = 2,
	IEEE8021D = 3
};

#define STP_MAX_MSG    100

/*modify the following macros*/
#define STP_PORT_BITS	10
#define STP_MAX_PORTS	(1 << STP_PORT_BITS)
#define debug_stp(fmt)   if (1) printf("STP: %s", fmt);

struct stp_instance {
	struct list_head next;
	struct list_head port_list;
	uint16_t   vlan_id;
	int32_t     protocol_spec;
	int32_t     stp_enabled;
	int32_t     priority;
	TIMESTMP    timesinceTC;
	int64_t     tolpolgy_changes;
	BRIDGEID    designated_root;
	BRIDGEID    bridge_id;
	int32_t     root_path_cost;
	PORT        root_port;
	TIMEOUT     max_age;
	TIMEOUT     hello_time;
	int32_t     hold_time;
	TIMEOUT     forward_delay;
	TIMEOUT     bridge_max_age;
	TIMEOUT     bridge_hello_time;
	TIMEOUT     bridge_forward_delay;
        TIMER_ID    hello_timer;
        TIMER_ID    tcn_timer;
        TIMER_ID    topology_change_timer;
        uint8_t     topology_change;
        uint8_t     topology_change_detected;
};

struct stp_port_entry {
	struct list_head list;
	struct stp_instance *br;
        PORT        port_no;
        int32_t     priority;
        int32_t     state;
        TRUTH       enabled;
        BRIDGEID    designated_root;
        int32_t     designated_cost;
        BRIDGEID    designated_bridge;
        uint16_t    designated_port;
	uint16_t    port_id;
        uint64_t    fwdtransitions;
        int32_t     path_cost;
        TIMER_ID    forward_delay_timer;
        TIMER_ID    hold_timer;
        TIMER_ID    message_age_timer;
	uint8_t     topology_change_ack;
        uint8_t	    config_pending;
};

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct stp_hdr {
  uint16_t protocol;
  uint8_t  version;
  uint8_t  type;
/*****************************************/
  uint8_t  flags;
  BRIDGEID root_id;
  int32_t  root_path_cost;
  BRIDGEID bridge_id;
  uint16_t port_id;
  uint16_t message_age;
  uint16_t max_age;
  uint16_t hello_time;
  uint16_t  forward_delay;
};



typedef struct stp_bpdu {
/*****************************************
 * Params common for the CONF and TC BPDU*
 * ***************************************/
  MACHDR   mac_hdr;
  ETHHDR   eth_hdr;
  uint16_t protocol;
  uint8_t  version;
  uint8_t  type;
/*****************************************/
  uint8_t  flags;
  BRIDGEID root_id;
  int32_t  root_path_cost;
  BRIDGEID bridge_id;
  uint16_t port_id;
  uint16_t message_age;
  uint16_t max_age;
  uint16_t hello_time;
  uint16_t  forward_delay;
} STP_BPDU_T;

#pragma pack(pop)   /* restore original alignment from stack */
extern bridge_group_t   this_bridge;
extern port_entry_t     this_bridge_ports[];
extern char switch_mac[];
extern struct stp_instance stp_global_instance;


int stp_process_bpdu (STP_BPDU_T *bpdu, uint16_t port);
struct stp_port_entry *stp_get_port(struct stp_instance *br, uint16_t port_no);
int stp_is_root_bridge(const struct stp_instance *br);
void stp_topology_change_detection(struct stp_instance *br);
void stp_config_bpdu_generation(struct stp_instance *br);
void stp_become_designated_port(struct stp_port_entry *p);
void stp_received_config_bpdu(struct stp_port_entry *p, STP_BPDU_T *bpdu);
void stp_received_tcn_bpdu(struct stp_port_entry *p);
void stp_send_tcn_bpdu(struct stp_port_entry *p);
int stp_task (void *arg);
int stp_process_events (int port, uint8_t event, int);

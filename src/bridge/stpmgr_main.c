#include "stp_info.h"


void* stpmgr_task (void *arg);
int process_bpdu (void *bpdu, uint16_t port, int, int);
int stpmgr_init (void);
int stp_process_events (int port, uint8_t event, int vlanid);
int stp_send_event (int event, int port, int vlanid);
int stp_rcv_bpdu (void *pkt, int port, int vlanid, int len);
int stp_enable_or_disable_port (int port, int state);
int rstp_enable_or_disable_port (int port, int state);
int rstp_process_bpdu (void *tpdu, uint16_t ifp);

static int stp_Q_pool_id = 0;
static int stp_Q_id = 0;

struct stp_msg {
	int  type;
	int  port;
	int  vlanid;
	int len;
	void *msg;
};

enum stp_msg_types {
	STP_MSG_TYPE_BPDU = 1, 
	STP_MSG_TYPE_EVENTS
};


int stpmgr_init (void)
{
	tmtaskid_t tid = 0;
	int len = sizeof (struct stp_msg);

	if ((stp_Q_id = msg_create_Q ("STPQ", STP_MAX_MSG, len)) < 0) {
		debug_stp ("Message Q Creation failed !\n");
		return -1;
	}

	stp_Q_pool_id = mem_pool_create ("STPQ", STP_MAX_MSG * len, 
                                         STP_MAX_MSG, 0);

	if (stp_Q_pool_id < 0) {
		debug_stp ("Mem pool creation failed !\n");
		return -1;
	}

	task_create ("STPMGR", 3, 3, 32000, stpmgr_task, NULL, NULL, &tid);

	return 0;
}

void *stpmgr_task (void *arg)
{
	struct stp_msg *msg = NULL;

	while (1) {
		if (msg_rcv (stp_Q_id, (char **)&msg, sizeof(struct stp_msg)) < 0) {
			continue;
		}
		switch (msg->type) {
			case STP_MSG_TYPE_BPDU:
				process_bpdu (msg->msg, msg->port, msg->vlanid, msg->len);
				free (msg->msg);
				break;
			case STP_MSG_TYPE_EVENTS:
				if (stp_process_events (msg->port, (uint8_t)
							((unsigned long)msg->msg), 
							msg->vlanid) < 0) {
				}
				break;
			default:
				debug_stp ("Invalid Message type\n");
				break;
		}
		free_blk (stp_Q_pool_id, msg);
	}
	return NULL;
}

int stp_rcv_bpdu (void *pkt, int port, int vlanid, int len)
{
	struct stp_msg	*msg = NULL;

	msg = alloc_block (stp_Q_pool_id);

	if (msg) {
		msg->type = STP_MSG_TYPE_BPDU;	
		msg->port = port;
		msg->vlanid = vlanid;
		msg->len = len;
		msg->msg = pkt;
		msg_send (stp_Q_id, msg, sizeof(struct stp_msg));
		return 0;
	}
	return -1;	
}

int stp_send_event (int event, int port, int vlanid)
{
	struct stp_msg	*msg = NULL;

	msg = alloc_block (stp_Q_pool_id);

	if (msg) {
		msg->type = STP_MSG_TYPE_EVENTS;	
		msg->port = port;
		msg->vlanid = vlanid;
		msg->msg = (char *)event;
		msg_send (stp_Q_id, msg, sizeof(struct stp_msg));
		return 0;
	}
	return -1;	
}

int stp_process_events (int port, uint8_t event, int vlanid)
{
	int stp_mode = vlan_get_this_bridge_stp_mode (vlanid);

	if (stp_mode == MODE_STP) {
		stp_enable_or_disable_port (port, event);

	} else if (stp_mode == MODE_RSTP) {
		rstp_enable_or_disable_port (port, event);
	}

	return 0;
}


int process_bpdu (void *bpdu, uint16_t port, int vlanid, int len)
{
	int stp_mode = vlan_get_this_bridge_stp_mode (vlanid);

	if (stp_mode == MODE_STP) {
	 	stp_process_bpdu (bpdu, port);

	} else if (stp_mode == MODE_RSTP) {
		rstp_process_bpdu (bpdu, port);
	}

	return 0;
}

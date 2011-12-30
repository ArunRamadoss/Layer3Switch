enum ACCEPTABLE_FRAME_TYPES {
	ADMITALL = 1,
	ADMITONLYVLANTAGGED
};

struct dot1q_port_vlan_stats {
	uint32_t   port;
	uint32_t   in_frames;
	uint32_t   out_frames;
	uint32_t   in_discards;
	uint32_t   in_overflow_frames;
	uint32_t   out_overflow_frames;
	uint32_t   in_overflow_discards;
	struct     list_head next;
};

struct vlan_static_entry {
	uint16_t vlan_id;
	char	 Name[MAX_VLAN_NAME];
	PORTLIST egress_ports;
	PORTLIST forbidden_egress_ports;
	PORTLIST untagged_ports;
	uint8_t	 row_status;
	uint8_t  stp_enabled;
	uint8_t  stp_mode;
	void     *stp_instance;
	void     *rstp_instance;
	struct list_head port_vlan_stats;
	struct list_head next;
};

struct dot1q_port_vlan_entry {
	uint32_t   gvrp_failed_registrations;
	int32_t    port;
	MACADDRESS gvrp_last_pdu_origin;
	uint16_t   pvid;
	uint8_t    acceptable_frametype;
	uint8_t    ingress_filtering;
	uint8_t    gvrp_status;
	uint8_t    restricted_vlan_registration;
};

#define SET_PORT_LIST(plist, portno) { \
		int byte = (portno - 1) / MAX_BITS_PER_BYTE; \
		int bit  = (portno - 1) % MAX_BITS_PER_BYTE; \
		plist[byte] |= (0x1  << bit); \
	}

#define IS_PORT_SET_PORT_LIST(plist, portno, res) { \
		int byte = (portno - 1) / MAX_BITS_PER_BYTE; \
		int bit  = (portno - 1) % MAX_BITS_PER_BYTE; \
		res = (plist[byte] & (0x1  << bit)); \
	}


#define RESET_PORT_LIST(plist, portno) { \
		int byte = (portno - 1) / MAX_BITS_PER_BYTE; \
		int bit  = (portno - 1) % MAX_BITS_PER_BYTE; \
		plist[byte] &= ~(0x1  << bit); \
	}



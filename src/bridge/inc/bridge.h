typedef struct dot1dBaseBridgeGroup {
	uint32_t   dot1q_max_supported_vlans;
	uint32_t   dot1q_num_vlans;
        int32_t    nports;
        int32_t    bridge_type;
        MACADDRESS addr;
	uint16_t   dot1q_max_vlan_id;
	uint8_t    dot1q_gvrp_status;
	uint8_t    dot1q_vlan_version;
	uint8_t    pad[2];
}bridge_group_t;

typedef struct dot1dBasePortEntry {
        int32_t  Port;
        int32_t  PortIfIndex;
        int32_t  PortCircuit;
        uint32_t PortDelayExceededDiscards;
        uint32_t PortMtuExceededDiscards;
}port_entry_t;

typedef struct dot1dTpBridgeGroup {
        uint32_t dot1dTpLearnedEntryDiscards;
        int32_t  dot1dTpAgingTime;
}tbridge_group_t;

#ifdef STATIC_ENTRY_SUPPORTED
typdef struct dot1dstaticentry  {
        MACADDRESS   mac_addr;
        int32_t      rxdport;
	OCTETSTRING  allowed2go;
        uint32_t     status;
};
#endif



typedef struct ifentry {
        int32_t   ifIndex;
        uint8_t   ifDescr[MAX_PORT_NAME];          
        int32_t   ifType;
        int32_t   ifMtu;   
        int32_t   ifSpeed;
        int32_t   ifAdminStatus;
        int32_t   ifOperStatus;
        uint32_t  ifLastChange; 
        uint32_t  ifInOctets; 
        uint32_t  ifInUcastPkts;
        uint32_t  ifInDiscards;    
        uint32_t  ifInErrors;     
        uint32_t  ifInUnknownProtos;
        uint32_t  ifOutOctets;
        uint32_t  ifOutUcastPkts;
        uint32_t  ifOutDiscards;   
        uint32_t  ifOutErrors;    
	struct stp_port_entry *pstp_info;
        MACADDRESS ifPhysAddress;  
}__attribute__ ((__packed__))port_t;

extern port_t port_cdb[];

#define   IF_INDEX(port)            port_cdb[port - 1].ifIndex
#define   IF_DESCR(port)            port_cdb[port - 1].ifDescr
#define   IF_TYPE(port)             port_cdb[port - 1].ifType
#define   IF_MTU(port)              port_cdb[port - 1].ifMtu
#define   IF_IFSPEED(port)          port_cdb[port - 1].ifSpeed
#define   IF_ADMIN_STATUS(port)     port_cdb[port - 1].ifAdminStatus
#define   IF_OPER_STATUS(port)      port_cdb[port - 1].ifOperStatus
#define   IF_LAST_CHGE(port)        port_cdb[port - 1].ifLastChange
#define   IF_IN_OCTS(port)          port_cdb[port - 1].ifInOctets
#define   IF_IN_UCAST_PKTS(port)    port_cdb[port - 1].ifInUcastPkts
#define   IF_IN_DISCARDS(port)      port_cdb[port - 1].ifInDiscards
#define   IF_IN_ERRORS(port)        port_cdb[port - 1].ifInErrors
#define   IF_IN_UNKNOWN_PROTO(port) port_cdb[port - 1].ifInUnknownProtos
#define   IF_OUT_OCTS(port)         port_cdb[port - 1].ifOutOctets
#define   IF_OUT_UCAST_PKTS(port)   port_cdb[port - 1].ifOutUcastPkts
#define   IF_OUT_DISCARDS(port)     port_cdb[port - 1].ifOutDiscards
#define   IF_OUT_ERRORS(port)       port_cdb[port - 1].ifOutErrors
#define   IF_STP_STATE(port)        port_cdb[port - 1].pstp_info->state
#define   IF_STP_INFO(port)         port_cdb[port - 1].pstp_info

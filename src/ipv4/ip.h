
struct ip_addr_entry {
        uint32_t   AddrType;
        uint32_t   Addr;    
        uint32_t   AddrMask;    
        uint32_t   IfIndex;
        uint32_t   Type;
        void *     Prefix;      
        uint32_t   Origin;      
        uint32_t   Status;
        uint32_t   Created;     
        uint32_t   LastChanged;
        uint32_t   RowStatus;
        uint32_t   StorageType;
};


extern struct ip_addr_entry ip_port[];

int set_ip_address (uint32_t ifindex, uint32_t ipaddress, uint32_t ipmask);

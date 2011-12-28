.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

DHCP_CODE_DIR = $(CODE_DIR)/dhcp
DHCP_INC_DIR =  $(DHCP_CODE_DIR)/inc -I$(COMMON_INC)
DHCP_SRCC_DIR = $(DHCP_CODE_DIR)
DHCP_OBJ_DIR = $(DHCP_CODE_DIR)/obj


DHCP_OBJS += $(DHCP_OBJ_DIR)/dhcp_cli.o

all: all-before  obj $(DHCP_OBJS) tm_dhcp.o all-after

obj:
	mkdir -p ${DHCP_OBJ_DIR}

clean: clean-custom
	$(RM) $(DHCP_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_dhcp.o

$(DHCP_OBJ_DIR)/dhcp_cli.o: $(DHCP_SRCC_DIR)/dhcp_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(DHCP_SRCC_DIR)/dhcp_cli.c -o $(DHCP_OBJ_DIR)/dhcp_cli.o $(CFLAGS) -I$(DHCP_INC_DIR)

tm_dhcp.o: $(DHCP_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_dhcp.o $(DHCP_OBJS)

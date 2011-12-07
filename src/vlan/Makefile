#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

VLAN_CODE_DIR = .
VLAN_INC_DIR =  $(VLAN_CODE_DIR)/../inc -I$(COMMON_INC)
VLAN_SRCC_DIR = $(VLAN_CODE_DIR)/
VLAN_OBJ_DIR = $(VLAN_CODE_DIR)/obj


VLAN_OBJS += $(VLAN_OBJ_DIR)/vlan_main.o
VLAN_OBJS += $(VLAN_OBJ_DIR)/vlan_cli.o
VLAN_OBJS += $(VLAN_OBJ_DIR)/vlan_stp.o

all: all-before  obj $(VLAN_OBJS) tm_vlan.o all-after

obj:
	mkdir -p ${VLAN_OBJ_DIR}

clean: clean-custom
	${RM} $(VLAN_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_vlan.o

$(VLAN_OBJ_DIR)/vlan_cli.o: $(VLAN_SRCC_DIR)/vlan_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(VLAN_SRCC_DIR)/vlan_cli.c -o $(VLAN_OBJ_DIR)/vlan_cli.o $(CFLAGS) -I$(VLAN_INC_DIR)

$(VLAN_OBJ_DIR)/vlan_main.o: $(VLAN_SRCC_DIR)/vlan_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(VLAN_SRCC_DIR)/vlan_main.c -o $(VLAN_OBJ_DIR)/vlan_main.o $(CFLAGS) -I$(VLAN_INC_DIR)

$(VLAN_OBJ_DIR)/vlan_stp.o: $(VLAN_SRCC_DIR)/vlan_stp.c
	$(CC) -c $(COMPILATION_SWITCH) $(VLAN_SRCC_DIR)/vlan_stp.c -o $(VLAN_OBJ_DIR)/vlan_stp.o $(CFLAGS) -I$(VLAN_INC_DIR)


tm_vlan.o: $(VLAN_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_vlan.o $(VLAN_OBJS)

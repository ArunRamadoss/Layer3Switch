#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

BRIDGE_CODE_DIR = .
BRIDGE_INC_DIR =  $(BRIDGE_CODE_DIR)/inc -I$(COMMON_INC)
BRIDGE_SRCC_DIR = $(BRIDGE_CODE_DIR)
BRIDGE_OBJ_DIR = $(BRIDGE_CODE_DIR)/obj


BRIDGE_OBJS += $(BRIDGE_OBJ_DIR)/fdb.o
BRIDGE_OBJS += $(BRIDGE_OBJ_DIR)/bridge.o
BRIDGE_OBJS += $(BRIDGE_OBJ_DIR)/stpmgr_main.o

all: all-before  obj $(BRIDGE_OBJS) tm_bridge.o all-after

obj:
	mkdir -p ${BRIDGE_OBJ_DIR}

clean: clean-custom
	$(RM) $(BRIDGE_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_bridge.o

$(BRIDGE_OBJ_DIR)/bridge.o: $(BRIDGE_SRCC_DIR)/bridge.c
	$(CC) -c $(COMPILATION_SWITCH) $(BRIDGE_SRCC_DIR)/bridge.c -o $(BRIDGE_OBJ_DIR)/bridge.o $(CFLAGS) -I$(BRIDGE_INC_DIR)

$(BRIDGE_OBJ_DIR)/fdb.o: $(BRIDGE_SRCC_DIR)/fdb.c
	$(CC) -c $(COMPILATION_SWITCH) $(BRIDGE_SRCC_DIR)/fdb.c -o $(BRIDGE_OBJ_DIR)/fdb.o $(CFLAGS) -I$(BRIDGE_INC_DIR)

$(BRIDGE_OBJ_DIR)/stpmgr_main.o: $(BRIDGE_SRCC_DIR)/stpmgr_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(BRIDGE_SRCC_DIR)/stpmgr_main.c -o $(BRIDGE_OBJ_DIR)/stpmgr_main.o $(CFLAGS) -I$(BRIDGE_INC_DIR)

tm_bridge.o: $(BRIDGE_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_bridge.o $(BRIDGE_OBJS)

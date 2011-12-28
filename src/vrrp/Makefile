#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

VRRP_CODE_DIR = $(CODE_DIR)/vrrp
VRRP_INC_DIR =  $(VRRP_CODE_DIR)/../inc -I$(COMMON_INC)
VRRP_SRCC_DIR = $(VRRP_CODE_DIR)/
VRRP_OBJ_DIR = $(VRRP_CODE_DIR)/obj


VRRP_OBJS += $(VRRP_OBJ_DIR)/vrrp_main.o
VRRP_OBJS += $(VRRP_OBJ_DIR)/vrrp_cli.o
VRRP_OBJS += $(VRRP_OBJ_DIR)/vrrpd.o

all: all-before  obj $(VRRP_OBJS) tm_vrrp.o.o all-after

obj:
	mkdir -p ${VRRP_OBJ_DIR}

clean: clean-custom
	${RM} $(VRRP_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_vrrp.o.o

$(VRRP_OBJ_DIR)/vrrp_cli.o: $(VRRP_SRCC_DIR)/vrrp_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(VRRP_SRCC_DIR)/vrrp_cli.c -o $(VRRP_OBJ_DIR)/vrrp_cli.o $(CFLAGS) -I$(VRRP_INC_DIR)

$(VRRP_OBJ_DIR)/vrrp_main.o: $(VRRP_SRCC_DIR)/vrrp_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(VRRP_SRCC_DIR)/vrrp_main.c -o $(VRRP_OBJ_DIR)/vrrp_main.o $(CFLAGS) -I$(VRRP_INC_DIR)

$(VRRP_OBJ_DIR)/vrrpd.o: $(VRRP_SRCC_DIR)/vrrpd.c
	$(CC) -c $(COMPILATION_SWITCH) $(VRRP_SRCC_DIR)/vrrpd.c -o $(VRRP_OBJ_DIR)/vrrpd.o $(CFLAGS) -I$(VRRP_INC_DIR)


tm_vrrp.o.o: $(VRRP_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_vrrp.o.o $(VRRP_OBJS)

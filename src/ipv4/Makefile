#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

IPV4_CODE_DIR = .
IPV4_INC_DIR =  $(IPV4_CODE_DIR)/inc -I$(COMMON_INC)
IPV4_SRCC_DIR = $(IPV4_CODE_DIR)
IPV4_OBJ_DIR = $(IPV4_CODE_DIR)/obj


IPV4_OBJS += $(IPV4_OBJ_DIR)/ip.o
IPV4_OBJS += $(IPV4_OBJ_DIR)/ip_conf.o
IPV4_OBJS += $(IPV4_OBJ_DIR)/ip_cli.o

all: all-before  obj $(IPV4_OBJS) tm_ip.o all-after

obj:
	mkdir -p ${IPV4_OBJ_DIR}

clean: clean-custom
	$(RM) $(IPV4_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_ip.o

$(IPV4_OBJ_DIR)/ip.o: $(IPV4_SRCC_DIR)/ip.c
	$(CC) -c $(COMPILATION_SWITCH) $(IPV4_SRCC_DIR)/ip.c -o $(IPV4_OBJ_DIR)/ip.o $(CFLAGS) -I$(IPV4_INC_DIR)

$(IPV4_OBJ_DIR)/ip_conf.o: $(IPV4_SRCC_DIR)/ip_conf.c
	$(CC) -c $(COMPILATION_SWITCH) $(IPV4_SRCC_DIR)/ip_conf.c -o $(IPV4_OBJ_DIR)/ip_conf.o $(CFLAGS) -I$(IPV4_INC_DIR)

$(IPV4_OBJ_DIR)/ip_cli.o: $(IPV4_SRCC_DIR)/ip_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(IPV4_SRCC_DIR)/ip_cli.c -o $(IPV4_OBJ_DIR)/ip_cli.o $(CFLAGS) -I$(IPV4_INC_DIR)

tm_ip.o: $(IPV4_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_ip.o $(IPV4_OBJS)

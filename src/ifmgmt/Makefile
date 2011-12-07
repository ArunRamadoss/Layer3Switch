#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

INTERF_CODE_DIR = .
INTERF_INC_DIR =  $(INTERF_CODE_DIR)/inc -I$(COMMON_INC)
INTERF_SRCC_DIR = $(INTERF_CODE_DIR)
INTERF_OBJ_DIR = $(INTERF_CODE_DIR)/obj


INTERF_OBJS += $(INTERF_OBJ_DIR)/port.o
INTERF_OBJS += $(INTERF_OBJ_DIR)/if_cli.o

all: all-before  obj $(INTERF_OBJS) tm_port.o all-after

obj:
	mkdir -p ${INTERF_OBJ_DIR}

clean: clean-custom
	rm $(INTERF_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_port.o

$(INTERF_OBJ_DIR)/port.o: $(INTERF_SRCC_DIR)/port.c
	$(CC) -c $(COMPILATION_SWITCH) $(INTERF_SRCC_DIR)/port.c -o $(INTERF_OBJ_DIR)/port.o $(CFLAGS) -I$(INTERF_INC_DIR)

$(INTERF_OBJ_DIR)/if_cli.o: $(INTERF_SRCC_DIR)/if_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(INTERF_SRCC_DIR)/if_cli.c -o $(INTERF_OBJ_DIR)/if_cli.o $(CFLAGS) -I$(INTERF_INC_DIR)
tm_port.o: $(INTERF_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_port.o $(INTERF_OBJS)

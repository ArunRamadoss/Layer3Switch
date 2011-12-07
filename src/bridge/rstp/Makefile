#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../../build/make.h

RSTP_CODE_DIR = .
RSTP_INC_DIR =  $(RSTP_CODE_DIR)/ -I$(COMMON_INC)
RSTP_SRCC_DIR = $(RSTP_CODE_DIR)/
RSTP_OBJ_DIR = $(RSTP_CODE_DIR)/obj


RSTP_OBJS += $(RSTP_OBJ_DIR)/rstp_main.o
RSTP_OBJS += $(RSTP_OBJ_DIR)/bridgestp.o
RSTP_OBJS += $(RSTP_OBJ_DIR)/rstp_cli.o

all: all-before  obj $(RSTP_OBJS) tm_rstp.o all-after

obj:
	mkdir -p ${RSTP_OBJ_DIR}

clean: clean-custom
	${RM} $(RSTP_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_rstp.o

$(RSTP_OBJ_DIR)/bridgestp.o: $(RSTP_SRCC_DIR)/bridgestp.c
	$(CC) -c $(COMPILATION_SWITCH) $(RSTP_SRCC_DIR)/bridgestp.c -o $(RSTP_OBJ_DIR)/bridgestp.o $(CFLAGS) -I$(RSTP_INC_DIR)

$(RSTP_OBJ_DIR)/rstp_cli.o: $(RSTP_SRCC_DIR)/rstp_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(RSTP_SRCC_DIR)/rstp_cli.c -o $(RSTP_OBJ_DIR)/rstp_cli.o $(CFLAGS) -I$(RSTP_INC_DIR)

$(RSTP_OBJ_DIR)/rstp_main.o: $(RSTP_SRCC_DIR)/rstp_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(RSTP_SRCC_DIR)/rstp_main.c -o $(RSTP_OBJ_DIR)/rstp_main.o $(CFLAGS) -I$(RSTP_INC_DIR)
tm_rstp.o: $(RSTP_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_rstp.o $(RSTP_OBJS)

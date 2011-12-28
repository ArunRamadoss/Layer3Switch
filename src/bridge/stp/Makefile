#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../../build/make.h

STP_CODE_DIR = $(CODE_DIR)/bridge/stp
STP_INC_DIR =  $(STP_CODE_DIR)/../inc -I$(COMMON_INC)
STP_SRCC_DIR = $(STP_CODE_DIR)/
STP_OBJ_DIR = $(STP_CODE_DIR)/obj


STP_OBJS += $(STP_OBJ_DIR)/stp_main.o
STP_OBJS += $(STP_OBJ_DIR)/stp_if.o
STP_OBJS += $(STP_OBJ_DIR)/stp_timer.o
STP_OBJS += $(STP_OBJ_DIR)/stp_cli.o
STP_OBJS += $(STP_OBJ_DIR)/llc.o

all: all-before  obj $(STP_OBJS) tm_stp.o all-after

obj:
	mkdir -p ${STP_OBJ_DIR}

clean: clean-custom
	${RM} $(STP_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_stp.o

$(STP_OBJ_DIR)/stp_if.o: $(STP_SRCC_DIR)/stp_if.c
	$(CC) -c $(COMPILATION_SWITCH) $(STP_SRCC_DIR)/stp_if.c -o $(STP_OBJ_DIR)/stp_if.o $(CFLAGS) -I$(STP_INC_DIR)

$(STP_OBJ_DIR)/stp_timer.o: $(STP_SRCC_DIR)/stp_timer.c
	$(CC) -c $(COMPILATION_SWITCH) $(STP_SRCC_DIR)/stp_timer.c -o $(STP_OBJ_DIR)/stp_timer.o $(CFLAGS) -I$(STP_INC_DIR)

$(STP_OBJ_DIR)/stp_cli.o: $(STP_SRCC_DIR)/stp_cli.c
	$(CC) -c $(COMPILATION_SWITCH) $(STP_SRCC_DIR)/stp_cli.c -o $(STP_OBJ_DIR)/stp_cli.o $(CFLAGS) -I$(STP_INC_DIR)

$(STP_OBJ_DIR)/stp_main.o: $(STP_SRCC_DIR)/stp_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(STP_SRCC_DIR)/stp_main.c -o $(STP_OBJ_DIR)/stp_main.o $(CFLAGS) -I$(STP_INC_DIR)
$(STP_OBJ_DIR)/llc.o: $(STP_SRCC_DIR)/llc.c
	$(CC) -c $(COMPILATION_SWITCH) $(STP_SRCC_DIR)/llc.c -o $(STP_OBJ_DIR)/llc.o $(CFLAGS) -I$(STP_INC_DIR)


tm_stp.o: $(STP_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_stp.o $(STP_OBJS)

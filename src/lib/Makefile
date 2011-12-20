#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

LIBS_CODE_DIR = .
LIBS_INC_DIR =  $(LIBS_CODE_DIR)/inc -I$(COMMON_INC)
LIBS_SRCC_DIR = $(LIBS_CODE_DIR)
LIBS_OBJ_DIR = $(LIBS_CODE_DIR)/obj

TIMERS_SRC_DIR = $(LIBS_SRCC_DIR)/src


LIBS_OBJS += $(LIBS_OBJ_DIR)/libinit.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/lock.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/msg_queue.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/tmr_init.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/msg_queue.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/libinit.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/msg_queue.o

all: all-before  obj $(LIBS_OBJS) lib.o all-after

obj:
	mkdir -p ${LIBS_OBJ_DIR}

clean: clean-custom
	rm $(LIBS_OBJ_DIR)/*.o $(FINAL_OBJ)/lib.o

$(LIBS_OBJ_DIR)/libinit.o: $(LIBS_SRCC_DIR)/libinit.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/libinit.c -o $(LIBS_OBJ_DIR)/libinit.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/lock.o: $(LIBS_SRCC_DIR)/lock.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/lock.c -o $(LIBS_OBJ_DIR)/lock.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/msg_queue.o: $(LIBS_SRCC_DIR)/msg_queue.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/msg_queue.c -o $(LIBS_OBJ_DIR)/msg_queue.o $(CFLAGS) -I$(LIBS_INC_DIR)

lib.o: $(LIBS_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/lib.o $(LIBS_OBJS)

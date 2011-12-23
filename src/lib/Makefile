#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

LIBS_CODE_DIR = $(CODE_DIR)/lib
LIBS_INC_DIR =  $(LIBS_CODE_DIR)/inc -I$(COMMON_INC)
LIBS_SRCC_DIR = $(LIBS_CODE_DIR)
LIBS_OBJ_DIR = $(LIBS_CODE_DIR)/obj

TIMERS_SRC_DIR = $(LIBS_SRCC_DIR)/timers
TASK_SRC_DIR = $(LIBS_SRCC_DIR)/task
MEM_SRC_DIR = $(LIBS_SRCC_DIR)/mem


LIBS_OBJS += $(LIBS_OBJ_DIR)/libinit.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/lock.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/util.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/msg_queue.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/hashlib.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/rbtree.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/tmr_init.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/update.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/thr_spec.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/btm_hlf.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/mem_main.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/tsk_lnx.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/tsk_main.o
LIBS_OBJS += $(LIBS_OBJ_DIR)/tsk_util.o


all: all-before  obj $(LIBS_OBJS) lib.o all-after

obj:
	mkdir -p ${LIBS_OBJ_DIR}

clean: clean-custom
	$(RM) $(LIBS_OBJ_DIR)/*.o $(FINAL_OBJ)/lib.o

$(LIBS_OBJ_DIR)/libinit.o: $(LIBS_SRCC_DIR)/libinit.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/libinit.c -o $(LIBS_OBJ_DIR)/libinit.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/util.o: $(LIBS_SRCC_DIR)/util.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/util.c -o $(LIBS_OBJ_DIR)/util.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/lock.o: $(LIBS_SRCC_DIR)/lock.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/lock.c -o $(LIBS_OBJ_DIR)/lock.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/msg_queue.o: $(LIBS_SRCC_DIR)/msg_queue.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/msg_queue.c -o $(LIBS_OBJ_DIR)/msg_queue.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/rbtree.o: $(LIBS_SRCC_DIR)/rbtree.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/rbtree.c -o $(LIBS_OBJ_DIR)/rbtree.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/hashlib.o: $(LIBS_SRCC_DIR)/hashlib.c
	$(CC) -c $(COMPILATION_SWITCH) $(LIBS_SRCC_DIR)/hashlib.c -o $(LIBS_OBJ_DIR)/hashlib.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/tmr_init.o: $(TIMERS_SRC_DIR)/tmr_init.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMERS_SRC_DIR)/tmr_init.c -o $(LIBS_OBJ_DIR)/tmr_init.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/update.o: $(TIMERS_SRC_DIR)/update.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMERS_SRC_DIR)/update.c -o $(LIBS_OBJ_DIR)/update.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/thr_spec.o: $(TIMERS_SRC_DIR)/thr_spec.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMERS_SRC_DIR)/thr_spec.c -o $(LIBS_OBJ_DIR)/thr_spec.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/btm_hlf.o: $(TIMERS_SRC_DIR)/btm_hlf.c
	$(CC) -c $(COMPILATION_SWITCH) $(TIMERS_SRC_DIR)/btm_hlf.c -o $(LIBS_OBJ_DIR)/btm_hlf.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/mem_main.o: $(MEM_SRC_DIR)/mem_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(MEM_SRC_DIR)/mem_main.c -o $(LIBS_OBJ_DIR)/mem_main.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/tsk_lnx.o: $(TASK_SRC_DIR)/tsk_lnx.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRC_DIR)/tsk_lnx.c -o $(LIBS_OBJ_DIR)/tsk_lnx.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/tsk_main.o: $(TASK_SRC_DIR)/tsk_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRC_DIR)/tsk_main.c -o $(LIBS_OBJ_DIR)/tsk_main.o $(CFLAGS) -I$(LIBS_INC_DIR)

$(LIBS_OBJ_DIR)/tsk_util.o: $(TASK_SRC_DIR)/tsk_util.c
	$(CC) -c $(COMPILATION_SWITCH) $(TASK_SRC_DIR)/tsk_util.c -o $(LIBS_OBJ_DIR)/tsk_util.o $(CFLAGS) -I$(LIBS_INC_DIR)


lib.o: $(LIBS_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/lib.o $(LIBS_OBJS)

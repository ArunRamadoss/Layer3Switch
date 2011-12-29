#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

LIBS_CODE_DIR = $(CODE_DIR)/lib
LIBS_INC_DIR =  $(LIBS_CODE_DIR)/ -I$(COMMON_INC)
LIBS_SRCC_DIR = $(LIBS_CODE_DIR)

LIBS_SRCS += $(LIBS_SRCC_DIR)/libinit.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/util.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/lock.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/msg_queue.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/hashlib.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/rbtree.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/tmr_init.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/update.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/thr_spec.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/btm_hlf.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/mem_main.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/tsk_lnx.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/tsk_main.c
LIBS_SRCS += $(LIBS_SRCC_DIR)/tsk_util.c


CFLAGS += -I$(LIBS_INC_DIR)

LIB_OBJ = $(LIBS_SRCS:.c=.o)

all: $(LIB_OBJ) lib.o

$(LIBS_OBJ) : %.o : %.c Makefile
	$(CC) -c $(CFLAGS) $< -o $@
lib.o: $(LIBS_OBJ)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/lib.o $(LIBS_SRCC_DIR)/*.o

clean:
	rm -f *.o $(FINAL_OBJ)/lib.o

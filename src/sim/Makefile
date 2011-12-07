#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

SIM_CODE_DIR = .
SIM_INC_DIR =  $(SIM_CODE_DIR)/inc -I$(COMMON_INC)
SIM_SRCC_DIR = $(SIM_CODE_DIR)/src
SIM_OBJ_DIR = $(SIM_CODE_DIR)/obj

SIM_OBJS += $(SIM_OBJ_DIR)/main_sim.o
SIM_OBJS += $(SIM_OBJ_DIR)/tm_virtl.o

all: all-before  obj $(SIM_OBJS) sim.o all-after

obj:
	mkdir -p ${SIM_OBJ_DIR}

clean: clean-custom
	rm $(SIM_OBJ_DIR)/*.o $(FINAL_OBJ)/sim.o

$(SIM_OBJ_DIR)/main_sim.o: $(SIM_SRCC_DIR)/main_sim.c
	$(CC) -c $(COMPILATION_SWITCH) $(SIM_SRCC_DIR)/main_sim.c -o $(SIM_OBJ_DIR)/main_sim.o $(CFLAGS) -I$(SIM_INC_DIR)

$(SIM_OBJ_DIR)/tm_virtl.o: $(SIM_SRCC_DIR)/tm_virtl.c
	$(CC) -c $(COMPILATION_SWITCH) $(SIM_SRCC_DIR)/tm_virtl.c -o $(SIM_OBJ_DIR)/tm_virtl.o $(CFLAGS) -I$(SIM_INC_DIR)

sim.o: $(SIM_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/sim.o $(SIM_OBJS)

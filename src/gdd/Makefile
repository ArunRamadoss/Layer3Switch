.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

GDD_CODE_DIR = .
GDD_INC_DIR =  $(GDD_CODE_DIR)/inc -I$(COMMON_INC)
GDD_SRCC_DIR = $(GDD_CODE_DIR)
GDD_OBJ_DIR = $(GDD_CODE_DIR)/obj


GDD_OBJS += $(GDD_OBJ_DIR)/gdd_generic.o

all: all-before  obj $(GDD_OBJS) tm_gdd.o all-after

obj:
	mkdir -p ${GDD_OBJ_DIR}

clean: clean-custom
	$(RM) $(GDD_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_gdd.o

$(GDD_OBJ_DIR)/gdd_generic.o: $(GDD_SRCC_DIR)/gdd_generic.c
	$(CC) -c $(COMPILATION_SWITCH) $(GDD_SRCC_DIR)/gdd_generic.c -o $(GDD_OBJ_DIR)/gdd_generic.o $(CFLAGS) -I$(GDD_INC_DIR)
tm_gdd.o: $(GDD_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_gdd.o $(GDD_OBJS)

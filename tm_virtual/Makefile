.PHONY: all all-before all-after clean clean-custom

include make.h

vmgr_CODE_DIR = .
vmgr_INC_DIR =  $(vmgr_CODE_DIR) -I $(COMMON_INC)
vmgr_SRCC_DIR = $(vmgr_CODE_DIR)
vmgr_OBJ_DIR = $(vmgr_CODE_DIR)/obj

vmgr_OBJS += $(vmgr_OBJ_DIR)/main.o

all: all-before  obj $(vmgr_OBJS) tmvirtual all-after

obj:
	mkdir -p ${vmgr_OBJ_DIR}

clean: clean-custom
	${RM} $(OBJ) $(BIN)  $(vmgr_OBJ_DIR)/*.o $(FINAL_OBJ)/* tmvirtual

$(vmgr_OBJ_DIR)/main.o: $(vmgr_SRCC_DIR)/main.c
	$(CC) -c $(COMPILATION_SWITCH) $(vmgr_SRCC_DIR)/main.c -o $(vmgr_OBJ_DIR)/main.o $(CFLAGS) -I$(vmgr_INC_DIR)

tmvirtual:
	gcc -o tmvirtual $(FINAL_OBJ)/*  $(LIB)

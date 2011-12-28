#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

CLI_CODE_DIR = $(CODE_DIR)/cli
CLI_INC_DIR =  $(CLI_CODE_DIR)/inc -I$(COMMON_INC)
CLI_SRCC_DIR = $(CLI_CODE_DIR)/src
CLI_OBJ_DIR = $(CLI_CODE_DIR)/obj

CLI_OBJS += $(CLI_OBJ_DIR)/cli_main.o
CLI_OBJS += $(CLI_OBJ_DIR)/cmd_parser.o
CLI_OBJS += $(CLI_OBJ_DIR)/user_mgmt.o
CLI_OBJS += $(CLI_OBJ_DIR)/login.o
CLI_OBJS += $(CLI_OBJ_DIR)/cli_mode.o

all: all-before  obj $(CLI_OBJS) tm_cli.o all-after

obj:
	mkdir -p ${CLI_OBJ_DIR}

clean: clean-custom
	${RM} $(OBJ) $(BIN)  $(CLI_OBJ_DIR)/*.o $(FINAL_OBJ)/tm_cli.o

$(CLI_OBJ_DIR)/cli_main.o: $(CLI_SRCC_DIR)/cli_main.c
	$(CC) -c $(COMPILATION_SWITCH) $(CLI_SRCC_DIR)/cli_main.c -o $(CLI_OBJ_DIR)/cli_main.o $(CFLAGS) -I$(CLI_INC_DIR)

$(CLI_OBJ_DIR)/cmd_parser.o: $(CLI_SRCC_DIR)/cmd_parser.c
	$(CC) -c $(COMPILATION_SWITCH) $(CLI_SRCC_DIR)/cmd_parser.c -o $(CLI_OBJ_DIR)/cmd_parser.o $(CFLAGS) -I$(CLI_INC_DIR)

$(CLI_OBJ_DIR)/user_mgmt.o: $(CLI_SRCC_DIR)/user_mgmt.c
	$(CC) -c $(COMPILATION_SWITCH) $(CLI_SRCC_DIR)/user_mgmt.c -o $(CLI_OBJ_DIR)/user_mgmt.o $(CFLAGS) -I$(CLI_INC_DIR)

$(CLI_OBJ_DIR)/login.o: $(CLI_SRCC_DIR)/login.c
	$(CC) -c $(COMPILATION_SWITCH) $(CLI_SRCC_DIR)/login.c -o $(CLI_OBJ_DIR)/login.o $(CFLAGS) -I$(CLI_INC_DIR)

$(CLI_OBJ_DIR)/cli_mode.o: $(CLI_SRCC_DIR)/cli_mode.c
	$(CC) -c $(COMPILATION_SWITCH) $(CLI_SRCC_DIR)/cli_mode.c -o $(CLI_OBJ_DIR)/cli_mode.o $(CFLAGS) -I$(CLI_INC_DIR)

tm_cli.o: $(CLI_OBJS)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_cli.o $(CLI_OBJS)

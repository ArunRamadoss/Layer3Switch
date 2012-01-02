.PHONY: all all-before all-after clean clean-custom

include ../../build/make.h

PLAT_LINUX_CODE_DIR = $(CODE_DIR)/platform/linux
PLAT_LINUX_INC_DIR =  $(PLAT_LINUX_CODE_DIR)/inc -I$(COMMON_INC)
PLAT_LINUX_SRCC_DIR = $(PLAT_LINUX_CODE_DIR)/src
HAL_CODE_DIR = $(CODE_DIR)/platform/hal

PLAT_LINUX_SRCS += $(PLAT_LINUX_SRCC_DIR)/if.c
PLAT_LINUX_SRCS += $(PLAT_LINUX_SRCC_DIR)/main_lnx.c
PLAT_LINUX_SRCS += $(PLAT_LINUX_SRCC_DIR)/../../hal/hal_cli.c


CFLAGS += -I$(PLAT_LINUX_INC_DIR)

PLAT_OBJ = $(PLAT_LINUX_SRCS:.c=.o)

all: $(PLAT_OBJ) linux.o

$(PLAT_LINUX_OBJ) : %.o : %.c Makefile
	$(CC) -c $(CFLAGS) $< -o $@
linux.o: $(PLAT_LINUX_OBJ)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/linux.o $(PLAT_LINUX_SRCC_DIR)/*.o $(HAL_CODE_DIR)/*.o

clean:
	rm -f $(PLAT_LINUX_SRCC_DIR)/*.o $(FINAL_OBJ)/linux.o $(HAL_CODE_DIR)/*.o

#   $Id: Makefile,v 1.4 2010/10/25 20:48:19 Sasi Exp $ 
.PHONY: all all-before all-after clean clean-custom

include ../build/make.h

UDP_CODE_DIR = $(CODE_DIR)/udp
UDP_INC_DIR =  $(UDP_CODE_DIR)/ -I$(COMMON_INC)
UDP_SRCC_DIR = $(UDP_CODE_DIR)

UDP_SRCS += $(UDP_SRCC_DIR)/udp_main.c

CFLAGS += -I$(UDP_INC_DIR)

LIB_OBJ = $(UDP_SRCS:.c=.o)

all: $(LIB_OBJ) tm_udp.o

$(UDP_OBJ) : %.o : %.c Makefile
	$(CC) -c $(CFLAGS) $< -o $@
tm_udp.o: $(UDP_OBJ)
	$(LD) $(LD_FLAGS) -o $(FINAL_OBJ)/tm_udp.o $(UDP_SRCC_DIR)/*.o

clean:
	rm -f *.o $(FINAL_OBJ)/tm_udp.o

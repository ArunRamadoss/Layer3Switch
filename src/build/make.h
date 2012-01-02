#!/bin/bash
.SILENT:

#(LINUX,WINDOWS,NONE) /*define OS your are Running */  /*Windows not supported now*/
TARGET_OS = LINUX

ifeq ($(TARGET_OS), LINUX)
COMPILATION_SWITCH += -DLINUX_WANTED
endif

ifeq ($(TARGET_OS), WINDOWS)
COMPILATION_SWITCH += -DWIN32_WANTED
endif

COMMON_INC= $(CODE_DIR)/inc  -I$(CODE_DIR)/lib -I$(CODE_DIR)/cli/inc -I$(CODE_DIR)/cli/src

CC   = echo $(notdir $<); gcc 
CXXFLAGS = $(CXXINCS)  
RM = rm -f
DEBUG_FLAGS = -g -DDEBUG
LD = ld
LD_FLAGS = -r
VERSION= 0.1
CFLAGS = $(DEBUG_FLAGS) -Wall -Wsign-compare -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wchar-subscripts -Wcast-qual -Wpadded

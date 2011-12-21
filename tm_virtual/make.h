#!/bin/bash
.SILENT:

#(LINUX,WINDOWS,NONE) /*define OS your are Running */  /*Windows not supported now*/
TARGET_OS = LINUX

ifeq ($(TARGET_OS), LINUX)
COMPILATION_SWITCH += -DLINUX_WANTED
LIB = -lpthread -lrt
endif

ifeq ($(TARGET_OS), WINDOWS)
COMPILATION_SWITCH += -DWIN32_WANTED
endif

COMMON_INC= $(CODE_DIR)/TechMindsLib/cmn
COMMON_INC= $(CODE_DIR)/../src/inc  -I$(CODE_DIR)/../src/lib

INC_DIR = $(CODE_DIR)/inc
SRC_DIR = $(CODE_DIR)/src
OBJ_DIR = $(CODE_DIR)/obj
LIB_DIR = $(CODE_DIR)/lib
CC   = echo $(notdir $<); gcc 
CXXFLAGS = $(CXXINCS)  
RM = rm -f
DEBUG_FLAGS = -g -DDEBUG
LD = ld
LD_FLAGS = -r
VERSION=1
CFLAGS = $(DEBUG_FLAGS) $(COMPILATION_SWITCH) -Wall -Wsign-compare -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wchar-subscripts -Wcast-qual

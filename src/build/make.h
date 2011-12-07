#!/bin/bash
#.SILENT:

#(LINUX,WINDOWS,NONE) /*define OS your are Running */  /*Windows not supported now*/
TARGET_OS = LINUX
#(SFS, NORMAL)
SCHED = SFS

ifeq ($(TARGET_OS), LINUX)
COMPILATION_SWITCH += -DLINUX_WANTED
endif

ifeq ($(TARGET_OS), WINDOWS)
COMPILATION_SWITCH += -DWIN32_WANTED
endif

ifeq ($(SCHED), SFS)
COMPILATION_SWITCH += -DSFS_WANTED
endif

COMMON_INC=$(CODE_DIR)/inc -I $(CODE_DIR)/TechMindsLib/cmn

CC   = echo $(notdir $<); gcc 
CXXFLAGS = $(CXXINCS)  
RM = rm -f
DEBUG_FLAGS = -g -DDEBUG
LD = ld
LD_FLAGS = -r
VERSION=1
CFLAGS = $(DEBUG_FLAGS) \
	-Wall 

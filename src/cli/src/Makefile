# Makefile for parser library.
# $Id: Makefile 97 2009-03-25 22:49:30Z henry $

# Copyright (c) 2008, Henry Kwok
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the project nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY HENRY KWOK ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL HENRY KWOK BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include ../../build/make.h

SRC_BASE = ..
SRC_FILES = cparser.c cparser_token.c cparser_token_tbl.c cparser_io_unix.c \
	    cparser_fsm.c cparser_line.c cparser_tree.c login.c user_mgmt.c cli_main.c cli_action.c
SRC_MOD = cparser.a

cparser_tree.c: cmd.cli
	$(SRC_BASE)/scripts/mk_parser.py $(CLI_FLAGS) cmd.cli

local_clean:
	rm -fr cparser_tree.c cparser_tree.h

# Define a root build directory based on the platform
ifeq ("$(OUTDIR)", "")
  OUTDIR = .
endif

BUILDDIR = .
OBJDIR   = .
BINDIR   = .
LIBDIR   = $(FINAL_OBJ)

# Add a bunch of default include path
SRC_INC += -I. -I../inc  -I$(SRC_BASE)/inc -I$(COMMON_INC)

SRC_OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC_FILES))
SRC_DEP = $(patsubst %.c,$(OBJDIR)/%.d,$(SRC_FILES))

# Include the list of dependency files
-include $(SRC_DEP)

# Implicit rule to compile all source files
$(OBJDIR)/%.o:%.c
	@echo "  DEP     $<..."
	$(CC) $(CFLAGS) -MM $(SRC_INC) -MF $(patsubst %.o,%.d,$@) -MT $@ $< 
	@echo "  COMPILE $<..."
	$(CC) -c $(CFLAGS) $(SRC_INC) $< -o $@
ifneq ("$(LIBRARY)", "")
	@echo "  ARCHIVE $(notdir $@)..."
	$(AR) rcs $(LIBDIR)/$(LIBRARY) $@
endif

all: lib bin

dir: $(OBJDIR)

lib: dir $(SRC_OBJ)

bin: dir $(BINDIR)/$(SRC_BIN)

$(OBJDIR):
	@echo "  MKDIR obj/$(notdir $@)..."
	mkdir -p $@

clean: local_clean
	rm -f  *.o *.d $(FINAL_OBJ)/libcparser.a

local_clean:

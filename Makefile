#!/usr/bin/make
#
# Usage:
# make (to build libcsv)
# make clean (to remove temporary files)
# make test (to build test file)
#
# When compiling a test file, import
# libdfloat.a into current directory.
#
# Makefile currently includes support for
# gcc as well as untested support for the
# LLVM and Open Watcom compilers.
#
# Modify environment variables for your
# system.

# Default is gcc running on *NIX
# Comment out if not using this setup
TEST_FILE := test.c
COMPILE := gcc
LINK := gcc 
ARCHIVE := ar
DELETE := rm

# Uncomment for Open Watcom on Windows
#COMPILE := wcl386
#LINK := wlink
#ARCHIVE := wlib
#DELETE := del

# Uncomment for LLVM running on *NIX
#COMPILE := clang
#LINK := llvm-ld
#ARCHIVE := llvm-ld
#DELETE := rm

# Add any additional compiler options here.
CMP_OPT :=

# Uncomment the following line if compiling
# in debugging mode:
# DEBUG := true

MACRO :=

ifdef DEBUG
  MACRO += $(if $(findstring gcc, $(COMPILE)),-D_DEBUG,)
  MACRO += $(if $(findstring wcl, $(COMPILE)),-D_DEBUG,)
  MACRO += $(if $(findstring clang, $(COMPILE)),-D_DEBUG,)
endif

# MK_OBJ is the option for compiling without linking
MK_OBJ :=
MK_OBJ += $(if $(findstring gcc, $(COMPILE)),-c,)
MK_OBJ += $(if $(findstring wcl, $(COMPILE)),-c,)
MK_OBJ += $(if $(findstring clang, $(COMPILE)),-c,)

# LNK_OPT contains the options for the linker
LNK_OPT :=
LNK_OPT += $(if $(findstring gcc, $(LINK)),-L . -lcsv -ldfloat,)
LNK_OPT += $(if $(findstring wlink, $(LINK)),LIBPATH . LIBRARY csv.lib dfloat.lib,)
LNK_OPT += $(if $(findstring llvm-ld, $(LINK)),-L . -lcsv -ldfloat,)

# LIBRARY is the filename for the library to be built
LIBRARY :=
LIBRARY += $(if $(findstring ar, $(ARCHIVE)),libcsv.a,)
LIBRARY += $(if $(findstring wlib, $(ARCHIVE)),csv.lib,)
LIBRARY += $(if $(findstring llvm-ld, $(ARCHIVE)),libcsv.a,)

# ARC_CMD accounts for a + before each object filename
# in Open Watcom
ARC_CMD := $(if $(findstring wlib, $(ARCHIVE)),+,)

# Options for the archiving utility
ARC_OPT :=
ARC_OPT += $(if $(findstring ar, $(ARCHIVE)),-rsv,)
ARC_OPT += $(if $(findstring wlib, $(ARCHIVE)),-b -n,)
ARC_OPT += $(if $(findstring llvm-ld, $(ARCHIVE)),-link-as-library -o,)

# The next section accounts for different file extensions
# for the object files.

FILE_OBJ :=
FILE_OBJ += $(if $(findstring gcc, $(COMPILE)),csv_file.o,)
FILE_OBJ += $(if $(findstring wcl, $(COMPILE)),csv_file.obj,)
FILE_OBJ += $(if $(findstring clang, $(COMPILE)),csv_file.o,)

TABLE_OBJ :=
TABLE_OBJ += $(if $(findstring gcc, $(COMPILE)),csv_table.o,)
TABLE_OBJ += $(if $(findstring wcl, $(COMPILE)),csv_table.obj,)
TABLE_OBJ += $(if $(findstring clang, $(COMPILE)),csv_table.o,)

SET_OBJ :=
SET_OBJ += $(if $(findstring gcc, $(COMPILE)),csv_set.o,)
SET_OBJ += $(if $(findstring wcl, $(COMPILE)),csv_set.obj,)
SET_OBJ += $(if $(findstring clang, $(COMPILE)),csv_set.o,)

SELECT_OBJ :=
SELECT_OBJ += $(if $(findstring gcc, $(COMPILE)),csv_select.o,)
SELECT_OBJ += $(if $(findstring wcl, $(COMPILE)),csv_select.obj,)
SELECT_OBJ += $(if $(findstring clang, $(COMPILE)),csv_select.o,)

# Object file that the test file gets compiled into
TEST_OBJ :=
TEST_OBJ += $(if $(findstring gcc, $(LINK)),$(subst .c,.o,$(TEST_FILE)),)
TEST_OBJ += $(if $(findstring wlink, $(LINK)),$(subst .c,.obj,$(TEST_FILE)),)
TEST_OBJ += $(if $(findstring llvm-ld, $(LINK)),$(subst .c,.o,$(TEST_FILE)),)

# Portion of link command to go before the object file when
# compiling the test program
LNK_CMD :=
LNK_CMD += $(if $(findstring gcc, $(LINK)),-o test,)
LNK_CMD += $(if $(findstring wlink, $(LINK)),FILE,)
LNK_CMD += $(if $(findstring llvm-ld, $(LINK)),-o test,)

.PHONY: clean test all

all: $(LIBRARY)
	@echo "Build complete"

# ARCHIVING PHASE:

$(LIBRARY): $(FILE_OBJ) $(TABLE_OBJ) $(SET_OBJ) $(SELECT_OBJ)
	$(ARCHIVE) $(ARC_OPT) $(LIBRARY) $(ARC_CMD)$(FILE_OBJ) $(ARC_CMD)$(TABLE_OBJ) $(ARC_CMD)$(SET_OBJ) $(ARC_CMD)$(SELECT_OBJ)

# COMPILATION PHASE:

$(FILE_OBJ): csv_file.c csv.h automata.h
	$(COMPILE) $(CMP_OPT) $(MACRO) $(MK_OBJ) csv_file.c

$(TABLE_OBJ): csv_table.c csv.h
	$(COMPILE) $(CMP_OPT) $(MK_OBJ) csv_table.c

$(SET_OBJ): csv_set.c csv.h
	$(COMPILE) $(CMP_OPT) $(MK_OBJ) csv_set.c

$(SELECT_OBJ): csv_select.c csv.h
	$(COMPILE) $(CMP_OPT) $(MK_OBJ) csv_select.c

# POST-BUILD PHASE:

clean:
	$(DELETE) $(FILE_OBJ) $(TABLE_OBJ) $(SET_OBJ) $(SELECT_OBJ)

# Must import libdfloat.a
# Test file not included in repository
test:
	$(COMPILE) $(CMP_OPT) $(MK_OBJ) $(TEST_FILE)
	$(LINK) $(LNK_CMD) $(TEST_OBJ) $(LNK_OPT)
	@echo "Build complete"

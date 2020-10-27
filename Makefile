#!/usr/bin/make
#
# Usage:
# make (to build libcsv)
# make clean (to remove temporary files)
# make test (to build test file)
#
# When compiling a test file, import
# libdfloat.a into current directory.

TEST_FILE := test.c

GCC_OPT :=
# Add any additional options for gcc here.

CPP_OPT := #-D_DEBUG
# Uncomment if compiling libcsv in debugging mode.

.PHONY: clean test all

all: libcsv.a
	@echo "Build complete"

# ARCHIVING PHASE:

libcsv.a: csv_file.o csv_table.o
	ar -rsv $@ csv_file.o csv_table.o

# COMPILATION PHASE:

csv_file.o: csv_file.c csv.h automata.h
	gcc $(GCC_OPT) $(CPP_OPT) -c csv_file.c

csv_table.o: csv_table.c csv.h
	gcc $(GCC_OPT) -c csv_table.c

# POST-BUILD PHASE:

clean:
	rm *.o

# Must import libdfloat.a
# Test file not included in repository
test:
	gcc $(GCC_OOPT) $(TEST_FILE) -L . -ldfloat -lcsv

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

CPP_OPT := #-D_DEBUG
# Uncomment if compiling libcsv in debugging mode.

.PHONY: clean test all

all: libcsv.a
	@echo "Build complete"

libcsv.a: csv_file.o csv_table.o
	ar -rsv $@ csv_file.o csv_table.o

csv_file.o: csv_file.c csv.h automata.h
	gcc $(GCC_OPT) $(CPP_OPT) -c csv_file.c

csv_table.o: csv_table.c csv.h
	gcc $(GCC_OPT) -c csv_table.c

clean:
	rm *.o

test:
	gcc $(TEST_FILE) -L . -ldfloat -lcsv

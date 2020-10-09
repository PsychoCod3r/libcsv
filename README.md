libcsv is a CSV library writen for the C programming language. It allows
a user to parse CSV code and read it into an abstract table structure,
as well as write a table back to a file. In the future I will add
functions for reading and modifying the table structures themselves.

---------------------------------------------------------------------------

Why did I write this library?

I wanted to make data science and machine learning programming in C
easier. Currently most ML programming is done in either R or Python,
and neither of these languages allow you to implement your own
algorithms, at least not algorithms that would be at all efficient. libcsv
is designed to be more powerful and flexible than any Python or R library,
giving a C programmer the basic tools for constructing machine learning
algorithms to their own exacting specifications.

---------------------------------------------------------------------------

Who is this library for?

This library is written for power users, control freaks, and just people
who like implementing things themselves. It is written for data scientists
and AI programmers who want a greater degree of flexibility than is
offered by higher-level languages like Python. Its goal is to give the
programmer maximum flexibility and control when designing their own
algorithms, providing a robust data interchange API that the programmer
can build machine learning algorithms on top of, and allowing them to
make their own decisions from thence onward.

Note: libcsv has one dependency: libdfloat. You can find this other
library in my list of repositories. You want to download both repositories
and merge them into one directory before compiling.

---------------------------------------------------------------------------

This repository currently includes the following files:

README - this file

LICENSE - the Michael Warren Free Software License under which this and
all my other software is released

csv.h - the main header file - defines all the data types and functions
related to CSV tables and CSV files

automata.h - a header file that includes macros and types for automata
that are used internally by libcsv; this file is needed when compiling
libcsv but you will not need to #include it in any projects that use
libcsv

csv_file.c - contains function definitions for working with CSV files

parser-demo.c - a demo program for the CSV validator and interpreter,
released very early on in libcsv's development and not really necessary
anymore

doc.txt - documentation of types and functions

------------------------------------------------------------------------

Installation and usage instructions:

1. Download libcsv and libdfloat repositories and merge them into
   one folder.

2. Run the following commands:

   gcc -c csv-file.c dfloat.c

   ar -rsv libcsv.a csv-file.o

   ar -rsv libdfloat.a dfloat.o

3. To link the libcsv and libdfloat libraries to a project, run the
   following command:

   gcc myproject -L dir -lcsv -ldfloat

   (where dir is the directory containing libcsv.a and libdfloat.a)

------------------------------------------------------------------------

If you have any feedback or want to report any issues, please don't
hesitate to notify me. You are helping me make these libraries
better. This library is currently in its alpha stage and needs people
to alpha-test it. Your participation is greatly appreciated.

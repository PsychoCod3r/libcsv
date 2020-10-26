**Introduction:**

libcsv is a CSV library written for the C programming language. It allows
a user to parse CSV code and read it into an abstract table structure, as
well as write a table back to a file. It also combines CSV with the
relational database concept, implementing various SQL operations that
a programmer can use to operate on CSV tables.

---------------------------------------------------------------------------

**Why did I write this library?**

I wanted to make data science and machine learning programming in C
easier. Currently most ML programming is done in either R or Python,
and neither of these languages allow you to implement your own
algorithms, at least not algorithms that would be at all efficient. libcsv
is designed to be more powerful and flexible than any Python or R library,
giving a C programmer the basic tools for constructing machine learning
algorithms to their own exacting specifications.

---------------------------------------------------------------------------

**Who is this library for?**

This library is written for power users, control freaks, and just people
who like implementing things themselves. It is written for data scientists
and AI programmers who want a greater degree of flexibility than is
offered by higher-level languages like Python. Its goal is to give the
programmer maximum flexibility and control when designing their own
algorithms, providing a robust data interchange API that the programmer
can build machine learning algorithms on top of, and allowing them to
make their own decisions from thence onward.

---------------------------------------------------------------------------

**Files included in this repository:**

- README.md - this file

- LICENSE.md - the Michael Warren Free Software License under which this
  and all my other software is released

- CoC.md - the Apolitical Code of Conduct which is used for this project

- DOC.md - documentation of types and functions

- TODO.md - list of modifications I want to make to this software in
  the future

- Makefile - used for building libcsv

- csv.h - the main header file - defines all the data types and functions
  related to CSV tables and CSV files

- automata.h - a header file that includes macros and types for automata
  that are used internally by libcsv; this file is needed when compiling
  libcsv but you will not need to `#include` it in any projects that use
  libcsv

- dfloat.h - header file for numerical types and operations used by libcsv

- csv\_file.c - contains function definitions for working with CSV files

- csv\_table.c - contains function definitions for working with CSV tables
  in the fashion of a relational database

- parser-demo.c - a demo program for the CSV validator and interpreter,
  released very early on in libcsv's development and not really necessary
  anymore

- syntax.vim: Additional syntax highlighting for Vim users

---------------------------------------------------------------------------

**Dependencies:**

[**libdfloat**](https://github.com/PsychoCod3r/libdfloat) (Implements the
main numerical type used by libcsv)

---------------------------------------------------------------------------

**Installation and usage instructions:**

1. Clone both the libcsv and libdfloat repositories.

2. Follow the libdfloat repo's instructions for building libdfloat.

3. Go into the libcsv directory and enter `make` at the command shell.
   To remove any temporary files, follow this with `make clean`.

4. Place libcsv.a and libdfloat.a in the same directory.

5. To link the libcsv and libdfloat libraries to a project, run the
   following command:

   `gcc myproject -L dir -lcsv -ldfloat`

   (where `dir` is the directory containing libcsv.a and libdfloat.a)

---------------------------------------------------------------------------

If you have any feedback or want to report any issues, please don't
hesitate to notify me. You are helping me make these libraries
better. This library is currently in its alpha stage and needs people
to alpha-test it. Your participation is greatly appreciated.

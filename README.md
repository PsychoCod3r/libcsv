libcsv is a CSV library written for the C programming language. It is
currently in the early stages of development, and the only functions I
have implemented so far are the CSV validator and the CSV interpreter
(which builds a table data structure from the CSV file). The file
parser-demo.c is a demonstration of both these functions. In the future
I will be adding further functions, eventually building a library that
will allow a programmer to serialize/deserialize CSV data between the CSV
format and the abstract table structure, as well as provide a complete
interface for performing operations on tables that have been extracted
from CSV files.


This repository currently includes the following files:

README - this file

LICENSE - the Michael Warren Free Software License under which this and
all my other software is released

parser-demo.c - a demo program for the CSV validator and interpreter


Usage for parser-demo.c: <executable> <csv-file> [--noheader]

Compile parser-demo.c with the _DEBUG macro to have the validator print
debugging information to the console.

If anything doesn't work for you, don't hesitate to notify me, so I can
fix it.


I know there's not much here at the moment, but I will be adding more
code very soon, so stay tuned!

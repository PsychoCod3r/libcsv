## Documentation for libcsv

### How to use libcsv

libcsv does two things: it serializes and deserializes CSV tables,
and it provides functions for operating on tables. To process a CSV
file with libcsv, first validate the file with `csv_validate_file()`
to make sure it contains well-formed CSV code, then deserialize the CSV
file into a table structure using `csv_read_table()`. This function
returns a pointer to a `csv_table` structure, which you can operate on
using the SQL functions provided in csv\_table.c. You access the table
through the Current File Pointer, which points to the current record. The
Current File Pointer starts at the beginning of the table, which is simply
a placeholder (meaning it contains no table data). Use `csv_next_record()`
to advance the Current Record Pointer to the next record in the table,
and use the `csv_rewind()` function to rewind it back to the beginning of
the table. Other functions are provided for modifying the table structure
as well as setting and getting the values in individual fields in the
current record. Once you are finished processing a table, you can use
`csv_write_table()` to serialize it into CSV code and write it back to
the CSV file.

---------------------------------------------------------------------------

### Data Types:

libcsv defines the following data types:

`csv_table` - a handle for a table structure

`csv_record` - a handle for a single record within a table

`csv_field` - a single field in the CSV header

Usually only the `csv_table` structure needs to be accessed directly.
`csv_field` only needs to be used when initializing a table, and `csv_record`
is only used internally by the library.

libcsv also defines the following additional data types:

`csv_set` - set type representing a subset of the rows of a table

`csv_partition` - a binary partition of a table into two subtables

---------------------------------------------------------------------------

### CSV File Functions:

libcsv defines the following functions for operating on CSV files:

---

`bool csv_validate_file( FILE *fp, bool has_header )`

Takes a file pointer for the CSV file and a Boolean indicating whether
the file is supposed to have a header; returns `true` if the file is valid
CSV, `false` otherwise

---

`csv_table *csv_read_table( FILE *fp, bool has_header )`

Reads a CSV file and builds a table structure based on that file,
returning a pointer to the table structure

---

`void csv_write_table( FILE *fp, csv_table *table, bool has_header )`

Writes a CSV table structure back to the file pointed to by `fp`

---------------------------------------------------------------------------

### SQL/Table Functions

libcsv defines several functions that are roughly analogous to commands
used in a SQL database. They are as follows:

---

`csv_table *csv_create_table( int rlen, csv_field **field_vector )`

Creates a new CSV table, initializing it with `rlen` fields specified by
`field_vector`

---

`void csv_drop_table( csv_table *table )`

Frees a table structure and sets its pointer to `NULL`

---

`csv_record *csv_next_record( csv_table *table )`

Moves the Current Record Pointer to the next record in the table and
returns `NULL` if there are no more records

---

`void csv_rewind( csv_table *table )`

Rewinds the Current Record Pointer to the beginning of the table, which
is a placeholder record (you have to use `csv_next_record()` from here to
get to the first record in the table)

---

`void csv_insert_record( csv_table *table, void **record )`

Appends the record given by `record` to the end of the table; this record
should include `void` pointers with values copied into them using either
`memcpy()` (for `dfloat` numbers) or `strncpy()` (for strings)

---

`void csv_insert_new_record( csv_table *table )`

Adds a blank record to the end of the table and then moves the Current
Record Pointer to that record so that its fields can be populated using
the setter functions listed later in this document.

---

`void csv_delete_current_record( csv_table *table )`

Deletes the record pointed to by the Current Record Pointer

---

`dfloat64_t *csv_get_number_field_by_name( csv_table *table, char *name )`

Returns the numeric value stored in the field in the current record
given by `name`

---

`dfloat64_t *csv_get_number_field_by_index( csv_table *table, int index )`

Returns the numeric value stored in the field in the current record
given by `index`

---

`char *csv_get_string_field_by_name( csv_table *table, char *name )`

Returns the string stored in the field in the current record given by
`name`

---

`char *csv_get_string_field_by_index( csv_table *table, int index )`

Returns the string stored in the field in the current record given by
`index`

---

`void csv_set_number_field_by_name( csv_table *table, char *name, dfloat64_t *val )`

Sets the number field in the current record given by `name` to the value
given by `val`

---

`void csv_set_number_field_by_index( csv_table *table, int index, dfloat64_t *val )`

Sets the number field in the current record given by `index` to the value
given by `val`

---

`void csv_set_string_field_by_name( csv_table *table, char *name, char *val )`

Copies the string given by `val` into the string field in the current
record given by `name`

---

`void csv_set_string_field_by_index( csv_table *table, int index, char *val )`

Copies the string given by `val` into the string field in the current
record given by `name`

---

You can also use the following abbreviations for the setter and getter functions:

`csv_gnfbn()` for `csv_get_number_field_by_name()`

`csv_gnfbi()` for `csv_get_number_field_by_index()`

`csv_gsfbn()` for `csv_get_string_field_by_name()`

`csv_gsfbi()` for `csv_get_string_field_by_index()`

`csv_snfbn()` for `csv_set_number_field_by_name()`

`csv_snfbi()` for `csv_set_number_field_by_index()`

`csv_ssfbn()` for `csv_set_string_field_by_name()`

`csv_ssfbi()` for `csv_set_string_field_by_index()`

--------------------------------------------------------------------------

### Set and Row Selection Functions

libcsv defines functions for working with sets and generating subset
tables.

---

`csv_set *csv_empty_set( int size )`

Returns an empty set of the given size

---

`csv_set *csv_set_universe( int size )`

Returns the universal set of the given size (the set containing all
elements/rows up to that number)

---

`void csv_set_add( csv_set *set, int element )`

Adds the given element to the given set

---

`void csv_set_del( csv_set *set, int element )`

Removes the given element from the given set

---

`bool csv_set_member( int element, csv_set *set )`

Returns `true` if the given element is a member of the given set,
`false` otherwise

---

`void csv_set_difference( csv_set *dst, csv_set *src )`

Computes the set difference between the two operands, subtracting `src`
from `dst` and storing the result in `dst`

---

`void csv_set_complement( csv_set *dst )`

Computes the complement of `dst` and stores it in `dst`

---

`void csv_set_union( csv_set *dst, csv_set *src )`

Computes the set union of `src` and `dst` and stores the result in `dst`

---

`void csv_set_intersection( csv_set *dst, csv_set *src )`

Computes the set intersection of `src` and `dst` and stores the result in
`dst`

---

`csv_set *csv_read_set( char * )`

Takes a string with a hexadecimal representation of a set and reads the
set from it, used mostly for debugging purposes

---

`char *csv_write_set( csv_set * )`

Writes a hexadecimal representation of a set to a string

---

**The next five functions are designed for building complex expressions
without lost objects accumulating.**

---

`csv_set *csv_set_difference_f( csv_set *set1, csv_set *set2 )`

Returns the set difference between the two operands, freeing both of
them in the process

---

`csv_set *csv_set_complement_f( csv_set *set )`

Returns the complement of the operand, freeing it in the process

---

`csv_set *csv_set_union_f( csv_set *set1, csv_set *set2 )`

Returns the union of the two operands, freeing both of them in the
process

---

`csv_set *csv_set_intersection_f( csv_set *set1, csv_set *set2 )`

Returns the intersection of the two operands, freeing both of them in
the process

---

`char *csv_write_set_f( csv_set *set )`

Writes the hexadecimal representation of a set to a string, freeing that
set in the process

---

**The remaining functions are used for generating subsets from conditions
and generating new tables from those subsets.**

---

`csv_set *csv_select_subset( csv_table *table, enum operators op, char *op1, char *op2 )`

Generates a subset representing the rows of `table` that match the
condition given by the operator `op` and operands `op1` and `op2`

`op1` is typically a field and `op2` is typically an immediate value,
unless `op` is `MOD`, in which case `op1` is the base of the modulus
operator and `op2` is the residue class.

`op` is one of the following:

- `EQ` for number field `op1` == numeric value `op2`

- `NE` for number field `op1` != numeric value `op2`

- `LT` for number field `op1` > numeric value `op2`

- `GT` for number field `op1` < numeric value `op2`

- `LE` for number field `op1` <= numeric value `op2`

- `GE` for number field `op1` >= numeric value `op2`

- `MOD` for row number % `op1` == `op2`

- `SEQ` for string field `op1` == string value `op2`

- `SNE` for string field `op1` != string value `op2`

---

`csv_table *csv_select_records_by_subset( csv_table *table, csv_set *subset )`

Generates a new table containing all the rows of `table` represented by
`subset`

---

`csv_partition *csv_partition_table_by_subset( csv_table *table, csv_set *subset )`

Generates a partition `part` of `table` with `part.ident` containing
all the rows represented by `subset` and `part.cplmt` containing all
the rows not represented by `subset`

--------------------------------------------------------------------------

Any questions or problems? Feel free to contact me at the following:

Github: github.com/PsychoCod3r

Personal email: acidkicks@protonmail.com

Submit issues at github.com/PsychoCod3r/libcsv

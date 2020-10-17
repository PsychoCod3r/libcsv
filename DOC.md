## Documentation for libcsv

### How to use libcsv

libcsv does two things: it serializes and deserializes CSV tables,
and it provides functions for operating on tables. To process a CSV
file with libcsv, first validate the file with `csv_validate_file()`
to make sure it contains well-formed CSV code, then deserialize the CSV
file into a table structure using `csv_read_table()`. This function
returns a pointer to a `csv_table` structure, which you can operate on
using the SQL functions provided in csv_table.c. You access the table
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

---------------------------------------------------------------------------

### CSV File Functions:

libcsv defines the following functions for operating on CSV files:

`bool csv_validate_file( FILE *fp, bool has_header )`

Takes a file pointer for the CSV file and a Boolean indicating whether
the file is supposed to have a header; returns `true` if the file is valid
CSV, `false` otherwise

`csv_table *csv_read_table( FILE *fp, bool has_header )`

Reads a CSV file and builds a table structure based on that file,
returning a pointer to the table structure

`void csv_write_table( FILE *fp, csv_table *table, bool has_header )`

Writes a CSV table structure back to the file pointed to by `fp`

---------------------------------------------------------------------------

### SQL/Table Functions

libcsv defines several functions that are roughly analogous to commands
used in a SQL database. They are as follows:

`csv_table *csv_create_table( int rlen, csv_field **field_vector )`

Creates a new CSV table, initializing it with `rlen` fields specified by
`field_vector`

`void csv_drop_table( csv_table *table )`

Frees a table structure and sets its pointer to `NULL`

`csv_record *csv_next_record( csv_table *table )`

Moves the Current Record Pointer to the next record in the table and
returns `NULL` if there are no more records

`void csv_rewind( csv_table *table )`

Rewinds the Current Record Pointer to the beginning of the table, which
is a placeholder record (you have to use `csv_next_record()` from here to
get to the first record in the table)

`void csv_insert_record( csv_table *table, void **record )`

Appends the record given by `record` to the end of the table; this record
should include `void` pointers with values copied into them using either
`memcpy()` (for `dfloat` numbers) or `strncpy()` (for strings)

`void csv_delete_current_record( csv_table *table )`

Deletes the record pointed to by the Current Record Pointer

`dfloat64_t *csv_get_number_field_by_name( csv_table *table, char *name )`

Returns the numeric value stored in the field in the current record
given by `name`

`dfloat64_t *csv_get_number_field_by_index( csv_table *table, int index )`

Returns the numeric value stored in the field in the current record
given by `index`

`char *csv_get_string_field_by_name( csv_table *table, char *name )`

Returns the string stored in the field in the current record given by
`name`

`char *csv_get_string_field_by_index( csv_table *table, int index )`

Returns the string stored in the field in the current record given by
`index`

`void csv_set_number_field_by_name( csv_table *table, char *name, dfloat64_t *val )`

Sets the number field in the current record given by `name` to the value
given by `val`

`void csv_set_number_field_by_index( csv_table *table, int index, dfloat64_t *val )`

Sets the number field in the current record given by `index` to the value
given by `val`

`void csv_set_string_field_by_name( csv_table *table, char *name, char *val )`

Copies the string given by `val` into the string field in the current
record given by `name`

`void csv_set_string_field_by_index( csv_table *table, int index, char *val )`

Copies the string given by `val` into the string field in the current
record given by `name`

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

Any questions or problems? Feel free to contact me at the following:

Github: github.com/PsychoCod3r

Personal email: acidkicks@protonmail.com

Submit issues at github.com/PsychoCod3r/libcsv

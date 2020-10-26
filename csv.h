/**********************************************
 * libcsv, Version 0.2.2 Alpha                *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: September 2020                       *
 * License: Michael Warren FSL Version 1.1    *
 * Current module: Header file for CSV types  *
 *                 and functions              *
 **********************************************/

#ifndef _CSV_
#define _CSV_

#include <stdio.h>
#include <stdbool.h>
#include "dfloat.h"

enum types { csv_number, csv_string };

// Metadata field for the table header
typedef struct {
	char *name;
	enum types type;
} csv_field;

// Single record in linked list table structure
struct _csv_record {
	void **record;
	struct _csv_record *next;
};

typedef struct _csv_record csv_record;

// Handle for abstract table structure
typedef struct {
	int rlen;           // # of fields in each record
	csv_field **header; // Table metadata
	csv_record *start;  // Pointer to first record
	csv_record *cur;    // Pointer to current record
} csv_table;

// Handles end-of-line sequence:
#if defined (_WIN16) || defined (_WIN32) || defined (_WIN64) || defined (__WIN32__) || defined (__TOS_WIN__) || defined (__WINDOWS__)
# define _EOL_ "\r\n"
#elif defined (OS2) || defined(_OS2) || defined(__OS2__) || defined (__TOS_OS2__)
# define _EOL_ "\r\n"
#elif defined (MSDOS) || defined (__MSDOS__) || defined (_MSDOS) || defined (__DOS__)
# define _EOL_ "\r\n"
#elif defined (macintosh) || defined (Macintosh)
# define _EOL_ "\r"
#else
# define _EOL_ "\n"
#endif

// Shorthand functions:
#define csv_gnfbn csv_get_number_field_by_name
#define csv_gnfbi csv_get_number_field_by_index
#define csv_gsfbn csv_get_string_field_by_name
#define csv_gsfbi csv_get_string_field_by_index
#define csv_snfbn csv_set_number_field_by_name
#define csv_snfbi csv_set_number_field_by_index
#define csv_ssfbn csv_set_string_field_by_name
#define csv_ssfbi csv_set_string_field_by_index

__BEGIN_DECLS
bool csv_validate_file( FILE *, bool );
csv_table *csv_read_table( FILE *, bool );
void csv_write_table( FILE *, csv_table *, bool );
csv_table *csv_create_table( int, csv_field ** );
void csv_drop_table( csv_table * );
csv_table *csv_alter_table_add( csv_table *, csv_field * );
csv_table *csv_alter_table_drop( csv_table *, char * );
csv_record *csv_next_record( csv_table * );
void csv_rewind( csv_table * );
void csv_insert_record( csv_table *, void ** );
void csv_insert_new_record( csv_table * );
void csv_delete_current_record( csv_table * );
dfloat64_t *csv_get_number_field_by_name( csv_table *, char * );
dfloat64_t *csv_get_number_field_by_index( csv_table *, int );
char *csv_get_string_field_by_name( csv_table *, char * );
char *csv_get_string_field_by_index( csv_table *, int );
void csv_set_number_field_by_name( csv_table *, char *, dfloat64_t * );
void csv_set_number_field_by_index( csv_table *, int, dfloat64_t * );
void csv_set_string_field_by_name( csv_table *, char *, char * );
void csv_set_string_field_by_index( csv_table *, int, char * );
__END_DECLS

#endif

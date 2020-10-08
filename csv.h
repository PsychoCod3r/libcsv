/**********************************************
 * libcsv, Version 0.1 Alpha                  *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: September 2020                       *
 * License: Michael Warren FSL                *
 * Current module: Header file for CSV types  *
 *                 and functions              *
 **********************************************/

#ifndef _CSV_
#define _CSV_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

__BEGIN_DECLS
bool csv_validate_file( FILE *, bool );
csv_table *csv_read_table( FILE *, bool );
void csv_write_table( FILE *, csv_table *, bool );
__END_DECLS

#endif

/**********************************************
 * libcsv, Version 0.2.2 Alpha                *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: September 2020                       *
 * License: Michael Warren FSL Version 1.1    *
 * Current module: Functions implementing SQL *
 *                 operations on CSV tables   *
 **********************************************/

#include <stdlib.h>
#include <string.h>
#include "csv.h"
#include "dfloat.h"

// Select next record
// Return NULL if end of table is reached
csv_record *csv_next_record( csv_table *table ){
	if( table->cur->next ){
		table->cur = table->cur->next;
		return table->cur;
	}
	return NULL;
}

// Rewind to the beginning of the table
void csv_rewind( csv_table *table ){
	table->cur = table->start;
}

// Equivelent to DROP TABLE in SQL
void csv_drop_table( csv_table *table ){
	csv_record *tmp;
	int f;

	free( table->header );

	// Free table records:
	csv_rewind( table );
	while( csv_next_record( table ) ){
		for( f = 0; f < table->rlen; f++ ){
			free( table->cur->record[f] );
		}
		tmp = table->cur;
		table->cur = table->cur->next;
		free( tmp );
	}

	// Next part prevents dangling pointer problems.
	table->start = NULL;
	table->cur = NULL;
	table->header = NULL;
	free( table );
	table = NULL;
}

// Equivalent to CREATE TABLE in SQL
csv_table *csv_create_table( int rlen, csv_field **field_vector ){
	csv_table *table;
	int f;
	table = (csv_table *) malloc( sizeof( csv_table ) );
	table->rlen = rlen;
	table->header = (csv_field **) calloc( rlen, sizeof( csv_field * ) );
	for( f = 0; f < rlen; f++ ){
		table->header[f] = (csv_field *) malloc( sizeof( csv_field ) );
		memcpy( table->header[f], field_vector[f], sizeof( csv_field ) );
	}
	table->start = (csv_record *) malloc( sizeof( csv_record ) );
	table->cur = table->start;
}

// Equivalent to INSERT INTO in SQL
void csv_insert_record( csv_table *table, void **record ){
	csv_record *save;
	int f;
	int len;
	save = table->cur;
	while( csv_next_record( table ) );
	table->cur->next = (csv_record *) calloc( 1, sizeof( csv_record ) );
	csv_next_record( table );
	table->cur->record = (void **) calloc( table->rlen, sizeof( void * ) );
	for( f = 0; f < table->rlen; f++ ){
		if( table->header[f]->type == csv_number ){
			table->cur->record[f] = malloc( sizeof( dfloat64_t ) );
			memcpy( table->cur->record[f], record[f], sizeof( dfloat64_t ) );
		}
		else if( table->header[f]->type == csv_string ){
			len = strlen( (char *) record[f] );
			table->cur->record[f] = malloc( len + 1 );
			strncpy( table->cur->record[f], record[f], len + 1 );
		}
	}
	table->cur->next = NULL;
	table->cur = save;
}

// INSERT a blank record and move to that record
void csv_insert_new_record( csv_table *table ){
	int f;
	while( csv_next_record( table ) );
	table->cur->next = (csv_record *) calloc( 1, sizeof( csv_record ) );
	csv_next_record( table );
	table->cur->record = (void **) calloc( table->rlen, sizeof( void * ) );
	for( f = 0; f < table->rlen; f++ ){
		if( table->header[f]->type == csv_number )
			table->cur->record[f] = malloc( sizeof( dfloat64_t ) );
		else if( table->header[f]->type == csv_string )
			table->cur->record[f] = malloc( 1 );
	}
	table->cur->next = NULL;
}

// Equivalent to DELETE FROM in SQL except it deletes the
// current record rather than those matching a condition
void csv_delete_current_record( csv_table *table ){
	csv_record *tmp1;
	csv_record *tmp2;
	int f;
	tmp1 = table->start;
	tmp2 = NULL;
	while( tmp1->next != table->cur )
		tmp1 = tmp1->next;
	if( table->cur->next )
		tmp2 = table->cur->next;
	for( f = 0; f < table->rlen; f++ )
		free( table->cur->record[f] );
	free( table->cur );
	if( tmp2 )
		tmp1->next = tmp2;
	else
		tmp1->next = NULL;
	table->cur = tmp1;
}

// Used to retrieve numeric values from the table
dfloat64_t *csv_get_number_field_by_name( csv_table *table, char *name ){
	dfloat64_t *df;
	int f;
	for( f = 0; f < table->rlen; f++ ){
		if( !strcmp( table->header[f]->name, name ) )
			break;
	}
	if( f == table->rlen )
	// Error: Name not found
		return NULL;
	if( table->header[f]->type != csv_number )
	// Error: Type mismatch
		return NULL;
	df = (dfloat64_t *) malloc( sizeof( dfloat64_t ) );
	memcpy( df, table->cur->record[f], sizeof( dfloat64_t ) );
	return df;
}

// Used to retrieve numeric values from the table
dfloat64_t *csv_get_number_field_by_index( csv_table *table, int index ){
	dfloat64_t *df;
	if( index < 0 || index >= table->rlen )
	// Error: Out-of-bounds
		return NULL;
	if( table->header[index]->type != csv_number )
	// Error: Type mismatch
		return NULL;
	df = (dfloat64_t *) malloc( sizeof( dfloat64_t ) );
	memcpy( df, table->cur->record[index], sizeof( dfloat64_t ) );
	return df;
}

// Used to retrieve string values from the table 
char *csv_get_string_field_by_name( csv_table *table, char *name ){
	char *str;
	int f;
	int len;
	for( f = 0; f < table->rlen; f++ ){
		if( !strcmp( table->header[f]->name, name ) )
			break;
	}
	if( f == table->rlen )
	// Error: Name not found
		return NULL;
	if( table->header[f]->type != csv_string )
	// Error: Type mismatch
		return NULL;
	len = strlen( table->cur->record[f] );
	str = (char *) malloc( len + 1 );
	strncpy( str, table->cur->record[f], len + 1 );
	return str;
}

// Used to retrieve string values from the table 
char *csv_get_string_field_by_index( csv_table *table, int index ){
	char *str;
	int len;
	if( index < 0 || index >= table->rlen )
	// Error: Out-of-bounds
		return NULL;
	if( table->header[index]->type != csv_string )
	// Error: Type mismatch
		return NULL;
	len = strlen( table->cur->record[index] );
	str = (char *) malloc( len + 1 );
	strncpy( str, table->cur->record[index], len + 1 );
	return str;
}

// Used to set the value of a number field
void csv_set_number_field_by_name( csv_table *table, char *name, dfloat64_t *val ){
	int f;
	for( f = 0; f < table->rlen; f++ ){
		if( !strcmp( table->header[f]->name, name ) )
			break;
	}
	if( f == table->rlen )
	// Error: Name not found
		return;
	if( table->header[f]->type != csv_number )
	// Error: Type mismatch
		return;
	memcpy( table->cur->record[f], val, sizeof( dfloat64_t ) );
}

// Used to set the value of a number field
void csv_set_number_field_by_index( csv_table *table, int index, dfloat64_t *val ){
	if( index < 0 || index >= table->rlen )
	// Error: Out-of-bounds
		return;
	if( table->header[index]->type != csv_number )
	// Error: Type mismatch
		return;
	memcpy( table->cur->record[index], val, sizeof( dfloat64_t ) );
}

// Used to set the value of a string field
void csv_set_string_field_by_name( csv_table *table, char *name, char *val ){
	int f;
	int len;
	for( f = 0; f < table->rlen; f++ ){
		if( !strcmp( table->header[f]->name, name ) )
			break;
	}
	if( f == table->rlen )
	// Error: Name not found
		return;
	if( table->header[f]->type != csv_string )
	// Error: Type mismatch
		return;
	len = strlen( val );
	table->cur->record[f] = realloc( table->cur->record[f], len + 1 );
	strncpy( table->cur->record[f], val, len + 1 );
}

// Used to set the value of a string field
void csv_set_string_field_by_index( csv_table *table, int index, char *val ){
	int len;
	if( index < 0 || index >= table->rlen )
	// Error: Out-of-bounds
		return;
	if( table->header[index]->type != csv_string )
	// Error: Type mismatch
		return;
	len = strlen( val );
	table->cur->record[index] = realloc( table->cur->record[index], len + 1 );
	strncpy( table->cur->record[index], val, len + 1 );
}

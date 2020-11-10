/**********************************************
 * libcsv, Version 0.3 Alpha                  *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: November 2020                        *
 * License: Michael Warren FSL Version 1.1    *
 * Current module: Implementation of SQL      *
 *                 SELECT command             *
 **********************************************/

#include <stdlib.h>
#include <string.h>
#include "csv.h"
#include "dfloat.h"

// Creates a set type indexing the records in a table that
// match the expression given by the operator and operand
csv_set *csv_select_subset( csv_table *table, enum operators operator, char *field, char *value ){
	csv_set *subset;
	int rcount; // # of records in table
	csv_record *save;
	int i, f;
	dfloat64_t *number_field;
	dfloat64_t *number_value;
	int int1, int2;
	char *string_field;
	save = table->cur;

	// Find the field with the given name:
	if( operator != MOD ){
		for( f = 0; f < table->rlen; f++ ){
			if( !strcmp( table->header[f]->name, field ) )
				break;
		}
		if( f == table->rlen )
		// Error: Name not found
			return NULL;
	}

	// Count records in table and use record
	// count to create an empty set object:
	csv_rewind( table );
	rcount = 0;
	while( csv_next_record( table ) )
		rcount++;
	csv_rewind( table );
	subset = csv_empty_set( rcount );

	for( i = 0; i < rcount; i++ ){
		csv_next_record( table );
		if( operator == EQ || operator == NE || operator == LT || operator == GT || operator == LE || operator == GE ){
		// Numerical comparison operators
			if( table->header[f]->type != csv_number )
				return NULL;
			number_field = csv_get_number_field_by_name( table, field );
			number_value = dfloat64_atof( value );
			switch( operator ){
				case EQ : if( !dfloat64_cmp( number_field, number_value ) )
						csv_set_add( subset, i );
				          break;
				case NE : if( dfloat64_cmp( number_field, number_value ) )
						csv_set_add( subset, i );
				          break;
				case LT : if( dfloat64_cmp( number_field, number_value ) == -1 )
						csv_set_add( subset, i );
				          break;

				case GT : if( dfloat64_cmp( number_field, number_value ) == 1 )
						csv_set_add( subset, i );
				          break;

				case LE : if( dfloat64_cmp( number_field, number_value ) <= 0 )
						csv_set_add( subset, i );
				          break;
				case GE : if( dfloat64_cmp( number_field, number_value ) >= 0 )
						csv_set_add( subset, i );
				          break;
			}
			free( number_field );
			free( number_value );
		}
		else if( operator == MOD ){
		// Modulus operator
			int1 = atoi( field );
			int2 = atoi( value );
			if( i % int1 == int2 )
				csv_set_add( subset, i );
		}
		else if( operator == SEQ || operator == SNE ){
		// String comparison operators
			if( table->header[f]->type != csv_string )
				return NULL;
			string_field = csv_get_string_field_by_name( table, field );
			if( operator == SEQ ){
				if( !strcmp( string_field, value ) )
					csv_set_add( subset, i );
			}
			else{
				if( strcmp( string_field, value ) )
					csv_set_add( subset, i );
			}
		}
	}
	table->cur = save;
	return subset;
}

// Creates a new table consisting of all the records in the
// given table indexed by the given set type
csv_table *csv_select_records_by_subset( csv_table *table, csv_set *subset ){
	csv_record *save;
	csv_table *subtab;
	int rnum;
	save = table->cur;
	subtab = csv_create_table( table->rlen, table->header );
	csv_rewind( table );
	rnum = 0;
	// Loop copies from table to subtab all records whose bit in the
	// given subset is set
	while( csv_next_record( table ) ){
		if( csv_set_member( rnum++, subset ) )
			csv_insert_record( subtab, table->cur->record );
	}
	table->cur = save;
	return subtab;
}

// Returns a partition including the subset and its complement
csv_partition *csv_partition_table_by_subset( csv_table *table, csv_set *subset ){
	csv_record *save;
	csv_partition *partition;
	int rnum;
	save = table->cur;
	partition = (csv_partition *) malloc( sizeof( csv_partition ) );
	partition->ident = csv_create_table( table->rlen, table->header );
	partition->cplmt = csv_create_table( table->rlen, table->header );
	rnum = 0;
	// Loop copies all records with 1 bits to ident and all records
	// with 0 bits to cplmt
	while( csv_next_record( table ) ){
		if( csv_set_member( rnum++, subset ) )
			csv_insert_record( partition->ident, table->cur->record );
		else
			csv_insert_record( partition->cplmt, table->cur->record );
	}
	table->cur = save;
	return partition;
}

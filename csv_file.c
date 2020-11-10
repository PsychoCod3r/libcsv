/**********************************************
 * libcsv, Version 0.3 Alpha                  *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: November 2020                        *
 * License: Michael Warren FSL Version 1.1    *
 * Current module: Functions for operating on *
 *                 CSV files                  *
 **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include "csv.h"
#include "automata.h"
#include "dfloat.h"

// Used in case the last line of the
// file is not newline-terminated
bool is_eof( FILE *fp ){
	int c;
	bool end;
	end = ((c = fgetc( fp )) == EOF );
	ungetc( c, fp );
	return end;
}

// Read a single line from the file
#define read_line( buf, size, fp )\
	if( buf ) free( buf );\
	size = 64;\
	buf = (char *) malloc( size );\
	fgets( buf, size, fp );\
	while( buf[strlen( buf ) - 1] != '\n' && !is_eof( fp ) ){\
		size <<= 1;\
		buf = (char *) realloc( buf, size );\
		fgets( buf + (size >> 1) - 1, size >> 1, fp );\
	}\
	size = 64

// Returns true if valid CSV, false otherwise
bool csv_validate_file( FILE *fp, bool has_header ){
	long pos;
	int c;
	int state;
	tm_head *head;
	pos = ftell( fp );
	rewind( fp );

	// Turing machine setup:
	state = MASTER;
	head = (tm_head *) calloc( 1, sizeof( tm_head ) );
	head->symbol = START1;

	// Turing machine main loop:
#ifdef _DEBUG
	puts("-------------------------------------------------------------------------------" );
#endif
	while( (c = fgetc( fp )) != EOF ){
#ifdef _DEBUG
			printf( "Current state: %s\n", state_strings[state] );
			printf( "Input symbol: %c,0x%s%x\n", c, (c<0x10)?"0":"", c );
			printf( "Current tape symbol: %s\n", symbol_strings[head->symbol] );
#endif
		if( state == TRAP ); // Do nothing
		else if( c == '\r' ); // Ignore carriage returns
		else if( head->right == NULL && head->symbol != STOP && head->symbol != STRING_STOP && head->symbol != NUMBER_STOP ){
		// True if reading the first record
			if( has_header ){
			// True if reading the header
				if( state == MASTER ){
					if( c == '\"' ){
						head->right = (tm_tape *) calloc( 1, sizeof( tm_tape ) );
						head->right->left = head;
						move_head_right( head );
						head_write( head, BLANK );
						state = S_TEXT;
					}
					else state = TRAP;
				}
				else if( state == S_TEXT ){
					if( c == '\"' ) state = S_FINAL;
					else if( c == '\n' ) state = TRAP;
				}
				else if( state == S_FINAL ){
					if( c == ',' ) state = MASTER;
					else if( c == '\n' ){
						head_write( head, STOP );
						state = END_HDR;
					}
					else state = TRAP;
				}
			}
			else{
			// True if reading the first record in a CSV file with no header
				if( head_read( head ) == START1 ){
				// True if at the beginning of the first record
					head_write( head, START2 );
				}
				if( state == MASTER ){
				// True if at the beginning of a field
					head->right = (tm_tape *) calloc( 1, sizeof( tm_tape ) );
					head->right->left = head;
					move_head_right( head );
					if( isdigit( c ) ){
						head->symbol = NUMBER;
						state = N_BEFORE;
					}
					else if( c == '-' ){
						head->symbol = NUMBER;
						state = N_MINUS;
					}
					else if( c == '\"' ){
						head->symbol = STRING;
						state = S_TEXT;
					}
					else state = TRAP;
				}
				else if( c == '\n' && (state == N_BEFORE || state == N_AFTER || state == S_FINAL ) ){
				// True if newline is encountered at the end of a valid field
					if( head_read( head ) == NUMBER ) head_write( head, NUMBER_STOP );
					else if( head_read( head ) == STRING ) head_write( head, STRING_STOP );
					state = FINAL;
				}
				else if( c == ',' && (state == N_BEFORE || state == N_AFTER || state == S_FINAL ) ){
				// True if comma is encountered at the end of a valid field
					state = MASTER;
				}
				else if( state == N_AFTER ){
				// True if in a number field after the decimal point but not at the end
					if( !isdigit( c ) ) state = TRAP;
				}
				else if( state == N_POINT ){
				// True if just after the decimal point in a number field
					if( isdigit( c ) ) state = N_AFTER;
					else state = TRAP;
				}
				else if( state == N_BEFORE ){
				// True if inside a number field where neither the decimal point nor the end of the field has been encountered
					if( c == '.' ) state = N_POINT;
					else if( !isdigit( c ) ) state = TRAP;
				}
				else if( state == N_MINUS ){
				// True if at the beginning of a number field with a negative value
					if( isdigit( c ) ) state = N_BEFORE;
					else state = TRAP;
				}
				else if( state == S_FINAL ) state = TRAP;
				// Any character other than comma or newline after the end quote is invalid
				else if( state == S_TEXT ){
					if( c == '\"' ) state = S_FINAL;
					else if( c == '\n' ) state = TRAP;
				}
			}
		}
		else if( state == END_HDR && head_read( head ) == STOP ){
		// True if end of header has just been reached
			while( head_read( head ) != START1 ){
				move_head_left( head );
			}
			head_write( head, START2 );
			move_head_right( head );
			if( isdigit( c ) || c == '-' ){
				if( head_read( head ) == BLANK )
					head_write( head, NUMBER );
				else if( head_read( head ) == STOP )
					head_write( head, NUMBER_STOP );
				if( isdigit( c ) ) state = N_BEFORE;
				else state = N_MINUS;
			}
			else if( c == '\"' ){
				if( head_read( head ) == BLANK )
					head_write( head, STRING );
				else if( head_read( head ) == STOP )
					head_write( head, STRING_STOP );
				state = S_TEXT;
			}
			else state = TRAP;
		}
		else if( state == MASTER && (head_read( head ) == BLANK || head_read( head ) == STOP) ){
		// True if entering a new field in the first non-header record in a CSV file with a header
			if( isdigit( c ) || c == '-' ){
				if( head_read( head ) == BLANK )
					head_write( head, NUMBER );
				else if( head_read( head ) == STOP )
					head_write( head, NUMBER_STOP );
				if( isdigit( c ) ) state = N_BEFORE;
				else state = N_MINUS;
			}
			else if( c == '\"' ){
				if( head_read( head ) == BLANK )
					head_write( head, STRING );
				else if( head_read( head ) == STOP )
					head_write( head, STRING_STOP );
				state = S_TEXT;
			}
			else state = TRAP;
		}
		else if( c == ',' && (state == N_BEFORE || state == N_AFTER || state == S_FINAL) ){
		// True if at the end of a valid field other than the last one in any non-header record
			if( head_read( head ) == STOP || head_read( head ) == NUMBER_STOP || head_read( head ) == STRING_STOP ){
			// True if the record has too many fields
				state = TRAP;
			}
			else{
				move_head_right( head );
				state = MASTER;
			}
		}
		else if( c == '\n' && (state == N_BEFORE || state == N_AFTER || state == S_FINAL) ){
		// True if at the end of a valid last field in any non-header record
			if( head_read( head ) == NUMBER_STOP || head_read( head ) == STRING_STOP )
				state = FINAL;
			else state = TRAP;
		}
		else if( state == N_AFTER ){
		// True if in a number field after the decimal point but not at the end
			if( !isdigit( c ) ) state = TRAP;
		}
		else if( state == N_POINT ){
		// True if just after the decimal point in a number field
			if( isdigit( c ) ) state = N_AFTER;
			else state = TRAP;
		}
		else if( state == N_BEFORE ){
		// True if inside a number field where neither the decimal point nor the end of the field has been encountered
			if( c == '.' ) state = N_POINT;
			else if( !isdigit( c ) ) state = TRAP;
		}
		else if( state == N_MINUS ){
			if( isdigit( c ) ) state = N_BEFORE;
			else state = TRAP;
		}
		else if( state == S_FINAL ) state = TRAP;
		// Any character other than comma or newline after the end quote is invalid
		else if( state == S_TEXT ){
		// True if inside quotes
			if( c == '\"' ) state = S_FINAL;
			else if( c == '\n' ) state = TRAP;
		}
		else if( state == MASTER ){
		// True if entering any field in any record other than the header or the first non-header record
			if( head_read( head ) == NUMBER || head_read( head ) == NUMBER_STOP ){
				if( isdigit( c ) ) state = N_BEFORE;
				else if( c == '-' ) state = N_MINUS;
				else state = TRAP;
			}
			else if( head_read( head ) == STRING || head_read( head ) == STRING_STOP ){
				if( c == '\"' ) state = S_TEXT;
				else state = TRAP;
			}
		}
		else if( state == FINAL ){
		// True if encountering another character after the end of a valid non-header record has been reached
			while( head_read( head ) != START2 ){
				move_head_left( head );
			}
			move_head_right( head );
			if( head_read( head ) == NUMBER || head_read( head ) == NUMBER_STOP ){
				if( isdigit( c ) ) state = N_BEFORE;
				else if( c == '-' ) state = N_MINUS;
				else state = TRAP;
			}
			else if( head_read( head ) == STRING || head_read( head ) == STRING_STOP ){
				if( c == '\"' ) state = S_TEXT;
				else state = TRAP;
			}
		}
		else{
			fprintf( stderr, "Error: Turing machine fall-though.\nIf you are seeing this message, it means that there is a possible set\nof Turing machine parameters that the programmer failed to account for.\nPlease notify Michael Warren a.k.a. Psycho Cod3r using the email address\nlisted on his GitHub.\n" );
			exit( -1 );
		}
#ifdef _DEBUG
		printf( "Next state: %s\n", state_strings[state] );
		printf( "Next tape symbol: %s\n", symbol_strings[head->symbol] );
		putchar( '\n' );
		if( state == FINAL ){
			puts( "--------------------------------------------------------------------------------" );
		}
		else if( state == MASTER ){
			puts( "----------------------------------------" );
		}
		else{
			puts( "--------------------" );
		}
#endif
	}

	// Section to account for non-newline-terminated last line:
	if( (state == N_BEFORE || state == N_AFTER || state == S_FINAL) && (head_read( head ) == NUMBER_STOP || head_read( head ) == STRING_STOP) ){
		state = FINAL;
	}

	// Free tape structs:
	while( head_read( head ) != START1 && head_read( head ) != START2 ){
		move_head_left( head );
	}
	while( head->right ){
		move_head_right( head );
		free( head->left );
	}
	free( head );

	fseek( fp, pos, SEEK_SET );
	return (state == FINAL);
}

// Reads a CSV table from a file
csv_table *csv_read_table( FILE *fp, bool has_header ){
	long pos;
	dfloat64_t *tmpf;
	csv_table *table;
	int i, f, c, x, state, len, size;
	char *buf = NULL;
	char *ptr = NULL;
	pos = ftell( fp );
	rewind( fp );
	table = (csv_table *) malloc( sizeof( csv_table ) );

	// Field-counting automaton:
	table->rlen = 1;
	state = OUT;
	read_line( buf, size, fp );
	len = strlen( buf );
	for( i = 0; i < len; i++ ){
		if( state == OUT ){
			if( buf[i] == '\"' ) state = IN;
			else if( buf[i] == ',' ) (table->rlen)++;
		}
		else if( state == IN && buf[i] == '\"' ) state = OUT;
	}

	table->header = (csv_field **) calloc( table->rlen, sizeof( csv_field * ) );
	for( f = 0; f < table->rlen; f++ ){
		table->header[f] = (csv_field *) malloc( sizeof( csv_field ) );
	}

	// Determine field names:
	if( has_header ){
		// Isolate field strings:
		state = OUT;
		for( i = 0; i < len; i++ ){
			if( state == OUT ){
				if( buf[i] == '\"' ) state = IN;
				else if( buf[i] == ',' ) buf[i] = '\0';
			}
			else if( state == IN && buf[i] == '\"' ) state = OUT;
			if( buf[i] == '\"' || buf[i] == '\r' || buf[i] == '\n' ) buf[i] = '\0';
		}
		// Read field strings:
		ptr = buf + 1;
		for( f = 0; f < table->rlen; f++ ){
			len = strlen( ptr );
			table->header[f]->name = (char *) malloc( len + 1 );
			strncpy( table->header[f]->name, ptr, len + 1 );
			ptr += len;
			while( ptr[0] == '\0' ) ptr++;
		}
	}
	else{
		for( f = 0; f < table->rlen; f++ ){
			// Format for field names will be "x<number>"
			x = ceil( log( (double) i ) / log( 10.0 ) ) + 2;
			table->header[f]->name = (char *) malloc( x );
			snprintf( table->header[f]->name, x, "x%d\0", f );
		}
	}

	// Automaton to determine types of fields:
	if( !has_header ) rewind( fp );
	state = START;
	read_line( buf, size, fp );
	len = strlen( buf );
	f = 0;
	for( i = 0; i < len; i++ ){
		if( state == START ){
			if( buf[i] == '\"' ){
				state = IN;
				table->header[f]->type = csv_string;
			}
			else{
				state = OUT;
				table->header[f]->type = csv_number;
			}
			f++;
		}
		if( state == IN ){
			if( buf[i] == '\"' )
				state = OUT;
		}
		if( state == OUT ){
			if( buf[i] == ',' )
				state = START;	
		}
	}

	// Code to build the table structure:
	rewind( fp );
	table->start = (csv_record *) calloc( 1, sizeof( csv_record ) );
	table->cur = table->start;
	if( has_header ) read_line( buf, size, fp );
	while( (c = fgetc( fp )) != EOF ){
		table->cur->next = (csv_record *) calloc( 1, sizeof( csv_record ) );
		table->cur = table->cur->next;
		table->cur->record = (void **) calloc( table->rlen, sizeof( void * ) );
		ungetc( c, fp );
		read_line( buf, size, fp );
		len = strlen( buf );
		// Isolate field strings:
		state = OUT;
		for( i = 0; i < len; i++ ){
			if( state == OUT ){
				if( buf[i] == '\"' ) state = IN;
				else if( buf[i] == ',' ) buf[i] = '\0';
			}
			else if( state == IN && buf[i] == '\"' ) state = OUT;
			if( buf[i] == '\"' || buf[i] == '\r' || buf[i] == '\n' ) buf[i] = '\0';
		}
		// Read field strings:
		ptr = buf;
		while( ptr[0] == '\0' ) ptr++;
		for( f = 0; f < table->rlen; f++ ){
			len = strlen( ptr );
			if( table->header[f]->type == csv_string ){
				table->cur->record[f] = malloc( len + 1 );
				strncpy( table->cur->record[f], ptr, len + 1 );
			}
			else if( table->header[f]->type == csv_number ){
				table->cur->record[f] = malloc( sizeof( dfloat64_t ) );
				tmpf = dfloat64_atof( ptr );
				memcpy( table->cur->record[f], tmpf, sizeof( dfloat64_t ) );
			}
			ptr += len;
			while( ptr[0] == '\0' ) ptr++;
		}
	}
	table->cur = table->start;

	fseek( fp, pos, SEEK_SET );
	return table;
}

// Writes a CSV table to a file, starting at the current file
// position, can be used multiple times with different tables
// to concatenate them into one file
void csv_write_table( FILE *fp, csv_table *table, bool has_header ){
	csv_record *saved;
	dfloat64_t *tmpf;
	char *tmpstr;
	char *fstr;
	int f;
	int i;
	int len;
	tmpf = (dfloat64_t *) malloc( sizeof( dfloat64_t ) );
	if( has_header ){
		f = 0;
		while( f < table->rlen ){
			fputc( '"', fp );
			fputs( table->header[f]->name, fp );
			fputc( '"', fp );
			if( (++f) == table->rlen )
				fputs( _EOL_, fp );
			else
				fputc( ',', fp );
		}
	}
	saved = table->cur;
	table->cur = table->start;
	while( table->cur->next ){
		table->cur = table->cur->next;
		f = 0;
		while( f < table->rlen ){
			if( table->header[f]->type == csv_string ){
				len = strlen( (char *) table->cur->record[f] );
				tmpstr = (char *) malloc( len + 1 );
				strncpy( tmpstr, table->cur->record[f], len + 1 );
				for( i = 0; i < len; i++ ){
					if( tmpstr[i] == '"' || tmpstr[i] == '\n' ){
						fprintf( stderr, "String contains invalid characters.\n" );
						return;
					}
				}
				fputc( '"', fp );
				fputs( (char *) tmpstr, fp );
				fputc( '"', fp );
			}
			else if( table->header[f]->type == csv_number ){
				memcpy( tmpf, table->cur->record[f], sizeof( dfloat64_t ) );
				fstr = dfloat64_ftoa( tmpf );
				fputs( fstr, fp );
				free( fstr ); // fstr was malloc'ed by dfloat64_ftoa()
			}
			if( (++f) == table->rlen )
				fputs( _EOL_, fp );
			else
				fputc( ',', fp );
		}
	}
}

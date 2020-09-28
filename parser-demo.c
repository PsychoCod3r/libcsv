/**********************************************
 * CSV Parser Demo, Version 0.1 Alpha         *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date September 2020                        *
 * License: Michael Warren FSL                *
 **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

enum types { number, string };

// Metadata field for the table header
struct csv_field {
	char *name;
	enum types type;
};

// Single record in linked list table structure
struct csv_record {
	void **record;
	struct csv_record *next;
};

// Handle for abstract table structure
struct csv_table {
	int rlen;                  // # of fields in each record
	struct csv_field **header; // Table metadata
	struct csv_record *start;  // Pointer to first record
	struct csv_record *cur;    // Pointer to current record
};

int usage( char * );
bool validate( FILE *, bool );
struct csv_table *read_table( FILE *, bool );

bool debug =
#ifdef _DEBUG
	true;
#else
	false;
#endif

// Tape alphabet for Turing machine
enum symbols { STRING, NUMBER, BLANK, START1, START2, STOP, STRING_STOP, NUMBER_STOP };
// STRING and NUMBER indicate String and Number fields
// STRING_STOP and NUMBER_STOP indicate last field in a record
// BLANK is a temporary placeholder symbol written when parsing the header
// START1 and START2 are written to the left end of the tape
// START1 indicates that the first non-header record has not yet been read
// STOP is written to the unit corresponding to the last field when parsing the header

char *symbol_strings[] =
#ifdef _DEBUG
	{ "STRING", "NUMBER", "BLANK", "START1", "START2", "STOP", "STRING_STOP", "NUMBER_STOP" };
#else
	{ NULL };
#endif

// Unit for tape used by Turing machine
struct tape {
	enum symbols symbol;
	struct tape *right;
	struct tape *left;
};

// States for Turing machine
#define MASTER   0 // Start state for whole machine, also entered from final states of Number and String DFAs when a comma is encountered
#define FINAL    1 // Final state for whole machine, entered from final states of Number and String DFAs when end of line is reached
#define TRAP     2 // Trap state, entered when an invalid character is encountered
#define END_HDR  3 // State for when the end of the optional header is reached
#define N_MINUS  4 // Accounts for optional minus sign at the beginning of a number
#define N_BEFORE 5 // Final state for Number, entered from MASTER state when digit is encountered and Number field is indicated
#define N_POINT  6 // Entered when decimal point is encountered
#define N_AFTER  7 // Final state for Number, entered from N_POINT when a digit is encountered
#define S_TEXT   8 // Nonfinal state for string, entered from MASTER state when start quote is encountered and String field is indicated
#define S_FINAL  9 // Final state for String, entered when end quote is encountered

char *state_strings[] =
#ifdef _DEBUG
	{ "MASTER", "FINAL", "TRAP", "END_HDR", "N_MINUS", "N_BEFORE", "N_POINT", "N_AFTER", "S_TEXT", "S_FINAL" };
#else
	{ NULL };
#endif

// Turing machine actions
#define move_head_left( head ) head = head->left
#define move_head_right( head ) head = head->right
#define head_read( head ) head->symbol
#define head_write( head, s ) head->symbol = s

// States for field counter
#define START 0
#define IN    1
#define OUT   2

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

int main( int argc, char **argv ){
	FILE *fp;
	char *csv_file = NULL;
	char *buf = NULL;
	size_t size;
	bool has_header = true;
	int i, r;
	double tmpf;
	char *tmps;
	struct csv_table *table;

	for( i = 1; i < argc; i++ ){
		if( argv[i][0] == '-' ){
			if( !strcmp( argv[i], "--noheader" ) ){
				has_header = false;
			}
		}
		else{
			csv_file = argv[i];
		}
	}
	if( !csv_file ){
		exit( usage( argv[0] ) );
	}
	if( !(fp = fopen( csv_file, "r" )) ){
		fprintf( stderr, "%s: %s: %s\n", argv[0], csv_file, strerror( errno ) );
		exit( errno );
	}

	if( !validate( fp, has_header ) ){
		fprintf( stderr, "Invalid CSV code.\n" );
		exit( -1 );
	}
	printf( "CSV code is valid.\n" );

	putchar( '\n' );

	table = read_table( fp, has_header );
	printf( "%d fields:\n", table->rlen );
	for( i = 0; i < table->rlen; i++ ){
		printf( "%s: ", table->header[i]->name );
		printf( "%s", (table->header[i]->type == number)?"number":"string" );
		putchar( '\n' );
	}
	putchar( '\n' );
	r = 1;
	while( table->cur->next ){
		table->cur = table->cur->next;
		printf( "Record %d:\n", r );
		for( i = 0; i < table->rlen; i++ ){
			printf( "%s: ", table->header[i]->name );
			if( table->header[i]->type == number ){
				memcpy( &tmpf, table->cur->record[i], sizeof( double ) );
				printf( "%f\n", tmpf );
			}
			else if( table->header[i]->type == string ){
				tmps = table->cur->record[i];
				printf( "%s\n", tmps );
			}
		}
		putchar( '\n' );
		r++;
	}

	fclose( fp );
	return 0;
}

int usage( char *executable ){
	printf( "Usage: %s filename.csv [--noheader]\n", executable );
	return errno;
}

struct csv_table *read_table( FILE *fp, bool has_header ){
	long pos;
	double tmpf;
	struct csv_table *table;
	int i, f, c, x, state, len, size;
	char *buf = NULL;
	char *ptr = NULL;
	pos = ftell( fp );
	rewind( fp );
	table = (struct csv_table *) malloc( sizeof( struct csv_table ) );

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

	table->header = (struct csv_field **) calloc( table->rlen, sizeof( struct csv_field * ) );
	for( f = 0; f < table->rlen; f++ ){
		table->header[f] = (struct csv_field *) malloc( sizeof( struct csv_field ) );
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
				table->header[f]->type = string;
			}
			else{
				state = OUT;
				table->header[f]->type = number;
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
	table->start = (struct csv_record *) calloc( 1, sizeof( struct csv_record ) );
	table->cur = table->start;
	if( has_header ) read_line( buf, size, fp );
	while( (c = fgetc( fp )) != EOF ){
		table->cur->next = (struct csv_record *) calloc( 1, sizeof( struct csv_record ) );
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
			if( table->header[f]->type == string ){
				table->cur->record[f] = malloc( len + 1 );
				strncpy( table->cur->record[f], ptr, len + 1 );
			}
			else if( table->header[f]->type == number ){
				table->cur->record[f] = malloc( sizeof( double ) );
				tmpf = atof( ptr );
				memcpy( table->cur->record[f], &tmpf, sizeof( double ) );
			}
			ptr += len;
			while( ptr[0] == '\0' ) ptr++;
		}
	}
	table->cur = table->start;

	fseek( fp, pos, SEEK_SET );
	return table;
}

bool validate( FILE *fp, bool has_header ){
	long pos;
	int c;
	int state;
	struct tape *head;
	pos = ftell( fp );
	rewind( fp );

	// Turing machine setup:
	state = MASTER;
	head = (struct tape *) calloc( 1, sizeof( struct tape ) );
	head->symbol = START1;

	// Turing machine main loop:
	if( debug ) puts("-------------------------------------------------------------------------------" );
	while( (c = fgetc( fp )) != EOF ){
		if( debug ){
			printf( "Current state: %s\n", state_strings[state] );
			printf( "Input symbol: %c,0x%s%x\n", c, (c<0x10)?"0":"", c );
			printf( "Current tape symbol: %s\n", symbol_strings[head->symbol] );
		}
		if( state == TRAP ); // Do nothing
		else if( c == '\r' ); // Ignore carriage returns
		else if( head->right == NULL && head->symbol != STOP && head->symbol != STRING_STOP && head->symbol != NUMBER_STOP ){
		// True if reading the first record
			if( has_header ){
			// True if reading the header
				if( state == MASTER ){
					if( c == '\"' ){
						head->right = (struct tape *) calloc( 1, sizeof( struct tape ) );
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
					head->right = (struct tape *) calloc( 1, sizeof( struct tape ) );
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
		// MORE CODE HERE
		if( debug ){
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
		}
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

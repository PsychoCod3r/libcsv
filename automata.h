/**********************************************
 * libcsv, Version 0.2.2 Alpha                *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: September 2020                       *
 * License: Michael Warren FSL Version 1.1    *
 * Current module: Header file for automata   *
 *                 functionality              *
 **********************************************/

#ifndef _AUTOMATA_
#define _AUTOMATA_

// Tape alphabet for Turing machine
enum symbols { STRING, NUMBER, BLANK, START1, START2, STOP, STRING_STOP, NUMBER_STOP };
// STRING and NUMBER indicate String and Number fields
// STRING_STOP and NUMBER_STOP indicate last field in a record
// BLANK is a temporary placeholder symbol written when parsing the header
// START1 and START2 are written to the left end of the tape
// START1 indicates that the first non-header record has not yet been read
// STOP is written to the unit corresponding to the last field when parsing the header

#ifdef _DEBUG
char *symbol_strings[] = { "STRING", "NUMBER", "BLANK", "START1", "START2", "STOP", "STRING_STOP", "NUMBER_STOP" };
#endif

// Unit for tape used by Turing machine
struct _tm_tape {
	enum symbols symbol;
	struct _tm_tape *right;
	struct _tm_tape *left;
};

typedef struct _tm_tape tm_tape;
typedef tm_tape tm_head;

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

#ifdef _DEBUG
char *state_strings[] = { "MASTER", "FINAL", "TRAP", "END_HDR", "N_MINUS", "N_BEFORE", "N_POINT", "N_AFTER", "S_TEXT", "S_FINAL" };
#endif

// Turing machine actions
#define move_head_left( head ) head = head->left
#define move_head_right( head ) head = head->right
#define head_read( head ) head->symbol
#define head_write( head, s ) head->symbol = s

// States for transducers in csv_read_table
#define START 0
#define IN    1
#define OUT   2

#endif

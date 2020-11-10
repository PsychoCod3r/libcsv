/**********************************************
 * libcsv, Version 0.3 Alpha                  *
 * Description: CSV library for C             *
 * Author: Michael Warren, a.k.a Psycho Cod3r *
 * Date: November 2020                        *
 * License: Michael Warren FSL Version 1.1    *
 * Current module: Functions for operating on *
 *                 CSV sets                   *
 **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "csv.h"

// Returns the empty set with the given size
csv_set *csv_empty_set( int size ){
	csv_set *empty_set;
	int i;
	empty_set = (csv_set *) malloc( sizeof( csv_set ) );
	empty_set->size = size / 8 + (size % 8 ? 1 : 0);
	// Don't add 1 byte if an exact multiple of 8
	empty_set->bits = (uint8_t *) malloc( size );
	for( i = 0; i < empty_set->size; i++ )
		empty_set->bits[i] = 0x00;
	return empty_set;
}

// Returns the universe with the given size
csv_set *csv_set_universe( int size ){
	csv_set *universe;
	int i;
	universe = (csv_set *) malloc( sizeof( csv_set ) );
	universe->size = size / 8 + (size % 8 ? 1 : 0);
	// Don't add 1 byte if an exact multiple of 8
	universe->bits = (uint8_t *) malloc( size );
	for( i = 0; i < universe->size; i++ )
		universe->bits[i] = 0xff;
	return universe;
}

// Adds a member to a set
void csv_set_add( csv_set *set, int member ){
	int major_index, minor_index;
	major_index = member / 8;
	minor_index = member % 8;
	set->bits[major_index] |= (1 << minor_index);
}

// Deletes a member from a set
void csv_set_del( csv_set *set, int member ){
	int major_index, minor_index;
	major_index = member / 8;
	minor_index = member % 8;
	set->bits[major_index] &= ~(1 << minor_index);
}

// Determines whether an element is a member of a set
bool csv_set_member( int element, csv_set *set ){
	int major_index, minor_index;
	major_index = element / 8;
	minor_index = element % 8;
	return set->bits[major_index] & (1 << minor_index);
}

/* 
 * Note: For any binary functions that follow, operands
 * must be the same size. Using two sets of different
 * sizes results in undefined behavior.
 */

// Calculates the set difference between src and dst
// and stores the result in dst
void csv_set_difference( csv_set *dst, csv_set *src ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] &= ~(src->bits[i]);
}

// Calculates the set complement of dst and stores it
// in dst
void csv_set_complement( csv_set *dst ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] = ~(dst->bits[i]);
}

// Calculates the set union of src and dst and stores
// the result in dst
void csv_set_union( csv_set *dst, csv_set *src ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] |= src->bits[i];
}

// Calculates the set intersection of src and dst and
// stores the result in dst
void csv_set_intersection( csv_set *dst, csv_set *src ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] &= src->bits[i];
}

/*
 * The following functions are meant to make the building
 * of more complex instructions easier. Do not use them on
 * persistent set variables.
 */

// Returns the set difference between the two operands,
// freeing the second operand in the process
csv_set *csv_set_difference_f( csv_set *dst, csv_set *src ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] &= ~(src->bits[i]);
	free( src );	
	return dst;
}

// Set complement function that returns the result
csv_set *csv_set_complement_f( csv_set *dst ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] = ~(dst->bits[i]);
	return dst;
}

// Returns the set union of the two operands,
// freeing the second operand in the process
csv_set *csv_set_union_f( csv_set *dst, csv_set *src ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] |= src->bits[i];
	free( src );
	return dst;
}

// Calculates the set intersection of the two operands,
// freeing the second operand in the process
csv_set *csv_set_intersection_f( csv_set *dst, csv_set *src ){
	int i;
	for( i = 0; i < dst->size; i++ )
		dst->bits[i] &= src->bits[i];
	free( src );
	return dst;
}

// Reads a set in hexadecimal from a string
csv_set *csv_read_set( char *src ){
	csv_set *dst;
	int len;
	int i;
	char buf[3];
	buf[2] = '\0'; // Makes sure sscanf() doesn't run off the end
	len = strlen( src );
	dst = (csv_set *) malloc( sizeof( csv_set ) );
	dst->size = len / 2;
	dst->bits = (uint8_t *) malloc( len / 2 );
	for( i = 0; i < len; i += 2 ){
		strncpy( buf, src+i, 2 );
		dst->bits[(len-i)/2-1] = (uint8_t) strtol( buf, NULL, 16 );
	}
	return dst;
}

// Writes a set in hexadecimal to a string
char *csv_write_set( csv_set *src ){
	char *dst;
	int i;
	int len;
	int hex;
	len = src->size * 2;
	dst = (char *) calloc( len + 1, 1 );
	for( i = 0; i < len; i += 2 ){
		hex = src->bits[(len-i)/2-1];
		snprintf( dst+i, 3, "%s%x", (hex<0x10)?"0":"", hex );
	}
	return dst;
}

// Free version of csv_write_set_f in case
// src is an immediate operand
char *csv_write_set_f( csv_set *src ){
	char *dst;
	dst = csv_write_set( src );
	free( src );
	return dst;
}

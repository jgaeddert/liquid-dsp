//
// liquid.internal.h
//
// Internal header file for liquid DSP for SDR
//
// This file includes function declarations which are intended
// for internal use
//

#ifndef __LIQUID_INTERNAL_H__
#define __LIQUID_INTERNAL_H__

// Configuration file
#include "config.h"

#include <complex.h>
#include "liquid.h"

//
// Random
//
float complex icrandnf();

//
// Miscellaneous utilities
//

// number of 1's in byte
extern unsigned int c_ones[256];

// Count the number of ones in an integer
unsigned int count_ones_static(unsigned int _x); 

// Count the number of ones in an integer, inline insertion
#define count_ones_inline_uint2(x) (    \
    c_ones[  (x)      & 0xFF ] +        \
    c_ones[ ((x)>>8)  & 0xFF ])

#define count_ones_inline_uint4(x) (    \
    c_ones[  (x)      & 0xFF ] +        \
    c_ones[ ((x)>> 8) & 0xFF ] +        \
    c_ones[ ((x)>>16) & 0xFF ] +        \
    c_ones[ ((x)>>24) & 0xFF ])

#if SIZEOF_INT == 2
#  define count_ones(x) count_ones_inline_uint2(x)
#elif SIZEOF_INT == 4
#  define count_ones(x) count_ones_inline_uint4(x)
#endif

//#define count_ones(x) count_ones_static(x)


// number of leading zeros in byte
extern unsigned int leading_zeros[256];

// Count leading zeros in an integer
unsigned int count_leading_zeros(unsigned int _x); 

// Most-significant bit index
unsigned int msb_index(unsigned int _x);

#endif // __LIQUID_INTERNAL_H__

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
// Debugging macros
//
#define DEBUG_PRINTF_FLOAT(F,STR,I,V)       \
    fprintf(F,"%s(%4u) = %12.4e;\n",STR,I+1,V)
#define DEBUG_PRINTF_CFLOAT(F,STR,I,V)      \
    fprintf(F,"%s(%4u) = %12.4e +j*%12.4e;\n",STR,I+1,crealf(V),cimagf(V))

//
// MODULE: buffer
//

// Buffers

#define buffer_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_BUFFER_DEFINE_INTERNAL_API(X,T)          \
void X(_linearize)(X() _b);                             \
void X(_c_read)(X() _b, T ** _v, unsigned int *_n);     \
void X(_s_read)(X() _b, T ** _v, unsigned int *_n);     \
void X(_c_write)(X() _b, T * _v, unsigned int _n);      \
void X(_s_write)(X() _b, T * _v, unsigned int _n);      \
void X(_c_release)(X() _b, unsigned int _n);            \
void X(_s_release)(X() _b, unsigned int _n);            \
void X(_c_push)(X() _b, T _v);                          \
void X(_s_push)(X() _b, T _v);

LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_FLOAT, float)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_CFLOAT, float complex)
LIQUID_BUFFER_DEFINE_INTERNAL_API(BUFFER_MANGLE_UINT, unsigned int)

// Windows

#define window_fast_access(c,i) (c->v[(c->read_index+i)%(c->len)])

#define LIQUID_WINDOW_DEFINE_INTERNAL_API(X,T)          \
void X(_linearize)(X() _b);

LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_FLOAT, float)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_CFLOAT, float complex)
LIQUID_WINDOW_DEFINE_INTERNAL_API(WINDOW_MANGLE_UINT, unsigned int)




//
// MODULE: random
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

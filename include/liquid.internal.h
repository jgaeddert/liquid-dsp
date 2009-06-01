/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

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
// MODULE : matrix
//

#define MATRIX_MANGLE_FLOAT(name)   LIQUID_CONCAT(fmatrix, name)
#define MATRIX_MANGLE_CFLOAT(name)  LIQUID_CONCAT(cfmatrix, name)

// large macro
//   X: name-mangling macro
//   T: data type
#define LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX,T)         \
void MATRIX(_gjelim)(T * _X,                                \
                unsigned int _XR,                           \
                unsigned int _XC);                          \
void MATRIX(_pivot)(T * _X,                                 \
               unsigned int _XR,                            \
               unsigned int _XC,                            \
               unsigned int _r,                             \
               unsigned int _c);                            \
void MATRIX(_swaprows)(T * _X,                              \
                  unsigned int _XR,                         \
                  unsigned int _XC,                         \
                  unsigned int _r1,                         \
                  unsigned int _r2);

LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_FLOAT, float)
LIQUID_MATRIX_DEFINE_INTERNAL_API(MATRIX_MANGLE_CFLOAT, liquid_float_complex)


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

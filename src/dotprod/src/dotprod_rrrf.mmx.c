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
 * MERCHANTABILITY or FITNESS FOR A PARfloatCULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

// 
// Floating-point dot product (MMX)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>

#include "liquid.internal.h"

#define DEBUG_DOTPROD_RRRF_MMX   0

// basic dot product (ordinal calculation)
void dotprod_rrrf_run(float *_h,
                      float *_x,
                      unsigned int _n,
                      float * _y)
{
    float r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    *_y = r;
}

// basic dot product (ordinal calculation) with loop unrolled
void dotprod_rrrf_run4(float *_h,
                       float *_x,
                       unsigned int _n,
                       float * _y)
{
    float r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _h[i]   * _x[i];
        r += _h[i+1] * _x[i+1];
        r += _h[i+2] * _x[i+2];
        r += _h[i+3] * _x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _h[i] * _x[i];

    *_y = r;
}


//
// structured MMX dot product
//

struct dotprod_rrrf_s {
    unsigned int n;     // length
    float * h_mem;      // allocated memory
    float * h;          // aligned coefficients array
};

dotprod_rrrf dotprod_rrrf_create(float * _h,
                                 unsigned int _n)
{
    dotprod_rrrf q = (dotprod_rrrf)malloc(sizeof(struct dotprod_rrrf_s));
    q->n = _n;

    // allocate memory for coefficients
    q->h_mem = (float*) malloc( (q->n + 4)*sizeof(float) );

    // find alignment and set internal pointer
    unsigned int i;
    for (i=0; i<4; i++) {
        int align = ((long int)(&q->h_mem[i]) & 15)/sizeof(float);

        if (align == 0)
            q->h = &q->h_mem[i];
    }

    // assert( h is 16-byte aligned )

    // set coefficients
    memmove(q->h, _h, _n*sizeof(float));

    return q;
}

// re-create the structured dotprod object
dotprod_rrrf dotprod_rrrf_recreate(dotprod_rrrf _dp,
                                   float * _h,
                                   unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_rrrf_destroy(_dp);
    _dp = dotprod_rrrf_create(_h,_n);
    return _dp;
}


void dotprod_rrrf_destroy(dotprod_rrrf _q)
{
    free(_q->h_mem);
    free(_q);
}

void dotprod_rrrf_print(dotprod_rrrf _q)
{
    printf("dotprod_rrrf:\n");
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.9f\n", i, _q->h[i]);
}

void dotprod_rrrf_execute(dotprod_rrrf _q,
                          float * _x,
                          float * _y)
{
    // alignment
    int al = ((long int)_x & 15)/sizeof(float);
    //printf("align : %d\n", al);

    // first cut: ...
    __m128 v;
    __m128 h;
    union { __m128 v; float w[4];} sum;
    float total = 0.0f;

    //
    unsigned int i;
    for (i=0; i<_q->n; i+=4) {
        // load inputs into register (unaligned)
        v = _mm_loadu_ps(&_x[i]);

        // load coefficients into register (aligned)
        h = _mm_load_ps(&_q->h[i]);

        // compute multiplication
        sum.v = _mm_mul_ps(v, h);

        // add into total (not necessarily efficient...)
        total += sum.w[0] + sum.w[1] + sum.w[2] + sum.w[3];
    }

    // clean up remaining elements
    for (; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
}

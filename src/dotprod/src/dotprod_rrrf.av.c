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
// Floating-point dot product (altivec velocity engine)
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define DEBUG_DOTPROD_RRRF_AV   0

// basic dot product

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
// structured dot product
//

struct dotprod_rrrf_s {
    unsigned int n; // length
    float *h[4];    // hold 4 copies on altivec machines
};

dotprod_rrrf dotprod_rrrf_create(float * _h, unsigned int _n)
{
    dotprod_rrrf dp = (dotprod_rrrf)malloc(sizeof(struct dotprod_rrrf_s));
    dp->n = _n;

    unsigned int i,j;
    for (i=0; i<4; i++) {
        dp->h[i] = calloc(1+(dp->n+i-1)/4,sizeof(vector float));
        for (j=0; j<dp->n; j++)
            dp->h[i][j+i] = _h[j];
    }

    return dp;
}

void dotprod_rrrf_destroy(dotprod_rrrf _dp)
{
    unsigned int i;
    for (i=0; i<4; i++)
        free(_dp->h[i]);
    free(_dp);
}

void dotprod_rrrf_print(dotprod_rrrf _dp)
{
    printf("dotprod_rrrf:\n");
    unsigned int i;
    for (i=0; i<_dp->n; i++)
        printf("%3u : %12.9f\n", i, _dp->h[0][i]);
}

void dotprod_rrrf_execute(dotprod_rrrf _dp,
                          float * _x,
                          float * _r)
{
    int al; // alignment

    vector float *ar,*d;
    vector float s0,s1,s2,s3;
    union { vector float v; float w[4];} s;
    unsigned int nblocks;

    ar = (vector float*)( (int)_x & ~15);
    al = ((int)_x & 15)/sizeof(float);

    d = (vector float*)_dp->h[al];

    // print values
#if DEBUG_DOTPROD_RRRF_AV
    vector float h;
    vector float x;
    unsigned int i;
    for (i=0; i<_dp->n; i+=4) {
        h = d[i/4];
        printf("  h : %12.8vf\n", h);
    }
    for (i=0; i<_dp->n; i+=4) {
        x = ar[i/4];
        printf("  x : %12.8vf\n", x);
    }
#endif

    nblocks = (_dp->n + al - 1)/4 + 1;
#if DEBUG_DOTPROD_RRRF_AV
    printf("nblocks : %u\n", nblocks);
#endif

    s0 = s1 = s2 = s3 = (vector float)(0);
    while (nblocks >= 4) {
        s0 = vec_madd(ar[nblocks-1],d[nblocks-1],s0);
        s1 = vec_madd(ar[nblocks-2],d[nblocks-2],s1);
        s2 = vec_madd(ar[nblocks-3],d[nblocks-3],s2);
        s3 = vec_madd(ar[nblocks-4],d[nblocks-4],s3);
        //printf("block\n");
        nblocks -= 4;
    }

#if DEBUG_DOTPROD_RRRF_AV
    printf("\n");
    printf("s0 : %12.8vf\n", s0);
    printf("s1 : %12.8vf\n", s1);
    printf("s2 : %12.8vf\n", s2);
    printf("s3 : %12.8vf\n", s3);
    printf("\n");
#endif

    s0 = vec_add(s0,s1,s0);
    s2 = vec_add(s2,s3,s2);
    s0 = vec_add(s0,s2,s0);

#if DEBUG_DOTPROD_RRRF_AV
    printf("s0 : %12.8vf\n", s0);
#endif

    while (nblocks-- > 0) {
        s0 = vec_madd(ar[nblocks],d[nblocks],s0);
        //printf("*s : %12.8vf\n", s0);
    }
#if DEBUG_DOTPROD_RRRF_AV
    printf("s0 : %12.8vf\n", s0);
#endif

    s.v = vec_add(s0,(vector float)(0));
#if DEBUG_DOTPROD_RRRF_AV
    printf("v  : %12.8vf\n", s.v);
    printf("w0 : %12.8f\n", s.w[0]);
    printf("w1 : %12.8f\n", s.w[1]);
    printf("w2 : %12.8f\n", s.w[2]);
    printf("w3 : %12.8f\n", s.w[3]);
#endif

    *_r = s.w[0] + s.w[1] + s.w[2] + s.w[3];
}


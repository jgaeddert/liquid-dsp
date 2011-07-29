/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// interleaver_create.c
//
// Create and initialize interleaver objects
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

interleaver interleaver_create(unsigned int _n,
                               interleaver_type _type)
{
    interleaver q = (interleaver) malloc(sizeof(struct interleaver_s));
    q->n = _n;

    return q;
}

// destroy interleaver object
void interleaver_destroy(interleaver _q)
{
    free(_q);
}

// initialize block interleaver
void interleaver_init_block(interleaver _q)
{
    unsigned int i,M=0,N,L=_q->n;
    // decompose into [M x N]
    L = _q->n;
    for (i=0; i<8*sizeof(unsigned int); i++) {
        if (L & 1)
            M = i;
        L >>= 1;
    }
    L = _q->n;

    M = 1<<(M/2);   // M ~ sqrt(L)
    if (M>1) M--;   // help ensure M is not exactly sqrt(L)

    if (M==0)
        fprintf(stderr,"warning: interleaver_init_block(), M=0\n");

    N = L / M;
    N += (L > (M*N)) ? 1 : 0; // ensures m*n >= _q->n

    //printf("len : %u, M=%u N=%u\n", _q->n, M, N);

    unsigned int j, m=0,n=0;
    for (i=0; i<L; i++) {
        //j = m*N + n; // input
        do {
            j = m*N + n; // output
            m++;
            if (m==M) {
                n = (n+1)%N;
                m=0;
            }
        } while (j>=L);
        
        //_q->p[i] = j;
        //printf("%u, ", j);
    }
    //printf("\n");

}

// initialize m-sequence interleaver
void interleaver_init_sequence(interleaver _q)
{
}

// set number of internal iterations
void interleaver_set_num_iterations(interleaver _q,
                                    unsigned int _n)
{
}



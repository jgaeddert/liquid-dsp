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
    q->len = _n;
    q->p = (unsigned int *) malloc((q->len)*sizeof(unsigned int));
    q->t = (unsigned char*) malloc((q->len)*sizeof(unsigned char));

    if (_n < 3) _type = LIQUID_INTERLEAVER_BLOCK;

    // initialize here
    switch (_type) {
    case LIQUID_INTERLEAVER_BLOCK:
        interleaver_init_block(q);
        break;
    case LIQUID_INTERLEAVER_SEQUENCE:
        interleaver_init_sequence(q);
        break;
    default:
        fprintf(stderr,"error: interleaver_create(), invalid type\n");
        exit(1);
    }

    // set default number of iterations
    interleaver_set_num_iterations(q,2);

    return q;
}

// destroy interleaver object
void interleaver_destroy(interleaver _q)
{
    free(_q->p);
    free(_q->t);
    free(_q);
}

// initialize block interleaver
void interleaver_init_block(interleaver _q)
{
    unsigned int i,M=0,N,L=_q->len;
    // decompose into [M x N]
    L = _q->len;
    for (i=0; i<8*sizeof(unsigned int); i++) {
        if (L & 1)
            M = i;
        L >>= 1;
    }
    L = _q->len;

    M = 1<<(M/2);   // M ~ sqrt(L)
    if (M>1) M--;   // help ensure M is not exactly sqrt(L)

    if (M==0)
        fprintf(stderr,"warning: interleaver_init_block(), M=0\n");

    N = L / M;
    N += (L > (M*N)) ? 1 : 0; // ensures m*n >= _q->len

    //printf("len : %u, M=%u N=%u\n", _q->len, M, N);

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
        
        _q->p[i] = j;
        //printf("%u, ", j);
    }
    //printf("\n");

}

// initialize m-sequence interleaver
void interleaver_init_sequence(interleaver _q)
{
    // generate msequence
    // m = ceil( log2( _q->len ) )
    unsigned int m = liquid_msb_index(_q->len);
    if (_q->len == (1<<(m-1)) )
        m--;
    msequence ms = msequence_create_default(m);
    unsigned int n = msequence_get_length(ms);
    unsigned int nby2 = n/2;

    unsigned int i, index=0;
    for (i=0; i<_q->len; i++) {
        // assign adding initial offset
        _q->p[(i+nby2)%(_q->len)] = index;

        do {
            index = ((index<<1) | msequence_advance(ms)) & n;
        } while (index >= _q->len);
    }
}

// set number of internal iterations
void interleaver_set_num_iterations(interleaver _q,
                                    unsigned int _n)
{
    _q->num_iterations = _n;

    if (_q->num_iterations > LIQUID_INTERLEAVER_NUM_MASKS) {
        fprintf(stderr,"warning: interleaver_set_num_iterations(), capping at %u\n",
                LIQUID_INTERLEAVER_NUM_MASKS);
        _q->num_iterations = LIQUID_INTERLEAVER_NUM_MASKS;
    }
}



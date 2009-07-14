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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

interleaver interleaver_create(unsigned int _n, interleaver_type _type)
{
    interleaver q = (interleaver) malloc(sizeof(struct interleaver_s));
    q->len = _n;
    q->p = (unsigned int *) malloc((q->len)*sizeof(unsigned int));
    q->t = (unsigned char*) malloc((q->len)*sizeof(unsigned char));

    if (_n < 3) _type = INT_BLOCK;

    // initialize here
    switch (_type) {
    case INT_BLOCK:
        interleaver_init_block(q);
        break;
    case INT_SEQUENCE:
        interleaver_init_sequence(q);
        break;
    default:
        printf("error: interleaver_create(), invalid type\n");
        exit(1);
    }

    return q;
}

void interleaver_destroy(interleaver _q)
{
    free(_q->p);
    free(_q->t);
    free(_q);
}

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
    //printf("m=%u\n", m);
    M = 1<<(M/2);   // m ~ sqrt(L)

    //M=2;
    N = L / M;
    N += (L > (M*N)) ? 1 : 0; // ensures m*n >= _q->len

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

void interleaver_init_sequence(interleaver _q)
{
    // generate msequence
    // m = ceil( log2( _q->len ) )
    unsigned int m = msb_index(_q->len);
    if (_q->len == (1<<(m-1)) )
        m--;
    msequence ms = msequence_create(m);
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



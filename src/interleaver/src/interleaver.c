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
// 
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h> // debug_print

#include "liquid.internal.h"

const unsigned char interleaver_mask[LIQUID_INTERLEAVER_NUM_MASKS] = {
    0x33, 0x55, 0x17, 0x6c};

// print interleaver internals
void interleaver_print(interleaver _q)
{
    unsigned int i;
    printf("interleaver [%u] :\n", _q->len);
    for (i=0; i<_q->len; i++)
        printf("  p[%u] : %u\n", i, _q->p[i]);
}

// print interleaver internals with debugging info
void interleaver_debug_print(interleaver _q)
{
    unsigned int n = (_q->len)*sizeof(unsigned char)*8;
    if (n>80) {
        printf("interleaver_debug_print(), too large to print debug info\n");
        return;
    }

    unsigned int t[n];
    interleaver_compute_bit_permutation(_q, t);

    unsigned int i,j,k;

    // compute permutation metrics: distance between bits
    float dtmp, dki, dmin=(float)n, dmean=0.0f;
    j = 0;
    for (k=1; k<3; k++) {
        //printf("==== k : %d\n", k);
        for (i=0; i<(n-k); i++) {
            dki = fabsf((float)(t[i]) - (float)(t[i+k]));
            dtmp = dki + (float)(k-1);
            //printf("    d(%u,%u) : %f\n", i, i+k, dki);

            dmean += dki;
            dmin = (dtmp < dmin) ? dki : dmin;
            j++;
        }
    }
    dmean /= j;

    printf("   ");
    j=0;
    for (i=0; i<n; i++) {
        if ((i%10)==0)  printf("%1u", j++);
        else            printf(" ");
    }
    printf("\n");

    for (i=0; i<n; i++) {
        printf("%2u ", i);
        for (j=0; j<n; j++) {
            if (j==t[i])
                printf("*");
            else if ((j%10)==0 && ((i%10)==0))
                printf("+");
            else if ((j%10)==0)
                printf("|");
            else if ((i%10)==0)
                printf("-");
            else if (j==i)
                printf("\\");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    printf("  dmin: %8.2f, dmean: %8.2f\n", dmin, dmean);
}

// execute forward interleaver (encoder)
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode(interleaver _q,
                        unsigned char * _msg_dec,
                        unsigned char * _msg_enc)
{
    memcpy(_msg_enc, _msg_dec, _q->len);

    // first iteration operates just on bytes
    if (_q->num_iterations > 0)
        interleaver_permute_forward(_msg_enc, _q->p, _q->len);

    unsigned int i;
    unsigned char mask=0x00;
    for (i=1; i<_q->num_iterations; i++) {
        unsigned int mask_id = i-1;
        mask = interleaver_mask[mask_id];

        interleaver_circshift_L4(_msg_enc, _q->len);
        interleaver_permute_forward_mask(_msg_enc, _q->p, _q->len, mask);
    }
}

// execute reverse interleaver (decoder)
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode(interleaver _q,
                        unsigned char * _msg_enc,
                        unsigned char * _msg_dec)
{
    memcpy(_msg_dec, _msg_enc, _q->len);

    unsigned int i;
    unsigned char mask=0x00;
    for (i=1; i<_q->num_iterations; i++) {
        unsigned int mask_id = _q->num_iterations-i;
        mask = interleaver_mask[mask_id];

        interleaver_permute_reverse_mask(_msg_dec, _q->p, _q->len, mask);
        interleaver_circshift_R4(_msg_dec, _q->len);
    }

    // first iteration operates just on bytes
    if (_q->num_iterations > 0)
        interleaver_permute_reverse(_msg_dec, _q->p, _q->len);
}


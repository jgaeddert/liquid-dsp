/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// structured quantizer
//

#include <stdio.h>
#include <stdlib.h>

struct QUANTIZER(_s) {
    int ctype;          // compander type
    unsigned int n;     // number of bits
                        // table?
};

// create quantizer object
//  _ctype      :   compander type (e.g. LIQUID_COMPANDER_LINEAR)
//  _range      :   maximum absolute input
//  _num_bits   :   number of bits per sample
QUANTIZER() QUANTIZER(_create)(liquid_compander_type _ctype,
                               float _range,
                               unsigned int _num_bits)
{
    // validate input
    if (_num_bits == 0) {
        fprintf(stderr,"error: quantizer_create(), must have at least one bit/sample\n");
        exit(1);
    }

    // create quantizer object
    QUANTIZER() q = (QUANTIZER()) malloc(sizeof(struct QUANTIZER(_s)));

    // initialize values
    q->ctype = _ctype;
    q->n     = _num_bits;

    // return object
    return q;
}

void QUANTIZER(_destroy)(QUANTIZER() _q)
{
    // free main object memory
    free(_q);
}

void QUANTIZER(_print)(QUANTIZER() _q)
{
    printf("quantizer:\n");
    printf("  compander :   ");
    switch(_q->ctype) {
    case LIQUID_COMPANDER_NONE:     printf("none\n");   break;
    case LIQUID_COMPANDER_LINEAR:   printf("linear\n"); break;
    case LIQUID_COMPANDER_MULAW:    printf("mu-law\n"); break;
    case LIQUID_COMPANDER_ALAW:     printf("A-law\n");  break;
    default:
        printf("unknown\n");
    }
    printf("  num bits  :   %u\n", _q->n);
}

void QUANTIZER(_execute_adc)(QUANTIZER() _q,
                             T _x,
                             unsigned int * _sample)
{
#if T_COMPLEX
#else
#endif
    *_sample = 0;
}

void QUANTIZER(_execute_dac)(QUANTIZER() _q,
                             unsigned int _sample,
                             T * _x)
{
#if T_COMPLEX
#else
#endif
    *_x = 0.0;
}



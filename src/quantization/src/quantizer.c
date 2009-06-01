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
#include <math.h>

#include "liquid.internal.h"

#define LIQUID_VALIDATE_INPUT
#define QUANTIZER_MAX_BITS      32

// inline quantizer: 'analog' signal in [-1, 1]

unsigned int quantize_adc(float _x, unsigned int _num_bits)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (_num_bits > QUANTIZER_MAX_BITS) {
        printf("error: quantize_adc(), maximum bits exceeded\n");
        exit(1);
    }
#endif

    // clip input
    _x = (_x >  1.0f) ?  1.0f : _x;
    _x = (_x < -1.0f) ? -1.0f : _x;

    unsigned int n = _num_bits-1;   // 
    unsigned int N = 1<<n;          // 2^n
    unsigned int r = roundf(fabsf(_x) * (N-1));

    // if negative set MSB to 1
    if (_x < 0)
        r |= N;

    return r;
}

float quantize_dac(unsigned int _s, unsigned int _num_bits)
{
#ifdef LIQUID_VALIDATE_INPUT
    if (_num_bits > QUANTIZER_MAX_BITS) {
        printf("error: quantize_dac(), maximum bits exceeded\n");
        exit(1);
    }
#endif
    unsigned int n = _num_bits-1;   //
    unsigned int N = 1<<n;          // 2^n
    float r = (float)(_s & (N-1)) / (float) (N-1);

    // check MSB, return negative if 1
    return (_s & N) ? -r : r;
}


struct quantizer_rf_s {
    unsigned int n;
};

quantizer_rf quantizer_rf_create(int _compander_type, float _range, unsigned int _num_bits)
{
    quantizer_rf q = (quantizer_rf) malloc(sizeof(struct quantizer_rf_s));
    return q;
}

void quantizer_rf_destroy(quantizer_rf _q)
{

}

void quantizer_rf_print(quantizer_rf _q)
{

}

void quantizer_rf_adc(quantizer_rf _q, float _x, unsigned int * _sample)
{

}

void quantizer_rf_dac(quantizer_rf _q, unsigned int _sample, float * _x)
{

}



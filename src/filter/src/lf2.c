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
// 2nd-order integrating loop filter
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

struct lf2_s {
    float BT;       // bandwidth-time product
    float beta;
    float alpha;

    float xi;       // damping factor
    float zeta;     // correction factor
        
    // loop filter state variables
    float tmp2;
    float q;
    float q_hat;
    float q_prime;
};

lf2 lf2_create(float _bt)
{
    // create object
    lf2 f = (lf2) malloc(sizeof(struct lf2_s));

    // set default damping factor
    f->xi = M_SQRT2;

    // reset internal state
    lf2_reset(f);

    // set bandwidth
    lf2_set_bandwidth(f, _bt);

    return f;
}

void lf2_destroy(lf2 _f)
{
    free(_f);
}

void lf2_print(lf2 _f)
{
    printf("loop filter : b = %6.4e\n", _f->BT);
}

void lf2_reset(lf2 _f)
{
    // reset internal filter state variables
    _f->tmp2 = 0.0f;
    _f->q = 0.0f;
    _f->q_hat = 0.0f;
    _f->q_prime = 0.0f;
}

void lf2_set_damping_factor(lf2 _f, float _xi)
{
    _f->xi = _xi;
    
    // recompute
    lf2_set_bandwidth(_f, _f->BT);
}

void lf2_set_bandwidth(lf2 _f, float _bt)
{
    _f->BT = _bt;
    lf2_generate_filter(_f);
}

// push input and compute output
void lf2_advance(lf2 _f, float _v, float *_v_hat)
{
    _f->q_prime = _v * _f->beta + _f->tmp2;
    _f->q_hat = _f->alpha*_v + _f->q_prime;
    _f->tmp2 = _f->q_prime;
    *_v_hat = _f->q_hat;
}

// 
// internal
//
void lf2_generate_filter(lf2 _f)
{
    float k1 = 1.0f;
    _f->beta = 2*(_f->BT)*(_f->xi+1/(4*(_f->xi)))/k1;
    _f->alpha = 2*(_f->xi)*(_f->beta);
}



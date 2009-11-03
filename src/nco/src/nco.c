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
// Numerically-controlled oscillator
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "liquid.internal.h"

#define NCO(name)   LIQUID_CONCAT(nco,name)
#define T           float
#define TC          float complex

#define SIN         sinf
#define COS         cosf
#define PI          (M_PI)

NCO() NCO(_create)(liquid_ncotype _type)
{
    NCO() q = (NCO()) malloc(sizeof(struct NCO(_s)));
    q->type = _type;

    // initialize sine table
    unsigned int i;
    for (i=0; i<256; i++)
        q->sintab[i] = sinf(2.0f*M_PI*(float)(i)/256.0f);

    NCO(_reset)(q);

    // set internal method
    if (q->type == LIQUID_NCO) {
        q->compute_sincos = &NCO(_compute_sincos);
    } else if (q->type == LIQUID_VCO) {
        q->compute_sincos = &vco_compute_sincos;
    } else {
        fprintf(stderr,"error: NCO(_create)(), unknown type : %u\n", q->type);
        exit(0);
    }

    return q;
}

void NCO(_destroy)(NCO() _q)
{
    free(_q);
}

void NCO(_reset)(NCO() _q)
{
    _q->theta = 0.0f;
    _q->d_theta = 0.0f;

    // reset sine table index
    _q->index = 0;

    // set internal sine, cosine values
    _q->sine = 0.0f;
    _q->cosine = 1.0f;
}

void NCO(_set_frequency)(NCO() _q, T _f)
{
    _q->d_theta = _f;
}

void NCO(_adjust_frequency)(NCO() _q, T _df)
{
    _q->d_theta += _df;
}

void NCO(_set_phase)(NCO() _q, T _phi)
{
    _q->theta = _phi;
    NCO(_constrain_phase)(_q);
}

void NCO(_adjust_phase)(NCO() _q, T _dphi)
{
    _q->theta += _dphi;
    NCO(_constrain_phase)(_q);
}

void NCO(_step)(NCO() _q)
{
    _q->theta += _q->d_theta;
    NCO(_constrain_phase)(_q);
}

T NCO(_get_phase)(NCO() _q) { return _q->theta; }
T NCO(_get_frequency)(NCO() _q) { return _q->d_theta; }

void NCO(_constrain_phase)(NCO() _q)
{
    if (_q->theta > PI)
        _q->theta -= 2*PI;
    else if (_q->theta < -PI)
        _q->theta += 2*PI;
}

void NCO(_compute_sincos)(NCO() _q)
{
    // assume phase is constrained to be in (-pi,pi)

    // compute index
    // NOTE : 40.743665 ~ 256 / (2*pi)
    // NOTE : add 512 to ensure positive value, add 0.5 for rounding precision
    // TODO : move away from floating-point specific code
    _q->index = ((unsigned int)((_q->theta)*40.743665f + 512.0f + 0.5f))&0xff;
    assert(_q->index < 256);
    
    _q->sine = _q->sintab[_q->index];
    _q->cosine = _q->sintab[(_q->index+64)&0xff];
}

void vco_compute_sincos(NCO() _q)
{
    _q->sine   = sinf(_q->theta);
    _q->cosine = cosf(_q->theta);
}

// TODO : compute sine, cosine internally
T NCO(_sin)(NCO() _q) {return sinf(_q->theta);}
T NCO(_cos)(NCO() _q) {return cosf(_q->theta);}

void NCO(_sincos)(NCO() _q, T* _s, T* _c)
{
    // compute sine, cosine internally
    _q->compute_sincos(_q);

    *_s = _q->sine;
    *_c = _q->cosine;
}

void NCO(_cexpf)(NCO() _q, TC * _y)
{
    // compute sine, cosine internally
    _q->compute_sincos(_q);

    // set _y[0] to [cos(theta) + _Complex_I*sin(theta)]
    *_y = _q->cosine + _Complex_I*(_q->sine);
}

// mixing functions

// Rotate input vector up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void NCO(_mix_up)(NCO() _q, TC _x, TC *_y)
{
    // compute sine, cosine internally
    _q->compute_sincos(_q);

    // multiply _x by [cos(theta) + _Complex_I*sin(theta)]
    *_y = _x * (_q->cosine + _Complex_I*(_q->sine));
    NCO(_step)(_q);
}

// Rotate input vector down by NCO angle, \f$\vec{y} = \vec{x}e^{-j\theta}\f$
void NCO(_mix_down)(NCO() _q, TC _x, TC *_y)
{
    // compute sine, cosine internally
    _q->compute_sincos(_q);

    // multiply _x by [cos(-theta) + _Complex_I*sin(-theta)]
    *_y = _x * (_q->cosine - _Complex_I*(_q->sine));
    NCO(_step)(_q);
}


// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
// TODO : implement NCO/VCO-specific versions
void NCO(_mix_block_up)(
    NCO() _q,
    TC *_x,
    TC *_y,
    unsigned int _N)
{
    unsigned int i;

    T theta =   _q->theta;
    T d_theta = _q->d_theta;
    for (i=0; i<_N; i++) {
        // multiply _x[i] by [cos(theta) + _Complex_I*sin(theta)]
        _y[i] = _x[i] * liquid_crotf_vect(theta);
        
        // NCO(_step(_q);
        theta += d_theta;
    }

    // NCO(_constrain_phase(_q);
    while (theta > PI)
        theta -= 2*PI;
    while (theta < -PI)
        theta += 2*PI;

    NCO(_set_phase)(_q, theta);
}

// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
// TODO : implement NCO/VCO-specific versions
void NCO(_mix_block_down)(
    NCO() _q,
    TC *_x,
    TC *_y,
    unsigned int _N)
{
    unsigned int i;

    T theta =   _q->theta;
    T d_theta = _q->d_theta;
    for (i=0; i<_N; i++) {
        // multiply _x[i] by [cos(-theta) + _Complex_I*sin(-theta)]
        _y[i] = _x[i] * liquid_crotf_vect(-theta);
        
        // NCO(_step(_q);
        theta += d_theta;
    }

    // NCO(_constrain_phase(_q);
    while (theta > PI)
        theta -= 2*PI;
    while (theta < -PI)
        theta += 2*PI;

    NCO(_set_phase)(_q, theta);
}


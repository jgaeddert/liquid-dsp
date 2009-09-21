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

nco nco_create()
{
    nco p = (nco) malloc(sizeof(struct nco_s));

    // initialize sine table
    unsigned int i;
    for (i=0; i<256; i++)
        p->sintab[i] = sinf(2.0f*M_PI*(float)(i)/256.0f);

    nco_reset(p);
    return p;
}

void nco_destroy(nco _nco)
{
    free(_nco);
}

void nco_reset(nco _nco)
{
    _nco->theta = 0.0f;
    _nco->d_theta = 0.0f;

    // reset sine table index
    _nco->index = 0;

    // set internal sine, cosine values
    _nco->sine = 0.0f;
    _nco->cosine = 1.0f;
}

void nco_set_frequency(nco _nco, float _f)
{
    _nco->d_theta = _f;
}

void nco_adjust_frequency(nco _nco, float _df)
{
    _nco->d_theta += _df;
}

void nco_set_phase(nco _nco, float _phi)
{
    _nco->theta = _phi;
    nco_constrain_phase(_nco);
}

void nco_adjust_phase(nco _nco, float _dphi)
{
    _nco->theta += _dphi;
    nco_constrain_phase(_nco);
}

void nco_step(nco _nco)
{
    _nco->theta += _nco->d_theta;
    nco_constrain_phase(_nco);
}

float nco_get_phase(nco _q) { return _q->theta; }
float nco_get_frequency(nco _q) { return _q->d_theta; }

void nco_constrain_phase(nco _nco)
{
    if (_nco->theta > M_PI)
        _nco->theta -= 2.0f*M_PI;
    else if (_nco->theta < -M_PI)
        _nco->theta += 2.0f*M_PI;
}

void nco_compute_sincos(nco _nco)
{
    // assume phase is constrained to be in (-pi,pi)

    // compute index
    // NOTE : 40.743665 ~ 256 / (2*pi)
    // NOTE : add 512 to ensure positive value, add 0.5 for rounding precision
    _nco->index = ((unsigned int)((_nco->theta)*40.743665f + 512.0f + 0.5f))&0xff;
    assert(_nco->index < 256);
    
    _nco->sine = _nco->sintab[_nco->index];
    _nco->cosine = _nco->sintab[(_nco->index+64)&0xff];
}

// TODO : use sine table
float nco_sin(nco _nco) {return sinf(_nco->theta);}
float nco_cos(nco _nco) {return cosf(_nco->theta);}

void nco_sincos(nco _nco, float* _s, float* _c)
{
    // compute sine, cosine from sine table
    nco_compute_sincos(_nco);
    *_s = _nco->sine;
    *_c = _nco->cosine;
}

void nco_cexpf(nco _nco, float complex * _y)
{
    // set _y[0] to [cos(theta) + _Complex_I*sin(theta)]
    *_y = liquid_crotf_vect(_nco->theta);
}

// mixing functions

// Rotate input vector up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_up(nco _nco, complex float _x, complex float *_y)
{
    // multiply _x by [cos(theta) + _Complex_I*sin(theta)]
    *_y = _x * liquid_crotf_vect(_nco->theta);
    nco_step(_nco);
}

// Rotate input vector down by NCO angle, \f$\vec{y} = \vec{x}e^{-j\theta}\f$
void nco_mix_down(nco _nco, complex float _x, complex float *_y)
{
    // multiply _x by [cos(-theta) + _Complex_I*sin(-theta)]
    *_y = _x * liquid_crotf_vect(-_nco->theta);
    nco_step(_nco);
}


// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_block_up(
    nco _nco,
    complex float *_x,
    complex float *_y,
    unsigned int _N)
{
    unsigned int i;

    float theta =   _nco->theta;
    float d_theta = _nco->theta;
    for (i=0; i<_N; i++) {
        // multiply _x[i] by [cos(theta) + _Complex_I*sin(theta)]
        _y[i] = _x[i] * liquid_crotf_vect(theta);
        
        // nco_step(_nco);
        theta += d_theta;
    }

    // nco_constrain_phase(_nco);
    while (theta > M_PI)
        theta -= 2*M_PI;
    while (theta < -M_PI)
        theta += 2*M_PI;

    nco_set_phase(_nco, theta);
}

// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
void nco_mix_block_down(
    nco _nco,
    complex float *_x,
    complex float *_y,
    unsigned int _N)
{
    unsigned int i;

    float theta =   _nco->theta;
    float d_theta = _nco->theta;
    for (i=0; i<_N; i++) {
        // multiply _x[i] by [cos(-theta) + _Complex_I*sin(-theta)]
        _y[i] = _x[i] * liquid_crotf_vect(-theta);
        
        // nco_step(_nco);
        theta += d_theta;
    }

    // nco_constrain_phase(_nco);
    while (theta > M_PI)
        theta -= 2*M_PI;
    while (theta < -M_PI)
        theta += 2*M_PI;

    nco_set_phase(_nco, theta);
}


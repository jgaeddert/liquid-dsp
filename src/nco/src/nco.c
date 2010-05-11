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

// create nco/vco object
NCO() NCO(_create)(liquid_ncotype _type)
{
    NCO() q = (NCO()) malloc(sizeof(struct NCO(_s)));
    q->type = _type;

    // initialize sine table
    unsigned int i;
    for (i=0; i<256; i++)
        q->sintab[i] = SIN(2.0f*M_PI*(float)(i)/256.0f);

    NCO(_reset)(q);

    // set internal method
    if (q->type == LIQUID_NCO) {
        q->compute_sincos = &NCO(_compute_sincos_nco);
    } else if (q->type == LIQUID_VCO) {
        q->compute_sincos = &NCO(_compute_sincos_vco);
    } else {
        fprintf(stderr,"error: NCO(_create)(), unknown type : %u\n", q->type);
        exit(0);
    }

    return q;
}

// destroy nco object
void NCO(_destroy)(NCO() _q)
{
    free(_q);
}

// reset internal state of nco object
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

// set frequency of nco object
void NCO(_set_frequency)(NCO() _q,
                         T _f)
{
    _q->d_theta = _f;
}

// adjust frequency of nco object
void NCO(_adjust_frequency)(NCO() _q,
                            T _df)
{
    _q->d_theta += _df;
}

// set phase of nco object, constraining phase
void NCO(_set_phase)(NCO() _q, T _phi)
{
    _q->theta = _phi;
    NCO(_constrain_phase)(_q);
}

// adjust phase of nco object, constraining phase
void NCO(_adjust_phase)(NCO() _q, T _dphi)
{
    _q->theta += _dphi;
    NCO(_constrain_phase)(_q);
}

// increment internal phase of nco object
void NCO(_step)(NCO() _q)
{
    _q->theta += _q->d_theta;
    NCO(_constrain_phase)(_q);
}

// get phase
T NCO(_get_phase)(NCO() _q) { return _q->theta; }

// ge frequency
T NCO(_get_frequency)(NCO() _q) { return _q->d_theta; }


// TODO : compute sine, cosine internally
T NCO(_sin)(NCO() _q) {return SIN(_q->theta);}
T NCO(_cos)(NCO() _q) {return COS(_q->theta);}

// compute sin, cos of internal phase
void NCO(_sincos)(NCO() _q, T* _s, T* _c)
{
    // compute sine, cosine internally, calling implementation-
    // specific function (nco, vco)
    _q->compute_sincos(_q);

    // return result
    *_s = _q->sine;
    *_c = _q->cosine;
}

// compute complex exponential of internal phase
void NCO(_cexpf)(NCO() _q,
                 TC * _y)
{
    // compute sine, cosine internally, calling implementation-
    // specific function (nco, vco)
    _q->compute_sincos(_q);

    // set _y[0] to [cos(theta) + _Complex_I*sin(theta)]
    *_y = _q->cosine + _Complex_I*(_q->sine);
}

// pll methods

// set pll bandwidth
void NCO(_pll_set_bandwidth)(NCO() _q,
                             T _b)
{
    // validate input
    if (_b < 0.0f) {
        fprintf(stderr,"error: nco_pll_set_bandwidth(), bandwidth must be positive\n");
        exit(1);
    }

    NCO(_pll_set_bandwidth_active_lag)(_q, _b);
    //NCO(_pll_set_bandwidth_active_PI)(_q, _b);
}

// advance pll
void NCO(_pll_step)(NCO() _q,
                    T _dphi)
{
    T phi_hat;

    // advance buffer
    _q->v[2] = _q->v[1];
    _q->v[1] = _q->v[0];

    // compute new v[0]
    _q->v[0] = _dphi - 
               _q->a[1]*_q->v[1] -
               _q->a[2]*_q->v[2];

    // compute output _y
    phi_hat = _q->b[0]*_q->v[0] +
              _q->b[1]*_q->v[1] +
              _q->b[2]*_q->v[2];

    _q->theta = phi_hat;

    // TODO : allow internal PLL to execute independently of NCO
}

// mixing functions

// Rotate input vector up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
//  _q      :   nco object
//  _x      :   input sample
//  _y      :   output sample
void NCO(_mix_up)(NCO() _q,
                  TC _x,
                  TC *_y)
{
    // compute sine, cosine internally, calling implementation-
    // specific function (nco, vco)
    _q->compute_sincos(_q);

    // multiply _x by [cos(theta) + _Complex_I*sin(theta)]
    *_y = _x * (_q->cosine + _Complex_I*(_q->sine));
    NCO(_step)(_q);
}

// Rotate input vector down by NCO angle, \f$\vec{y} = \vec{x}e^{-j\theta}\f$
//  _q      :   nco object
//  _x      :   input sample
//  _y      :   output sample
void NCO(_mix_down)(NCO() _q,
                    TC _x,
                    TC *_y)
{
    // compute sine, cosine internally
    _q->compute_sincos(_q);

    // multiply _x by [cos(-theta) + _Complex_I*sin(-theta)]
    *_y = _x * (_q->cosine - _Complex_I*(_q->sine));
    NCO(_step)(_q);
}


// Rotate input vector array up by NCO angle, \f$\vec{y} = \vec{x}e^{j\theta}\f$
// TODO : implement NCO/VCO-specific versions
//  _q      :   nco object
//  _x      :   input array [size: _n x 1]
//  _y      :   output sample [size: _n x 1]
//  _n      :   number of input, output samples
void NCO(_mix_block_up)(NCO() _q,
                        TC *_x,
                        TC *_y,
                        unsigned int _n)
{
    unsigned int i;

    T theta =   _q->theta;
    T d_theta = _q->d_theta;
    for (i=0; i<_n; i++) {
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
//  _q      :   nco object
//  _x      :   input array [size: _n x 1]
//  _y      :   output sample [size: _n x 1]
//  _n      :   number of input, output samples
void NCO(_mix_block_down)(NCO() _q,
                          TC *_x,
                          TC *_y,
                          unsigned int _n)
{
    unsigned int i;

    T theta =   _q->theta;
    T d_theta = _q->d_theta;
    for (i=0; i<_n; i++) {
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

//
// internal methods
//

// constrain phase of NCO object to be in (-pi,pi)
void NCO(_constrain_phase)(NCO() _q)
{
    if (_q->theta > PI)
        _q->theta -= 2*PI;
    else if (_q->theta < -PI)
        _q->theta += 2*PI;
}

// compute sin, cos of internal phase of nco
void NCO(_compute_sincos_nco)(NCO() _q)
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

// compute sin, cos of internal phase of vco
void NCO(_compute_sincos_vco)(NCO() _q)
{
    _q->sine   = SIN(_q->theta);
    _q->cosine = COS(_q->theta);
}

// use active lag loop filter
//          1 + t2 * s
//  F(s) = ------------
//          1 + t1 * s
void NCO(_pll_set_bandwidth_active_lag)(NCO() _q,
                                        float _b)
{
    _q->bandwidth = _b;
    _q->zeta = 1.0f / sqrt(2.0f);

    // loop filter (active lag)
    T wn = _q->bandwidth;   // natural frequency
    T zeta = _q->zeta;      // damping factor
    T K = 1000;             // loop gain
    T t1 = K/(wn*wn);
    T t2 = 2*zeta/wn - 1/K;

    _q->b[0] = 2*K*(1.+t2/2.0f);
    _q->b[1] = 2*K*2.;
    _q->b[2] = 2*K*(1.-t2/2.0f);

    _q->a[0] =  1 + t1/2.0f;
    _q->a[1] = -t1;
    _q->a[2] = -1 + t1/2.0f;

    // normalize coefficients
    _q->b[0] /= _q->a[0];
    _q->b[1] /= _q->a[0];
    _q->b[2] /= _q->a[0];

    _q->a[1] /= _q->a[0];
    _q->a[2] /= _q->a[0];
    _q->a[0] = 1.0f;
}

// use active PI ("proportional + integration") loop filter
//          1 + t2 * s
//  F(s) = ------------
//           t1 * s
void NCO(_pll_set_bandwidth_active_PI)(NCO() _q,
                                       float _b)
{
    _q->bandwidth = _b;
    _q->zeta = 1.0f / sqrt(2.0f);

    // loop filter (active lag)
    T wn = _q->bandwidth;   // natural frequency
    T zeta = _q->zeta;      // damping factor
    T K = 1000;             // loop gain
    T t1 = K/(wn*wn);
    T t2 = 2*zeta/wn;

    _q->b[0] = 2*K*(1.+t2/2.0f);
    _q->b[1] = 2*K*2.;
    _q->b[2] = 2*K*(1.-t2/2.0f);

    _q->a[0] =  t1/2.0f;
    _q->a[1] = -t1;
    _q->a[2] =  t1/2.0f;

    // normalize coefficients
    _q->b[0] /= _q->a[0];
    _q->b[1] /= _q->a[0];
    _q->b[2] /= _q->a[0];

    _q->a[1] /= _q->a[0];
    _q->a[2] /= _q->a[0];
    _q->a[0] = 1.0f;

}



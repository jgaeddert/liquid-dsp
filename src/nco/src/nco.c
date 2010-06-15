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
// Numerically-controlled oscillator (nco) with internal phase-locked
// loop (pll) implementation
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NCO_PLL_BANDWIDTH_DEFAULT   (0.1)
#define NCO_PLL_GAIN_DEFAULT        (1000)

// internal PLL IIR filter form
//  1   :   Direct Form-I   (suggested)
//  2   :   Direct Form-II  (can become unstable if filter bandwidth changes)
#define NCO_PLL_IIRFILT_FORM        (1)

#define LIQUID_DEBUG_NCO            (0)

// create nco/vco object
NCO() NCO(_create)(liquid_ncotype _type)
{
    NCO() q = (NCO()) malloc(sizeof(struct NCO(_s)));
    q->type = _type;

    // initialize sine table
    unsigned int i;
    for (i=0; i<256; i++)
        q->sintab[i] = SIN(2.0f*M_PI*(float)(i)/256.0f);

    // set default pll bandwidth
    NCO(_reset)(q);
    NCO(_pll_set_bandwidth)(q, NCO_PLL_BANDWIDTH_DEFAULT);

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
    _q->theta = 0;
    _q->d_theta = 0;

    // reset sine table index
    _q->index = 0;

    // set internal sine, cosine values
    _q->sine = 0;
    _q->cosine = 1;

    // reset pll filter state
    NCO(_pll_reset)(_q);

    // reset pll base frequency
    _q->pll_dtheta_base = 0;
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
    _q->theta += _q->pll_dtheta_base;
    NCO(_constrain_phase)(_q);
}

// get phase
T NCO(_get_phase)(NCO() _q)
{
    return _q->theta;
}

// ge frequency
T NCO(_get_frequency)(NCO() _q)
{
    // return both internal NCO phase step as well
    // as PLL phase step
    return _q->d_theta + _q->pll_dtheta_base;
}


// TODO : compute sine, cosine internally
T NCO(_sin)(NCO() _q)
{
    // compute internal sin, cos
    _q->compute_sincos(_q);

    // return resulting cosine component
    return _q->sine;
}

T NCO(_cos)(NCO() _q)
{
    // compute internal sin, cos
    _q->compute_sincos(_q);

    // return resulting cosine component
    return _q->cosine;
}

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

// reset pll state, retaining base frequency
void NCO(_pll_reset)(NCO() _q)
{
    // retain base frequency
    _q->pll_dtheta_base += _q->d_theta;
#if LIQUID_DEBUG_NCO
    printf("base frequency : %f\n", _q->pll_dtheta_base);
#endif

#if NCO_PLL_IIRFILT_FORM == 1
    // clear Direct Form I filter state
    _q->x[0] = 0;
    _q->x[1] = 0;
    _q->x[2] = 0;

    _q->y[0] = 0;
    _q->y[1] = 0;
    _q->y[2] = 0;

#elif NCO_PLL_IIRFILT_FORM == 2
    // clear Direct Form II filter state
    _q->v[0] = 0;
    _q->v[1] = 0;
    _q->v[2] = 0;
#else
#  error "invalid NCO_PLL_IIRFILT_FORM value"
#endif

    // reset phase state
    _q->pll_phi_prime = 0;
    _q->pll_phi_hat = 0;
}

// set pll bandwidth
void NCO(_pll_set_bandwidth)(NCO() _q,
                             T _b)
{
    // validate input
    if (_b < 0.0f) {
        fprintf(stderr,"error: nco_pll_set_bandwidth(), bandwidth must be positive\n");
        exit(1);
    }

#if NCO_PLL_IIRFILT_FORM == 2
    // reset pll, saving frequency state (pll_dtheta_base)
    NCO(_pll_reset)(_q);
#endif

    // compute loop filter using active lag design
    NCO(_pll_set_bandwidth_active_lag)(_q, _b);

    // compute loop filter using active PI design
    //NCO(_pll_set_bandwidth_active_PI)(_q, _b);
}

// advance pll phase
//  _q      :   nco object
//  _dphi   :   phase error
void NCO(_pll_step)(NCO() _q,
                    T _dphi)
{
    // save pll phase state
    _q->pll_phi_prime = _q->pll_phi_hat;

#if NCO_PLL_IIRFILT_FORM == 1
    // Direct Form I

    // advance buffer x
    _q->x[2] = _q->x[1];
    _q->x[1] = _q->x[0];
    _q->x[0] = _dphi;

    // advance buffer y
    _q->y[2] = _q->y[1];
    _q->y[1] = _q->y[0];

    // compute new v
    float v = _q->x[0] * _q->b[0] +
              _q->x[1] * _q->b[1] +
              _q->x[2] * _q->b[2];

    // compute new y[0]
    _q->pll_phi_hat = v -
                      _q->y[1] * _q->a[1] -
                      _q->y[2] * _q->a[2];

    _q->y[0] = _q->pll_phi_hat;

#elif NCO_PLL_IIRFILT_FORM == 2
    // Direct Form II

    // advance buffer
    _q->v[2] = _q->v[1];
    _q->v[1] = _q->v[0];

    // compute new v[0] from input
    _q->v[0] = _dphi - 
               _q->a[1]*_q->v[1] -
               _q->a[2]*_q->v[2];

    // compute output phase state
    _q->pll_phi_hat = _q->b[0]*_q->v[0] +
                      _q->b[1]*_q->v[1] +
                      _q->b[2]*_q->v[2];

#else
#  error "invalid NCO_PLL_IIRFILT_FORM value"
#endif

    // compute new phase step (frequency)
    NCO(_set_frequency)(_q, _q->pll_phi_hat - _q->pll_phi_prime);
}

// mixing functions

// Rotate input vector up by NCO angle, y = x exp{+j theta}
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
}

// Rotate input vector down by NCO angle, y = x exp{-j theta}
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
}


// Rotate input vector array up by NCO angle:
//      y(t) = x(t) exp{+j (f*t + theta)}
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
        
        theta += d_theta;
    }

    NCO(_set_phase)(_q, theta);
}

// Rotate input vector array down by NCO angle:
//      y(t) = x(t) exp{-j (f*t + theta)}
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
        
        theta += d_theta;
    }

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
    T wn = _q->bandwidth;       // natural frequency
    T zeta = _q->zeta;          // damping factor
    T K = NCO_PLL_GAIN_DEFAULT; // loop gain
    T t1 = K/(wn*wn);
    T t2 = 2*zeta/wn - 1/K;

    _q->b[0] = 2*K*(1.+t2/2.0f);
    _q->b[1] = 2*K*2.;
    _q->b[2] = 2*K*(1.-t2/2.0f);

    _q->a[0] =  1 + t1/2.0f;
    _q->a[1] = -t1;
    _q->a[2] = -1 + t1/2.0f;

    // normalize coefficients
    T a0 = _q->a[0];
    _q->b[0] /= a0;
    _q->b[1] /= a0;
    _q->b[2] /= a0;

    _q->a[1] /= a0;
    _q->a[2] /= a0;
    _q->a[0] /= a0;
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
    T wn = _q->bandwidth;       // natural frequency
    T zeta = _q->zeta;          // damping factor
    T K = NCO_PLL_GAIN_DEFAULT; // loop gain
    T t1 = K/(wn*wn);
    T t2 = 2*zeta/wn;

    _q->b[0] = 2*K*(1.+t2/2.0f);
    _q->b[1] = 2*K*2.;
    _q->b[2] = 2*K*(1.-t2/2.0f);

    _q->a[0] =  t1/2.0f;
    _q->a[1] = -t1;
    _q->a[2] =  t1/2.0f;

    // normalize coefficients
    T a0 = _q->a[0];
    _q->b[0] /= a0;
    _q->b[1] /= a0;
    _q->b[2] /= a0;

    _q->a[1] /= a0;
    _q->a[2] /= a0;
    _q->a[0] /= a0;

}



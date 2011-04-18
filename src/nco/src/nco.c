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
    q->a[0] = 1.0f;     q->b[0] = 0.0f;
    q->a[1] = 0.0f;     q->b[1] = 0.0f;
    q->a[2] = 0.0f;     q->b[2] = 0.0f;
    q->pll_filter = iirfiltsos_rrrf_create(q->b, q->a);
    NCO(_reset)(q);
    NCO(_pll_set_bandwidth)(q, NCO_PLL_BANDWIDTH_DEFAULT);

    // set internal method
    if (q->type == LIQUID_NCO) {
        q->compute_sincos = &NCO(_compute_sincos_nco);
    } else if (q->type == LIQUID_VCO) {
        q->compute_sincos = &NCO(_compute_sincos_vco);
    } else {
        fprintf(stderr,"error: NCO(_create)(), unknown type : %u\n", q->type);
        exit(1);
    }

    return q;
}

// destroy nco object
void NCO(_destroy)(NCO() _q)
{
    iirfiltsos_rrrf_destroy(_q->pll_filter);
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

    // clear phase-locked loop filter
    iirfiltsos_rrrf_clear(_q->pll_filter);

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

    _q->bandwidth = _b;
    _q->zeta = 1/sqrtf(2.0f);

    // compute loop filter using active lag design
    iirdes_pll_active_lag(_q->bandwidth, _q->zeta, NCO_PLL_GAIN_DEFAULT, _q->b, _q->a);

    // compute loop filter using active PI design
    //iirdes_pll_active_PI(_q->bandwidth, _q->zeta, NCO_PLL_GAIN_DEFAULT, _q->b, _q->a);
    
    iirfiltsos_rrrf_set_coefficients(_q->pll_filter, _q->b, _q->a);
}

// advance pll phase
//  _q      :   nco object
//  _dphi   :   phase error
void NCO(_pll_step)(NCO() _q,
                    T _dphi)
{
    // save pll phase state
    _q->pll_phi_prime = _q->pll_phi_hat;

    // execute internal filter (direct form I)
    iirfiltsos_rrrf_execute_df1(_q->pll_filter,
                                _dphi,
                                &_q->pll_phi_hat);

    // compute new phase step (frequency)
    NCO(_set_frequency)(_q, _q->pll_phi_hat - _q->pll_phi_prime);

    // constrain frequency
    //NCO(_constrain_frequency)(_q);
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
        _y[i] = _x[i] * liquid_cexpjf(theta);
        
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
        _y[i] = _x[i] * liquid_cexpjf(-theta);
        
        theta += d_theta;
    }

    NCO(_set_phase)(_q, theta);
}

//
// internal methods
//

// constrain frequency of NCO object to be in (-pi,pi)
void NCO(_constrain_frequency)(NCO() _q)
{
    if (_q->d_theta > M_PI)
        _q->d_theta -= 2*M_PI;
    else if (_q->d_theta < -M_PI)
        _q->d_theta += 2*M_PI;
}

// constrain phase of NCO object to be in (-pi,pi)
void NCO(_constrain_phase)(NCO() _q)
{
    if (_q->theta > M_PI)
        _q->theta -= 2*M_PI;
    else if (_q->theta < -M_PI)
        _q->theta += 2*M_PI;
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


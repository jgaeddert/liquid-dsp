/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

struct NCO(_s) {
    liquid_ncotype type;
    T theta;            // NCO phase
    T d_theta;          // NCO frequency
    T sintab[256];      // sine table
    unsigned int index; // table index
    T sine;
    T cosine;
    void (*compute_sincos)(NCO() _q);

    // phase-locked loop
    T alpha;
    T beta;
};

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

    // reset object and return
    NCO(_reset)(q);
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
T NCO(_get_phase)(NCO() _q)
{
    return _q->theta;
}

// ge frequency
T NCO(_get_frequency)(NCO() _q)
{
    return _q->d_theta;
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
}

// set pll bandwidth
void NCO(_pll_set_bandwidth)(NCO() _q,
                             T     _bandwidth)
{
    // validate input
    if (_bandwidth < 0.0f) {
        fprintf(stderr,"error: nco_pll_set_bandwidth(), bandwidth must be positive\n");
        exit(1);
    }

    _q->alpha = _bandwidth;         // frequency proportion
    _q->beta  = sqrtf(_q->alpha);   // phase proportion
}

// advance pll phase
//  _q      :   nco object
//  _dphi   :   phase error
void NCO(_pll_step)(NCO() _q,
                    T     _dphi)
{
    // increase frequency proportional to error
    NCO(_adjust_frequency)(_q, _dphi*_q->alpha);

    // increase phase proportional to error
    NCO(_adjust_phase)(_q, _dphi*_q->beta);

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
    // FIXME: this method should be more efficient but is causing occasional
    //        errors so instead favor slower but more reliable algorithm
#if 0
    T theta =   _q->theta;
    T d_theta = _q->d_theta;
    for (i=0; i<_n; i++) {
        // multiply _x[i] by [cos(theta) + _Complex_I*sin(theta)]
        _y[i] = _x[i] * liquid_cexpjf(theta);
        
        theta += d_theta;
    }

    NCO(_set_phase)(_q, theta);
#else
    for (i=0; i<_n; i++) {
        // mix single sample up
        NCO(_mix_up)(_q, _x[i], &_y[i]);

        // step NCO phase
        NCO(_step)(_q);
    }
#endif
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
    // FIXME: this method should be more efficient but is causing occasional
    //        errors so instead favor slower but more reliable algorithm
#if 0
    T theta =   _q->theta;
    T d_theta = _q->d_theta;
    for (i=0; i<_n; i++) {
        // multiply _x[i] by [cos(-theta) + _Complex_I*sin(-theta)]
        _y[i] = _x[i] * liquid_cexpjf(-theta);
        
        theta += d_theta;
    }

    NCO(_set_phase)(_q, theta);
#else
    for (i=0; i<_n; i++) {
        // mix single sample down
        NCO(_mix_down)(_q, _x[i], &_y[i]);

        // step NCO phase
        NCO(_step)(_q);
    }
#endif
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


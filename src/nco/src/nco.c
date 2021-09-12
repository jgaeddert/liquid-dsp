/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
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

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NCO_PLL_BANDWIDTH_DEFAULT   (0.1)
#define NCO_PLL_GAIN_DEFAULT        (1000)

#define LIQUID_DEBUG_NCO            (0)

struct NCO(_s) {
    liquid_ncotype  type;           // NCO type (e.g. LIQUID_VCO)
    T               sintab[1024];   // sine look-up table
    uint32_t        theta;          // 32-bit phase     [radians]
    uint32_t        d_theta;        // 32-bit frequency [radians/sample]

    // phase-locked loop
    T               alpha;          // frequency proportion
    T               beta;           // phase proportion
};


// constrain phase/frequency to be in [-pi,pi)
int NCO(_constrain_phase)(NCO() _q);
int NCO(_constrain_frequency)(NCO() _q);

// compute trigonometric functions for nco/vco type
int NCO(_compute_sincos_nco)(NCO() _q);
int NCO(_compute_sincos_vco)(NCO() _q);

// reset internal phase-locked loop filter
int NCO(_pll_reset)(NCO() _q);

// constrain phase (or frequency) and convert to fixed-point
uint32_t NCO(_constrain)(float _theta);

// compute index for sine look-up table
unsigned int NCO(_index)(NCO() _q);

// create nco/vco object
NCO() NCO(_create)(liquid_ncotype _type)
{
    NCO() q = (NCO()) malloc(sizeof(struct NCO(_s)));
    q->type = _type;

    // initialize sine table
    unsigned int i;
    for (i=0; i<1024; i++)
        q->sintab[i] = SIN(2.0f*LIQUID_PI*(float)(i)/1024.0f);

    // set default pll bandwidth
    NCO(_pll_set_bandwidth)(q, NCO_PLL_BANDWIDTH_DEFAULT);

    // reset object and return
    NCO(_reset)(q);
    return q;
}

// destroy nco object
int NCO(_destroy)(NCO() _q)
{
    if (_q==NULL)
        return liquid_error(LIQUID_EIOBJ,"nco_%s_destroy(), object is null", EXTENSION);

    free(_q);
    return LIQUID_OK;
}

// Print nco object internals to stdout
int NCO(_print)(NCO() _q)
{
    printf("nco [phase: 0x%.8x rad, freq: 0x%.8x rad/sample]\n",
            _q->theta, _q->d_theta);
#if LIQUID_DEBUG_NCO
    // print entire table
    unsigned int i;
    for (i=0; i<1024; i++)
        printf("  sintab[%4u] = %16.12f\n", i, _q->sintab[i]);
#endif
    return LIQUID_OK;
}

// reset internal state of nco object
int NCO(_reset)(NCO() _q)
{
    // reset phase and frequency states
    _q->theta   = 0;
    _q->d_theta = 0;

    // reset pll filter state
    return NCO(_pll_reset)(_q);
}

// set frequency of nco object
int NCO(_set_frequency)(NCO() _q,
                        T     _dtheta)
{
    _q->d_theta = NCO(_constrain)(_dtheta);
    return LIQUID_OK;
}

// adjust frequency of nco object
int NCO(_adjust_frequency)(NCO() _q,
                           T     _df)
{
    _q->d_theta += NCO(_constrain)(_df);
    return LIQUID_OK;
}

// set phase of nco object, constraining phase
int NCO(_set_phase)(NCO() _q,
                    T     _phi)
{
    _q->theta = NCO(_constrain)(_phi);
    return LIQUID_OK;
}

// adjust phase of nco object, constraining phase
int NCO(_adjust_phase)(NCO() _q,
                       T     _dphi)
{
    _q->theta += NCO(_constrain)(_dphi);
    return LIQUID_OK;
}

// increment internal phase of nco object
int NCO(_step)(NCO() _q)
{
    _q->theta += _q->d_theta;
    return LIQUID_OK;
}

// get phase [radians]
T NCO(_get_phase)(NCO() _q)
{
    return 2.0f*LIQUID_PI*(float)_q->theta / (float)(1LLU<<32);
}

// get frequency [radians/sample]
T NCO(_get_frequency)(NCO() _q)
{
    float d_theta = 2.0f*LIQUID_PI*(float)_q->d_theta / (float)(1LLU<<32);
    return d_theta > LIQUID_PI ? d_theta - 2*LIQUID_PI : d_theta;
}

// compute sine, cosine internally
T NCO(_sin)(NCO() _q)
{
    unsigned int index = NCO(_index)(_q);
    return _q->sintab[index];
}

T NCO(_cos)(NCO() _q)
{
    // add pi/2 phase shift
    unsigned int index = (NCO(_index)(_q) + 256) & 0x3ff;
    return _q->sintab[index];
}

// compute sin, cos of internal phase
int NCO(_sincos)(NCO() _q,
                 T *   _s,
                 T *   _c)
{
    // add pi/2 phase shift
    unsigned int index = NCO(_index)(_q);

    // return result
    *_s = _q->sintab[(index    )        ];
    *_c = _q->sintab[(index+256) & 0x3ff];
    return LIQUID_OK;
}

// compute complex exponential of internal phase
int NCO(_cexpf)(NCO() _q,
                TC *  _y)
{
    float vsin;
    float vcos;
    NCO(_sincos)(_q, &vsin, &vcos);
    *_y = vcos + _Complex_I*vsin;
    return LIQUID_OK;
}

// pll methods

// reset pll state, retaining base frequency
int NCO(_pll_reset)(NCO() _q)
{
    return LIQUID_OK;
}

// set pll bandwidth
int NCO(_pll_set_bandwidth)(NCO() _q,
                            T     _bw)
{
    // validate input
    if (_bw < 0.0f)
        return liquid_error(LIQUID_EIRANGE,"nco_pll_set_bandwidth(), bandwidth must be positive");

    _q->alpha = _bw;                // frequency proportion
    _q->beta  = SQRT(_q->alpha);    // phase proportion
    return LIQUID_OK;
}

// advance pll phase
//  _q      :   nco object
//  _dphi   :   phase error
int NCO(_pll_step)(NCO() _q,
                   T     _dphi)
{
    // increase frequency proportional to error
    NCO(_adjust_frequency)(_q, _dphi*_q->alpha);

    // increase phase proportional to error
    NCO(_adjust_phase)(_q, _dphi*_q->beta);

    // constrain frequency
    //NCO(_constrain_frequency)(_q);
    return LIQUID_OK;
}

// mixing functions

// Rotate input vector up by NCO angle, y = x exp{+j theta}
//  _q      :   nco object
//  _x      :   input sample
//  _y      :   output sample
int NCO(_mix_up)(NCO() _q,
                 TC    _x,
                 TC *  _y)
{
    // compute complex phasor
    TC v;
    NCO(_cexpf)(_q, &v);

    // rotate input
    *_y = _x * v;
    return LIQUID_OK;
}

// Rotate input vector down by NCO angle, y = x exp{-j theta}
//  _q      :   nco object
//  _x      :   input sample
//  _y      :   output sample
int NCO(_mix_down)(NCO() _q,
                   TC    _x,
                   TC *  _y)
{
    // compute complex phasor
    TC v;
    NCO(_cexpf)(_q, &v);

    // rotate input (negative direction)
    *_y = _x * CONJ(v);
    return LIQUID_OK;
}


// Rotate input vector array up by NCO angle:
//      y(t) = x(t) exp{+j (f*t + theta)}
// TODO : implement NCO/VCO-specific versions
//  _q      :   nco object
//  _x      :   input array [size: _n x 1]
//  _y      :   output sample [size: _n x 1]
//  _n      :   number of input, output samples
int NCO(_mix_block_up)(NCO()        _q,
                       TC *         _x,
                       TC *         _y,
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
    return LIQUID_OK;
}

// Rotate input vector array down by NCO angle:
//      y(t) = x(t) exp{-j (f*t + theta)}
// TODO : implement NCO/VCO-specific versions
//  _q      :   nco object
//  _x      :   input array [size: _n x 1]
//  _y      :   output sample [size: _n x 1]
//  _n      :   number of input, output samples
int NCO(_mix_block_down)(NCO()        _q,
                         TC *         _x,
                         TC *         _y,
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
    return LIQUID_OK;
}

//
// internal methods
//

// constrain phase (or frequency) and convert to fixed-point
uint32_t NCO(_constrain)(float _theta)
{
    // divide value by 2*pi and compute modulo
    float p = _theta * 0.159154943091895;   // 1/(2 pi) ~ 0.159154943091895

    // extract fractional part of p
    float fpart = p - ((long)p);    // fpart is in (-1,1)

    // ensure fpart is in [0,1)
    if (fpart < 0.) fpart += 1.;

    // map to range of precision needed
    uint32_t retVal = (uint32_t)(fpart * 0xffffffff);
    return retVal >= UINT_MAX ? UINT_MAX : retVal;
}

// compute index for sine look-up table
unsigned int NCO(_index)(NCO() _q)
{
    //return (_q->theta >> 22) & 0x3ff; // round down
    return ((_q->theta + (1<<21)) >> 22) & 0x3ff; // round appropriately
}


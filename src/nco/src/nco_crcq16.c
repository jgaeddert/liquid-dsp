/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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
// numerically-controlled oscillator (nco) API, 16-bit fixed-point precision
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "liquidfpm.internal.h"
#include "liquid.internal.h"

#define EXTENSION "crcq16"

#define NCO_PLL_BANDWIDTH_DEFAULT   (0.1)
#define NCO_PLL_GAIN_DEFAULT        (1000)

#define LIQUID_DEBUG_NCO            (0)

struct nco_crcq16_s {
    liquid_ncotype type;
    q16_t theta;            // NCO phase
    q16_t d_theta;          // NCO frequency
    q16_t sintab[256];      // sine table
    unsigned int index; // table index
    q16_t sine;
    q16_t cosine;
    int (*compute_sincos)(nco_crcq16 _q);

    // phase-locked loop
    q16_t bandwidth;
    q16_t zeta;
    q16_t a[3];
    q16_t b[3];
    iirfiltsos_rrrq16 pll_filter;    // phase-locked loop filter
};

// 
// forward declaration of internal methods
//

// constrain phase/frequency to be in [-pi,pi)
int nco_crcq16_constrain_phase(nco_crcq16 _q);
int nco_crcq16_constrain_frequency(nco_crcq16 _q);

// compute trigonometric functions for nco/vco type
int nco_crcq16_compute_sincos_nco(nco_crcq16 _q);
int nco_crcq16_compute_sincos_vco(nco_crcq16 _q);

// reset internal phase-locked loop filter
int nco_crcq16_pll_reset(nco_crcq16 _q);

// create nco/vco object
nco_crcq16 nco_crcq16_create(liquid_ncotype _type)
{
    nco_crcq16 q = (nco_crcq16) malloc(sizeof(struct nco_crcq16_s));
    q->type = _type;

    // initialize sine table
    unsigned int i;
    for (i=0; i<256; i++)
        q->sintab[i] = q16_float_to_fixed(sinf(2.0f*M_PI*(float)(i)/256.0f));

    // set default pll bandwidth
    q->a[0] = q16_one;  q->b[0] = 0;
    q->a[1] = 0;        q->b[1] = 0;
    q->a[2] = 0;        q->b[2] = 0;
    q->pll_filter = iirfiltsos_rrrq16_create(q->b, q->a);
    nco_crcq16_reset(q);
    nco_crcq16_pll_set_bandwidth(q, NCO_PLL_BANDWIDTH_DEFAULT);

    // set internal method
    if (q->type == LIQUID_NCO) {
        q->compute_sincos = &nco_crcq16_compute_sincos_nco;
    } else if (q->type == LIQUID_VCO) {
        q->compute_sincos = &nco_crcq16_compute_sincos_vco;
    } else if (q->type == LIQUID_VCO_DIRECT) {
        return liquid_error_config("nco_%s_create(), unsupported type 'LIQUID_VCO_DIRECT'", EXTENSION);
    } else {
        return liquid_error_config("nco_%s_create(), unknown type : %u", EXTENSION, q->type);
    }

    return q;
}

// copy object
nco_crcq16 nco_crcq16_copy(nco_crcq16 _q)
{
    return liquid_error_config("nco_%s_copy(), method not yet implemented", EXTENSION);
}

// destroy nco object
int nco_crcq16_destroy(nco_crcq16 _q)
{
    iirfiltsos_rrrq16_destroy(_q->pll_filter);
    free(_q);
    return LIQUID_OK;
}

//
int nco_crcq16_print(nco_crcq16 _q)
{
    printf("<liquid.nco_%s", EXTENSION);
    switch (_q->type) {
    case LIQUID_NCO: printf(", type=\"nco\""); break;
    case LIQUID_VCO_INTERP:
    case LIQUID_VCO_DIRECT:
        printf(", type=\"vco\""); break;
    default:;
    }
    printf(", phase=0x%.8x", _q->theta);
    printf(", freq=0x%.8x", _q->d_theta);
    printf(">\n");
    return LIQUID_OK;
}

// reset internal state of nco object
int nco_crcq16_reset(nco_crcq16 _q)
{
    _q->theta = 0;
    _q->d_theta = 0;

    // reset sine table index
    _q->index = 0;

    // set internal sine, cosine values
    _q->sine = 0;
    _q->cosine = 1;

    // reset pll filter state
    return nco_crcq16_pll_reset(_q);
}

// get frequency
q16_t nco_crcq16_get_frequency(nco_crcq16 _q)
{
    // return both internal NCO phase step as well
    // as PLL phase step
    return _q->d_theta;
}

// set frequency of nco object
int nco_crcq16_set_frequency(nco_crcq16 _q,
                             q16_t      _f)
{
    _q->d_theta = _f;
    //_q->d_theta = q16_2pi >> 6;
    return LIQUID_OK;
}

// adjust frequency of nco object
int nco_crcq16_adjust_frequency(nco_crcq16 _q,
                                q16_t _df)
{
    _q->d_theta += _df;
    return LIQUID_OK;
}

// get phase
q16_t nco_crcq16_get_phase(nco_crcq16 _q)
{
    return _q->theta;
}

// set phase of nco object, constraining phase
int nco_crcq16_set_phase(nco_crcq16 _q, q16_t _phi)
{
    _q->theta = _phi;
    //nco_crcq16_constrain_phase(_q);
    return LIQUID_OK;
}

// adjust phase of nco object, constraining phase
int nco_crcq16_adjust_phase(nco_crcq16 _q, q16_t _dphi)
{
    _q->theta += _dphi;
    //nco_crcq16_constrain_phase(_q);
    return LIQUID_OK;
}

// TODO: add stub for get_vcodirect_frequency
// TODO: add stub for set_vcodirect_frequency

// increment internal phase of nco object
int nco_crcq16_step(nco_crcq16 _q)
{
    _q->theta += _q->d_theta;
    //nco_crcq16_constrain_phase(_q);
    return LIQUID_OK;
}

// compute and return sine
q16_t nco_crcq16_sin(nco_crcq16 _q)
{
    // compute internal sin, cos
    _q->compute_sincos(_q);

    // return resulting cosine component
    return _q->sine;
}

// compute and return cosine
q16_t nco_crcq16_cos(nco_crcq16 _q)
{
    // compute internal sin, cos
    _q->compute_sincos(_q);

    // return resulting cosine component
    return _q->cosine;
}

// compute sin, cos of internal phase
int nco_crcq16_sincos(nco_crcq16 _q, q16_t* _s, q16_t* _c)
{
    // compute sine, cosine internally, calling implementation-
    // specific function (nco, vco)
    _q->compute_sincos(_q);

    // return result
    *_s = _q->sine;
    *_c = _q->cosine;
    return LIQUID_OK;
}

// compute complex exponential of internal phase
int nco_crcq16_cexpf(nco_crcq16 _q,
                      cq16_t *   _y)
{
#if 0
    // compute sine, cosine internally, calling implementation-
    // specific function (nco, vco)
    _q->compute_sincos(_q);

    // set _y[0] to [cos(theta) + _Complex_I*sin(theta)]
    *_y = _q->cosine + _Complex_I*(_q->sine);
#else
    // FIXME: use internal values
    *_y = cq16_cexpj( _q->theta );
    //_y->real = _q->cosine;
    //_y->imag = _q->sine;
#endif
    return LIQUID_OK;
}

// pll methods

// reset pll state, retaining base frequency
// TODO: add macro for this in global header
int nco_crcq16_pll_reset(nco_crcq16 _q)
{
    // clear phase-locked loop filter
    return iirfiltsos_rrrq16_reset(_q->pll_filter);
}

// set pll bandwidth
int nco_crcq16_pll_set_bandwidth(nco_crcq16 _q,
                                 float      _b)
{
    // validate input
    if (_b < 0.0f) {
        return liquid_error(LIQUID_EICONFIG, "nco_%s_set_bandwidth(), bandwidth must be positive", EXTENSION);
    }

    _q->bandwidth = _b;
    _q->zeta = 1/sqrtf(2.0f);

    float K     = NCO_PLL_GAIN_DEFAULT; // gain
    float zeta  = 1.0f / sqrtf(2.0f);   // damping factor
    float wn    = _b;                   // natural frequency
    float t1    = K/(wn*wn);            // 
    float t2    = 2.*zeta/wn - 1./K;    //

    // compute scaling factor
    float v = 1.0f / (1. + t1/2.0f);

    // feed-forward coefficients
    float b0 = v*2*K*(1.+t2/2.0f);
    float b1 = v*2*K*(2.        );
    float b2 = v*2*K*(1.-t2/2.0f);

    // feed-back coefficients
    float a0 = 1.0f;
    float a1 = v*(-1. + t1/2.0f);
    float a2 = 0.0f;
    //printf("b = [%8.4f %8.4f %8.4f]\n", b0, b1, b2);
    //printf("a = [%8.4f %8.4f %8.4f]\n", a0, a1, a2);

    // feed-forward coefficients
    _q->b[0] = q16_float_to_fixed( b0 );
    _q->b[1] = q16_float_to_fixed( b1 );
    _q->b[2] = q16_float_to_fixed( b2 );

    // feed-back coefficients
    _q->a[0] = q16_float_to_fixed( a0 );
    _q->a[1] = q16_float_to_fixed( a1 );
    _q->a[2] = q16_float_to_fixed( a2 );
    
    // set coefficients
    return iirfiltsos_rrrq16_set_coefficients(_q->pll_filter, _q->b, _q->a);
}

// advance pll phase
//  _q      :   nco object
//  _dphi   :   phase error
int nco_crcq16_pll_step(nco_crcq16 _q,
                        q16_t      _dphi)
{
    // execute internal filter (direct form I)
    q16_t error_filtered = 0.0f;
    iirfiltsos_rrrq16_execute_df1(_q->pll_filter,
                                  _dphi,
                                  &error_filtered);

    // increase frequency proportional to error
    nco_crcq16_adjust_frequency(_q, error_filtered);

    // constrain frequency
    //nco_crcq16_constrain_frequency(_q);
    return LIQUID_OK;
}

// mixing functions

// Rotate input vector up by NCO angle, y = x exp{+j theta}
//  _q      :   nco object
//  _x      :   input sample
//  _y      :   output sample
int nco_crcq16_mix_up(nco_crcq16 _q,
                      cq16_t     _x,
                      cq16_t *   _y)
{
    // compute sine, cosine internally, calling implementation-
    // specific function (nco, vco)
    _q->compute_sincos(_q);

    // multiply _x by [cos(theta) + _Complex_I*sin(theta)]
    cq16_t v;
    v.real = _q->cosine;
    v.imag = _q->sine;
    *_y = cq16_mul(_x, v);
#if 0
    *_y = _x * (_q->cosine + _Complex_I*(_q->sine));
#endif
    return LIQUID_OK;
}

// Rotate input vector down by NCO angle, y = x exp{-j theta}
//  _q      :   nco object
//  _x      :   input sample
//  _y      :   output sample
int nco_crcq16_mix_down(nco_crcq16 _q,
                        cq16_t     _x,
                        cq16_t *   _y)
{
    // compute sine, cosine internally
    _q->compute_sincos(_q);

    // multiply _x by [cos(-theta) + _Complex_I*sin(-theta)]
    cq16_t v;
    v.real = _q->cosine;
    v.imag = -(_q->sine);
    *_y = cq16_mul(_x, v);
#if 0
    *_y = _x * (_q->cosine - _Complex_I*(_q->sine));
#endif
    return LIQUID_OK;
}


// Rotate input vector array up by NCO angle:
//      y(t) = x(t) exp{+j (f*t + theta)}
// TODO : implement NCO/VCO-specific versions
//  _q      :   nco object
//  _x      :   input array [size: _n x 1]
//  _y      :   output sample [size: _n x 1]
//  _n      :   number of input, output samples
int nco_crcq16_mix_block_up(nco_crcq16   _q,
                            cq16_t *     _x,
                            cq16_t *     _y,
                            unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        nco_crcq16_mix_up(_q, _x[i], &_y[i]);
#if 0
    q16_t theta =   _q->theta;
    q16_t d_theta = _q->d_theta;
    for (i=0; i<_n; i++) {
        // multiply _x[i] by [cos(theta) + _Complex_I*sin(theta)]
        _y[i] = _x[i] * liquid_cexpjf(theta);
        
        theta += d_theta;
    }

    nco_crcq16_set_phase(_q, theta);
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
int nco_crcq16_mix_block_down(nco_crcq16   _q,
                              cq16_t *     _x,
                              cq16_t *     _y,
                              unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        nco_crcq16_mix_down(_q, _x[i], &_y[i]);
#if 0
    q16_t theta =   _q->theta;
    q16_t d_theta = _q->d_theta;
    for (i=0; i<_n; i++) {
        // multiply _x[i] by [cos(-theta) + _Complex_I*sin(-theta)]
        _y[i] = _x[i] * liquid_cexpjf(-theta);
        
        theta += d_theta;
    }

    nco_crcq16_set_phase(_q, theta);
#endif
    return LIQUID_OK;
}

//
// internal methods
//

// constrain frequency of NCO object to be in (-pi,pi)
int nco_crcq16_constrain_frequency(nco_crcq16 _q)
{
    if (_q->d_theta > q16_pi)
        _q->d_theta -= q16_2pi;
    else if (_q->d_theta < -q16_pi)
        _q->d_theta += q16_2pi;
    return LIQUID_OK;
}

// constrain phase of NCO object to be in (-pi,pi)
int nco_crcq16_constrain_phase(nco_crcq16 _q)
{
    if (_q->theta > q16_pi)
        _q->theta -= q16_2pi;
    else if (_q->theta < -q16_pi)
        _q->theta += q16_2pi;
    return LIQUID_OK;
}

// compute sin, cos of internal phase of nco
int nco_crcq16_compute_sincos_nco(nco_crcq16 _q)
{
    // use fixed look-up table
    q16_sincos_tab(_q->theta, &_q->sine, &_q->cosine);
    return LIQUID_OK;
}

// compute sin, cos of internal phase of vco
int nco_crcq16_compute_sincos_vco(nco_crcq16 _q)
{
    // use cordic (although might not be as accurate as q16_sincos_tab() method)
    unsigned int num_iterations = q16_bits;
    q16_sincos_cordic(_q->theta, &_q->sine, &_q->cosine, num_iterations);
    return LIQUID_OK;
}


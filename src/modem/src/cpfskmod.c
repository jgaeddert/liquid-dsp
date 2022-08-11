/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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
// continuous phase frequency-shift keying modulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// 
// internal methods
//

// design transmit filter
int cpfskmod_firdes(unsigned int _k,
                    unsigned int _m,
                    float        _beta,
                    int          _type,
                    float *      _h,
                    unsigned int _h_len);

// cpfskmod
struct cpfskmod_s {
    // common
    unsigned int bps;           // bits per symbol
    unsigned int k;             // samples per symbol
    unsigned int m;             // filter delay (symbols)
    float        beta;          // filter bandwidth parameter
    float        h;             // modulation index
    int          type;          // filter type (e.g. LIQUID_CPFSK_SQUARE)
    unsigned int M;             // constellation size
    unsigned int symbol_delay;  // transmit filter delay [symbols]

    // pulse-shaping filter
    float * ht;                 // filter coefficients
    unsigned int ht_len;        // filter length
    firinterp_rrrf  interp;     // interpolator

    // phase integrator
    float * phase_interp;       // phase interpolation buffer
    iirfilt_rrrf integrator;    // integrator
};

// create cpfskmod object (frequency modulator)
//  _bps    :   bits per symbol, _bps > 0
//  _h      :   modulation index, _h > 0
//  _k      :   samples/symbol, _k > 1, _k even
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   filter bandwidth parameter, _beta > 0
//  _type   :   filter type (e.g. LIQUID_CPFSK_SQUARE)
cpfskmod cpfskmod_create(unsigned int _bps,
                         float        _h,
                         unsigned int _k,
                         unsigned int _m,
                         float        _beta,
                         int          _type)
{
    // validate input
    if (_bps == 0)
        return liquid_error_config("cpfskmod_create(), bits/symbol must be greater than 0");
    if (_k < 2 || (_k%2))
        return liquid_error_config("cpfskmod_create(), samples/symbol must be greater than 2 and even");
    if (_m == 0)
        return liquid_error_config("cpfskmod_create(), filter delay must be greater than 0");
    if (_beta <= 0.0f || _beta > 1.0f)
        return liquid_error_config("cpfskmod_create(), filter roll-off must be in (0,1]");
    if (_h <= 0.0f)
        return liquid_error_config("cpfskmod_create(), modulation index must be greater than 0");

    // create main object memory
    cpfskmod q = (cpfskmod) malloc(sizeof(struct cpfskmod_s));

    // set basic internal properties
    q->bps  = _bps;     // bits per symbol
    q->h    = _h;       // modulation index
    q->k    = _k;       // samples per symbol
    q->m    = _m;       // filter delay (symbols)
    q->beta = _beta;    // filter roll-off factor (only for certain filters)
    q->type = _type;    // filter type

    // derived values
    q->M = 1 << q->bps; // constellation size

    // create object depending upon input type
    float b[2] = {0.5f,  0.5f}; // integrator feed-forward coefficients
    float a[2] = {1.0f, -1.0f}; // integrator feed-back coefficients
    q->ht_len = 0;
    q->ht = NULL;
    unsigned int i;
    switch(q->type) {
    case LIQUID_CPFSK_SQUARE:
        q->ht_len = q->k;
        q->symbol_delay = 1;
        // modify integrator
        b[0] = 0.0f;
        b[1] = 1.0f;
        break;
    case LIQUID_CPFSK_RCOS_FULL:
        q->ht_len = q->k;
        q->symbol_delay = 1;
        break;
    case LIQUID_CPFSK_RCOS_PARTIAL:
        // TODO: adjust response based on 'm'
        q->ht_len = 3*q->k;
        q->symbol_delay = 2;
        break;
    case LIQUID_CPFSK_GMSK:
        q->symbol_delay = q->m + 1;
        q->ht_len = 2*(q->k)*(q->m) + (q->k) + 1;
        break;
    default:
        return liquid_error_config("cpfskmodem_create(), invalid filter type '%d'", q->type);
    }

    // create pulse-shaping filter and scale by modulation index
    q->ht = (float*) malloc(q->ht_len *sizeof(float));
    cpfskmod_firdes(q->k, q->m, q->beta, q->type, q->ht, q->ht_len);
    for (i=0; i<q->ht_len; i++)
        q->ht[i] *= M_PI * q->h;
    q->interp = firinterp_rrrf_create(q->k, q->ht, q->ht_len);

    // create phase integrator
    q->phase_interp = (float*) malloc(q->k*sizeof(float));
    q->integrator = iirfilt_rrrf_create(b,2,a,2);

    // reset modem object
    cpfskmod_reset(q);

    return q;
}

// destroy cpfskmod object
int cpfskmod_destroy(cpfskmod _q)
{
    // destroy pulse-shaping filter/interpolator
    free(_q->ht);
    free(_q->phase_interp);
    firinterp_rrrf_destroy(_q->interp);

    // destroy phase integrator
    iirfilt_rrrf_destroy(_q->integrator);

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

// print cpfskmod object internals
int cpfskmod_print(cpfskmod _q)
{
    printf("cpfskmod : continuous-phase frequency-shift keying modem\n");
    printf("    bits/symbol     :   %u\n", _q->bps);
    printf("    modulation index:   %-6.3f\n", _q->h);
    printf("    samples/symbol  :   %u\n", _q->k);
    printf("    filter delay    :   %u symbols\n", _q->m);
    printf("    filter roll-off :   %-6.3f\n", _q->beta);
    printf("    filter type     :   ");
    switch(_q->type) {
    case LIQUID_CPFSK_SQUARE:       printf("square\n");         break;
    case LIQUID_CPFSK_RCOS_FULL:    printf("rcos (full)\n");    break;
    case LIQUID_CPFSK_RCOS_PARTIAL: printf("rcos (partial)\n"); break;
    case LIQUID_CPFSK_GMSK:         printf("gmsk\n");           break;
    default:                        printf("unknown\n");        break;
    }
    printf("    filter          :\n");
    // print filter coefficients
    unsigned int i;
    for (i=0; i<_q->ht_len; i++)
        printf("        h(%3u) = %12.8f;\n", i+1, _q->ht[i]);
    return LIQUID_OK;
}

// reset state
int cpfskmod_reset(cpfskmod _q)
{
    // reset interpolator
    firinterp_rrrf_reset(_q->interp);

    // reset phase integrator
    iirfilt_rrrf_reset(_q->integrator);
    return LIQUID_OK;
}

// get transmit delay [symbols]
unsigned int cpfskmod_get_delay(cpfskmod _q)
{
    return _q->symbol_delay;
}

// modulate sample
//  _q      :   frequency modulator object
//  _s      :   input symbol
//  _y      :   output sample array [size: _k x 1]
int cpfskmod_modulate(cpfskmod        _q,
                      unsigned int    _s,
                      float complex * _y)
{
    // run interpolator
    float v = 2.0f*_s - (float)(_q->M) + 1.0f;
    firinterp_rrrf_execute(_q->interp, v, _q->phase_interp);

    // integrate phase state
    unsigned int i;
    float theta;
    for (i=0; i<_q->k; i++) {
        // push phase through integrator
        iirfilt_rrrf_execute(_q->integrator, _q->phase_interp[i], &theta);

        // compute output
        _y[i] = liquid_cexpjf(theta);
    }
    return LIQUID_OK;
}

// 
// internal methods
//

// design transmit filter
int cpfskmod_firdes(unsigned int _k,
                    unsigned int _m,
                    float        _beta,
                    int          _type,
                    float *      _ht,
                    unsigned int _ht_len)
{
    unsigned int i;
    // create filter based on specified type
    switch(_type) {
    case LIQUID_CPFSK_SQUARE:
        // square pulse
        if (_ht_len != _k)
            return liquid_error(LIQUID_EICONFIG,"cpfskmodem_firdes(), invalid filter length (square)");
        for (i=0; i<_ht_len; i++)
            _ht[i] = 1.0f;
        break;
    case LIQUID_CPFSK_RCOS_FULL:
        // full-response raised-cosine pulse
        if (_ht_len != _k)
            return liquid_error(LIQUID_EICONFIG,"cpfskmodem_firdes(), invalid filter length (rcos)");
        for (i=0; i<_ht_len; i++)
            _ht[i] = 1.0f - cosf(2.0f*M_PI*i/(float)_ht_len);
        break;
    case LIQUID_CPFSK_RCOS_PARTIAL:
        // full-response raised-cosine pulse
        if (_ht_len != 3*_k)
            return liquid_error(LIQUID_EICONFIG,"cpfskmodem_firdes(), invalid filter length (rcos)");
        // initialize with zeros
        for (i=0; i<_ht_len; i++)
            _ht[i] = 0.0f;
        // adding raised-cosine pulse with half-symbol delay
        for (i=0; i<2*_k; i++)
            _ht[i+_k/2] = 1.0f - cosf(2.0f*M_PI*i/(float)(2*_k));
        break;
    case LIQUID_CPFSK_GMSK:
        // Gauss minimum-shift keying pulse
        if (_ht_len != 2*_k*_m + _k + 1)
            return liquid_error(LIQUID_EICONFIG,"cpfskmodem_firdes(), invalid filter length (gmsk)");
        // initialize with zeros
        for (i=0; i<_ht_len; i++)
            _ht[i] = 0.0f;
        // adding Gauss pulse with half-symbol delay
        liquid_firdes_gmsktx(_k,_m,_beta,0.0f,&_ht[_k/2]);
        break;
    default:
        return liquid_error(LIQUID_EINT,"cpfskmodem_firdes(), invalid filter type '%d'", _type);
    }

    // normalize pulse area to unity
    float ht_sum = 0.0f;
    for (i=0; i<_ht_len; i++)
        ht_sum += _ht[i];
    for (i=0; i<_ht_len; i++)
        _ht[i] *= 1.0f / ht_sum;

    return LIQUID_OK;
}


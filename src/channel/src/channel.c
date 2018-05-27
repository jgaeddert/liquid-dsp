/*
 * Copyright (c) 2007 - 2016 Joseph Gaeddert
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
// Generic channel
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// portable structured channel object
struct CHANNEL(_s) {
    // additive white Gauss noise
    int             enabled_awgn;       // AWGN enabled?
    T               gamma;              // channel gain
    T               nstd;               // noise standard deviation
    float           noise_floor_dB;     // noise floor
    float           SNRdB;              // signal-to-noise ratio [dB]

    // carrier offset
    int             enabled_carrier;    // carrier offset enabled?
    float           dphi;               // channel gain
    float           phi;                // noise standard deviation
    NCO()           nco;                // oscillator

    // multi-path channel
    int             enabled_multipath;  // enable multi-path channel filter?
    FIRFILT()       channel_filter;     // multi-path channel filter object
    TC *            h;                  // multi-path channel filter coefficients
    unsigned int    h_len;              // multi-path channel filter length

    // shadowing channel
    int             enabled_shadowing;  // enable shadowing?
    IIRFILT()       shadowing_filter;   // shadowing filter object
    float           shadowing_std;      // shadowing standard deviation
    float           shadowing_fd;       // shadowing Doppler frequency
};

// create structured channel object with default parameters
CHANNEL() CHANNEL(_create)(void)
{
    CHANNEL() q = (CHANNEL()) malloc(sizeof(struct CHANNEL(_s)));

    // initialize all options as off
    q->enabled_awgn      = 0;
    q->enabled_carrier   = 0;
    q->enabled_multipath = 0;
    q->enabled_shadowing = 0;

    // create internal objects
    q->nco              = NCO(_create)(LIQUID_VCO);
    q->h_len            = 1;
    q->h                = (TC*) malloc(q->h_len*sizeof(TC));
    q->h[0]             = 1.0f;
    q->channel_filter   = FIRFILT(_create)(q->h, q->h_len);
    q->shadowing_filter = NULL;

    // return object
    return q;
}

// destroy channel object
void CHANNEL(_destroy)(CHANNEL() _q)
{
    // destroy internal objects
    NCO(_destroy)(_q->nco);
    FIRFILT(_destroy)(_q->channel_filter);
    if (_q->shadowing_filter != NULL)
        IIRFILT(_destroy)(_q->shadowing_filter);
    free(_q->h);

    // free main object memory
    free(_q);
}

// print channel object
void CHANNEL(_print)(CHANNEL() _q)
{
    printf("channel\n");
    if (_q->enabled_awgn)       printf("  AWGN:      SNR=%.3f dB, gamma=%.3f, std=%.6f\n", _q->SNRdB, _q->gamma, _q->nstd);
    if (_q->enabled_carrier)    printf("  carrier:   dphi=%.3f, phi=%.3f\n", _q->dphi, _q->phi);
    if (_q->enabled_multipath)  printf("  multipath: h_len=%u\n", _q->h_len);
    if (_q->enabled_shadowing)  printf("  shadowing: std=%.3fdB, fd=%.3f\n", _q->shadowing_std, _q->shadowing_fd);
}

// apply additive white Gausss noise impairment
//  _q              : channel object
//  _noise_floor_dB : noise floor power spectral density
//  _SNR_dB         : signal-to-noise ratio [dB]
void CHANNEL(_add_awgn)(CHANNEL() _q,
                        float     _noise_floor_dB,
                        float     _SNRdB)
{
    // enable module
    _q->enabled_awgn = 1;

    //
    _q->noise_floor_dB = _noise_floor_dB;
    _q->SNRdB          = _SNRdB;

    // set values appropriately
    _q->nstd  = powf(10.0f, _noise_floor_dB/20.0f);
    _q->gamma = powf(10.0f, (_q->SNRdB+_q->noise_floor_dB)/20.0f);
}

// apply carrier offset impairment
//  _q          : channel object
//  _frequency  : carrier frequency offse [radians/sample
//  _phase      : carrier phase offset    [radians]
void CHANNEL(_add_carrier_offset)(CHANNEL() _q,
                                  float     _frequency,
                                  float     _phase)
{
    // enable module
    _q->enabled_carrier = 1;

    // carrier frequency/phase offsets
    _q->dphi = _frequency;
    _q->phi  = _phase;

    // set values appropriately
    NCO(_set_frequency)(_q->nco, _q->dphi);
    NCO(_set_phase)    (_q->nco, _q->phi);
}

// apply multi-path channel impairment
//  _q          : channel object
//  _h          : channel coefficients (NULL for random)
//  _h_len      : number of channel coefficients
void CHANNEL(_add_multipath)(CHANNEL()    _q,
                             TC *         _h,
                             unsigned int _h_len)
{
    if (_h_len == 0) {
        fprintf(stderr,"warning: channel_%s_add_multipath(), filter length is zero (ignoring)\n", EXTENSION_FULL);
        return;
    } else if (_h_len > 1000) {
        fprintf(stderr,"warning: channel_%s_add_multipath(), filter length exceeds maximum\n", EXTENSION_FULL);
        exit(1);
    }

    // enable module
    _q->enabled_multipath = 1;

    // set values appropriately
    // TODO: test for types other than float complex
    if (_q->h_len != _h_len)
        _q->h = (TC*) realloc(_q->h, _h_len*sizeof(TC));

    // update length
    _q->h_len = _h_len;
    
    // copy coefficients internally
    if (_h == NULL) {
        // generate random coefficients using m-sequence generator
        // TODO: support types other than float
        _q->h[0] = 1.0f;
        unsigned int i;
        msequence ms = msequence_create_default(14);
        for (i=1; i<_q->h_len; i++) {
            float vi = msequence_generate_symbol(ms, 8) / 256.0f - 0.5f;
            float vq = msequence_generate_symbol(ms, 8) / 256.0f - 0.5f;
            _q->h[i] = (vi + _Complex_I*vq) * 0.5f;
        }
        msequence_destroy(ms);
    } else {
        // copy coefficients internally
        memmove(_q->h, _h, _q->h_len*sizeof(TC));
    }

    // re-create channel filter
    _q->channel_filter = FIRFILT(_recreate)(_q->channel_filter, _q->h, _q->h_len);
}

// apply slowly-varying shadowing impairment
//  _q          : channel object
//  _sigma      : std. deviation for log-normal shadowing
//  _fd         : Doppler frequency, _fd in (0,0.5)
void CHANNEL(_add_shadowing)(CHANNEL() _q,
                             float     _sigma,
                             float     _fd)
{
    if (_q->enabled_shadowing) {
        fprintf(stderr,"warning: channel_%s_add_shadowing(), shadowing already enabled\n", EXTENSION_FULL);
        return;
    } else if (_sigma <= 0) {
        fprintf(stderr,"warning: channel_%s_add_shadowing(), standard deviation less than or equal to zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_fd <= 0 || _fd >= 0.5) {
        fprintf(stderr,"warning: channel_%s_add_shadowing(), Doppler frequency must be in (0,0.5)\n", EXTENSION_FULL);
        exit(1);
    }

    // enable module
    _q->enabled_shadowing = 1;

    // TODO: set values appropriately
    _q->shadowing_std = _sigma;
    _q->shadowing_fd  = _fd;

    // re-create channel filter
    // TODO: adjust gain
    //_q->shadowing_filter = IIRFILT(_create_lowpass)(11, _q->shadowing_fd);
    float alpha = _q->shadowing_fd;
    float a[2] = {1.0f, alpha-1.0f};
    float b[2] = {alpha, 0};
    _q->shadowing_filter = IIRFILT(_create)(b,2,a,2);
}

// apply channel impairments on single input sample
//  _q      : channel object
//  _x      : input sample
//  _y      : output sample pointer
void CHANNEL(_execute)(CHANNEL() _q,
                       TI        _x,
                       TO *      _y)
{
    float complex r;
    // apply filter
    if (_q->enabled_multipath) {
        FIRFILT(_push)(   _q->channel_filter, _x);
        FIRFILT(_execute)(_q->channel_filter, &r);
    } else {
        r = _x;
    }

    // apply shadowing if enabled
    if (_q->enabled_shadowing) {
        // TODO: use type-specific value other than float
        float g = 0;
        IIRFILT(_execute)(_q->shadowing_filter, randnf()*_q->shadowing_std, &g);
        g /= _q->shadowing_fd * 6.9f;
        g = powf(10.0f, g/20.0f);
        r *= g;
    }

    // apply carrier if enabled
    if (_q->enabled_carrier) {
        NCO(_mix_up)(_q->nco, r, &r);
        NCO(_step)  (_q->nco);
    }

    // apply AWGN if enabled
    if (_q->enabled_awgn) {
        r *= _q->gamma;
        r += _q->nstd * ( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;
    }

    // set output value
    *_y = r;
}

// apply channel impairments on single input sample
//  _q      : channel object
//  _x      : input array [size: _n x 1]
//  _n      : input array length
//  _y      : output array [size: _n x 1]
void CHANNEL(_execute_block)(CHANNEL()    _q,
                             TI *         _x,
                             unsigned int _n,
                             TO *         _y)
{
    // apply channel effects on each input sample
    // TODO: apply in blocks
    unsigned int i;
    for (i=0; i<_n; i++)
        CHANNEL(_execute)(_q, _x[i], &_y[i]);
}


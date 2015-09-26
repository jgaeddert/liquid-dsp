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
// continuous phase frequency-shift keying demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// 
// internal methods
//

// initialize coherent demodulator
void cpfskdem_init_coherent(cpfskdem _q);

// initialize non-coherent demodulator
void cpfskdem_init_noncoherent(cpfskdem _q);

// demodulate array of samples (coherent)
void cpfskdem_demodulate_coherent(cpfskdem        _q,
                                  float complex   _y,
                                  unsigned int  * _s,
                                  unsigned int  * _nw);

// demodulate array of samples (non-coherent)
void cpfskdem_demodulate_noncoherent(cpfskdem        _q,
                                     float complex   _y,
                                     unsigned int  * _s,
                                     unsigned int  * _nw);

// cpfskdem
struct cpfskdem_s {
    // common
    unsigned int bps;           // bits per symbol
    unsigned int k;             // samples per symbol
    unsigned int m;             // filter delay (symbols)
    float        beta;          // filter bandwidth parameter
    float        h;             // modulation index
    int          type;          // filter type (e.g. LIQUID_CPFSK_GMSK)

    // constellation size
    unsigned int M;

    // demodulator type
    enum {
        CPFSKDEM_COHERENT=0,    // coherent demodulator
        CPFSKDEM_NONCOHERENT    // non-coherent demodulator
    } demod_type;

    // demodulation function pointer
    void (*demodulate)(cpfskdem        _q,
                       float complex   _y,
                       unsigned int  * _s,
                       unsigned int  * _nw);

    // common data structure shared between coherent and non-coherent
    // demodulator receivers
    union {
        // coherent demodulator
        struct {
            nco_crcf nco;       // oscillator/phase-locked loop
            firpfb_crcf mf;     // matched filter
            firpfb_crcf dmf;    // matched filter (derivative)
        } coherent;

        // non-coherent demodulator
        struct {
            firpfb_rrrf mf;     // matched filter
            firpfb_rrrf dmf;    // matched filter (derivative)
            //eqlms_rrrf equalizer;
        } noncoherent;
    } data;
};

// create cpfskdem object (frequency modulator)
//  _bps    :   bits per symbol, _bps > 0
//  _h      :   modulation index, _h > 0
//  _k      :   samples/symbol, _k > 1, _k even
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   filter bandwidth parameter, _beta > 0
//  _type   :   filter type (e.g. LIQUID_CPFSK_SQUARE)
cpfskdem cpfskdem_create(unsigned int _bps,
                         float        _h,
                         unsigned int _k,
                         unsigned int _m,
                         float        _beta,
                         int          _type)
{
    // validate input
    if (_bps == 0) {
        fprintf(stderr,"error: cpfskdem_create(), bits/symbol must be greater than 0\n");
        exit(1);
    } else if (_k < 2 || (_k%2)) {
        fprintf(stderr,"error: cpfskmod_create(), samples/symbol must be greater than 2 and even\n");
    } else if (_m == 0) {
        fprintf(stderr,"error: cpfskdem_create(), filter delay must be greater than 0\n");
        exit(1);
    } else if (_beta <= 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: cpfskdem_create(), filter roll-off must be in (0,1]\n");
        exit(1);
    } else if (_h <= 0.0f) {
        fprintf(stderr,"error: cpfskdem_create(), modulation index must be greater than 0\n");
        exit(1);
    }

    // create main object memory
    cpfskdem q = (cpfskdem) malloc(sizeof(struct cpfskdem_s));

    // set basic internal properties
    q->bps  = _bps;     // bits per symbol
    q->h    = _h;       // modulation index
    q->k    = _k;       // samples per symbol
    q->m    = _m;       // filter delay (symbols)
    q->beta = _beta;    // filter roll-off factor (only for certain filters)
    q->type = _type;    // filter type

    // derived values
    q->M = 1 << q->bps; // constellation size

    // coherent or non-coherent?
    if (q->h > 0.66667f) {
        cpfskdem_init_noncoherent(q);
    } else {
        cpfskdem_init_coherent(q);
    }

    // reset modem object
    cpfskdem_reset(q);

    return q;
}

// initialize coherent demodulator
void cpfskdem_init_coherent(cpfskdem _q)
{
    // specify coherent receiver
    _q->demod_type = CPFSKDEM_COHERENT;

    // set demodulate function pointer
    _q->demodulate = cpfskdem_demodulate_coherent;

    // create object depending upon input type
    switch(_q->type) {
    case LIQUID_CPFSK_SQUARE:
    case LIQUID_CPFSK_RCOS_FULL:
    case LIQUID_CPFSK_RCOS_PARTIAL:
    case LIQUID_CPFSK_GMSK:
        break;
    }

}

// initialize non-coherent demodulator
void cpfskdem_init_noncoherent(cpfskdem _q)
{
    // specify non-coherent receiver
    _q->demod_type = CPFSKDEM_NONCOHERENT;
    
    // set demodulate function pointer
    _q->demodulate = cpfskdem_demodulate_noncoherent;

    // create object depending upon input type
    switch(_q->type) {
    case LIQUID_CPFSK_SQUARE:
    case LIQUID_CPFSK_RCOS_FULL:
    case LIQUID_CPFSK_RCOS_PARTIAL:
    case LIQUID_CPFSK_GMSK:
        break;
    }

}

// destroy modem object
void cpfskdem_destroy(cpfskdem _q)
{
    switch(_q->demod_type) {
    case CPFSKDEM_COHERENT:
        break;
    case CPFSKDEM_NONCOHERENT:
        break;
    }

    // free main object memory
    free(_q);
}

// print modulation internals
void cpfskdem_print(cpfskdem _q)
{
    printf("cpfskdem:\n");
    printf("    k   :   %u\n", _q->k);
}

// reset modem object
void cpfskdem_reset(cpfskdem _q)
{
    switch(_q->demod_type) {
    case CPFSKDEM_COHERENT:
        break;
    case CPFSKDEM_NONCOHERENT:
        break;
    }
}

// demodulate array of samples
//  _q      :   continuous-phase frequency demodulator object
//  _y      :   input sample array [size: _n x 1]
//  _n      :   input sample array length
//  _s      :   output symbol array
//  _nw     :   number of output symbols written
void cpfskdem_demodulate(cpfskdem        _q,
                         float complex * _y,
                         unsigned int    _n,
                         unsigned int  * _s,
                         unsigned int  * _nw)
{
    // iterate through each sample calling type-specific demodulation function
    unsigned int i;
    unsigned int num_written = 0;
    for (i=0; i<_n; i++) {
        unsigned int nw;
        _q->demodulate(_q, _y[i], &_s[num_written], &nw);

        // update number of symbols written
        num_written += nw;
    }

    // set output number of bits written
    *_nw = num_written;
}

// demodulate array of samples (coherent)
void cpfskdem_demodulate_coherent(cpfskdem        _q,
                                  float complex   _y,
                                  unsigned int  * _s,
                                  unsigned int  * _nw)
{
    *_nw = 0;
}

// demodulate array of samples (non-coherent)
void cpfskdem_demodulate_noncoherent(cpfskdem        _q,
                                     float complex   _y,
                                     unsigned int  * _s,
                                     unsigned int  * _nw)
{
    *_nw = 0;
}


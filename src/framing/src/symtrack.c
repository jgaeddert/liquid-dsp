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
// Symbol tracker/synchronizer
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_SYMTRACK           0
#define DEBUG_SYMTRACK_PRINT     0
#define DEBUG_SYMTRACK_FILENAME  "symtrack_internal_debug.m"
#define DEBUG_BUFFER_LEN        (1024)

//
// forward declaration of internal methods
//

// internal structure
struct SYMTRACK(_s) {
    // parameters
    int             filter_type;        // filter type (e.g. LIQUID_RNYQUIST_RKAISER)
    unsigned int    k;                  // samples/symbol
    unsigned int    m;                  // filter semi-length
    float           beta;               // filter excess bandwidth
    int             mod_scheme;         // demodulator

    // automatic gain control
    AGC()           agc;                // agc object
    float           agc_bandwidth;      // agc bandwidth

    // symbol timing recovery
    SYMSYNC()       symsync;            // symbol timing recovery object
    float           symsync_bandwidth;  // symsync loop bandwidth
    TO              symsync_buf[8];     // symsync output buffer
    unsigned int    symsync_index;      // symsync output sample index

    // equalizer/decimator
    EQLMS()         eq;                 // equalizer (LMS)
    unsigned int    eq_len;             // equalizer length
    float           eq_bandwidth;       // equalizer bandwidth

    // nco/phase-locked loop
    NCO()           nco;                // nco (carrier recovery)
    float           pll_bandwidth;      // phase-locked loop bandwidth

    // demodulator
    MODEM()         demod;              // linear modem demodulator

    // state and counters
    unsigned int    num_syms_rx;        // number of symbols recovered
};

// create symtrack object with basic parameters
//  _ftype  : filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _k      : samples per symbol
//  _m      : filter delay (symbols)
//  _beta   : filter excess bandwidth
//  _ms     : modulation scheme (e.g. LIQUID_MODEM_QPSK)
SYMTRACK() SYMTRACK(_create)(int          _ftype,
                             unsigned int _k,
                             unsigned int _m,
                             float        _beta,
                             int          _ms)
{
    // validate input
    if (_m == 0) {
        fprintf(stderr,"error: symtrack_%s_create(), filter delay must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta <= 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: symtrack_%s_create(), filter excess bandwidth must be in (0,1]\n", EXTENSION_FULL);
        exit(1);
    } else if (_ms == LIQUID_MODEM_UNKNOWN || _ms >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"error: symtrack_%s_create(), invalid modulation scheme\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate memory for main object
    SYMTRACK() q = (SYMTRACK()) malloc( sizeof(struct SYMTRACK(_s)) );

    // set input parameters
    q->filter_type = _ftype;
    q->k           = _k;
    q->m           = _m;
    q->beta        = _beta;
    q->mod_scheme  = _ms == LIQUID_MODEM_UNKNOWN ? LIQUID_MODEM_BPSK : _ms;

    // create automatic gain control
    q->agc = AGC(_create)();
    
    // create symbol synchronizer (output rate: 2 samples per symbol)
    if (q->filter_type == LIQUID_FIRFILT_UNKNOWN)
        q->symsync = SYMSYNC(_create_kaiser)(q->k, q->m, 0.9f, 16);
    else
        q->symsync = SYMSYNC(_create_rnyquist)(q->filter_type, q->k, q->m, q->beta, 16);
    SYMSYNC(_set_output_rate)(q->symsync, 2);

    // create equalizer as default low-pass filter with integer symbol delay (2 samples/symbol)
    q->eq_len = 2 * 4 + 1;
    q->eq = EQLMS(_create_lowpass)(q->eq_len,0.45f);

    // nco and phase-locked loop
    q->nco = NCO(_create)(LIQUID_VCO);

    // demodulator
    q->demod = MODEM(_create)(q->mod_scheme);

    // set default bandwidth
    SYMTRACK(_set_bandwidth)(q, 0.1f);

    // reset and return main object
    SYMTRACK(_reset)(q);
    return q;
}

// create symtrack object using default parameters
SYMTRACK() SYMTRACK(_create_default)()
{
    return SYMTRACK(_create)(LIQUID_FIRFILT_ARKAISER,
                             2,     // samples/symbol
                             7,     // filter delay
                             0.3f,  // filter excess bandwidth
                             LIQUID_MODEM_QPSK);
}


// destroy symtrack object, freeing all internal memory
void SYMTRACK(_destroy)(SYMTRACK() _q)
{
    // destroy objects
    AGC    (_destroy)(_q->agc);
    SYMSYNC(_destroy)(_q->symsync);
    EQLMS  (_destroy)(_q->eq);
    NCO    (_destroy)(_q->nco);
    MODEM  (_destroy)(_q->demod);

    // free main object
    free(_q);
}

// print symtrack object's parameters
void SYMTRACK(_print)(SYMTRACK() _q)
{
    printf("symtrack_%s:\n", EXTENSION_FULL);
}

// reset symtrack internal state
void SYMTRACK(_reset)(SYMTRACK() _q)
{
    // reset objects

    // reset internal counters
    _q->symsync_index = 0;
    _q->num_syms_rx = 0;
}

// set symtrack modulation scheme
void SYMTRACK(_set_modscheme)(SYMTRACK() _q,
                              int        _ms)
{
    // validate input
    if (_ms == LIQUID_MODEM_UNKNOWN || _ms >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"error: symtrack_%s_set_modscheme(), invalid/unsupported modulation scheme\n", EXTENSION_FULL);
        exit(1);
    }

    // set internal modulation scheme
    _q->mod_scheme = _ms;

    // re-create modem
    _q->demod = MODEM(_recreate)(_q->demod, _q->mod_scheme);
}

// set symtrack internal bandwidth
void SYMTRACK(_set_bandwidth)(SYMTRACK() _q,
                              float      _bw)
{
    // validate input
    if (_bw < 0) {
        fprintf(stderr,"error: symtrack_%s_set_bandwidth(), bandwidth must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // set bandwidths accordingly
    // TODO: set bandwidths based on input bandwidth
    float agc_bandwidth     = 0.02f;
    float symsync_bandwidth = 0.001f;
    float eq_bandwidth      = 0.02f;
    float pll_bandwidth     = 0.001f;

    // automatic gain control
    AGC(_set_bandwidth)(_q->agc, agc_bandwidth);

    // symbol timing recovery
    SYMSYNC(_set_lf_bw)(_q->symsync, symsync_bandwidth);

    // equalizer
    EQLMS(_set_bw)(_q->eq, eq_bandwidth);
    
    // phase-locked loop
    NCO(_pll_set_bandwidth)(_q->nco, pll_bandwidth);
}

// adjust internal nco by requested phase
void SYMTRACK(_adjust_phase)(SYMTRACK() _q,
                             T          _dphi)
{
    // adjust internal nco phase
    NCO(_adjust_phase)(_q->nco, _dphi);
}

// execute synchronizer on single input sample
//  _q      : synchronizer object
//  _x      : input data sample
//  _y      : output data array
//  _ny     : number of samples written to output buffer
void SYMTRACK(_execute)(SYMTRACK()     _q,
                        TI             _x,
                        TO *           _y,
                        unsigned int * _ny)
{
    TO v;   // output sample
    unsigned int i;
    unsigned int num_outputs = 0;

    // run sample through automatic gain control
    AGC(_execute)(_q->agc, _x, &v);

    // symbol synchronizer
    unsigned int nw = 0;
    SYMSYNC(_execute)(_q->symsync, &v, 1, _q->symsync_buf, &nw);

    // process each output sample
    for (i=0; i<nw; i++) {
        // update phase-locked loop
        NCO(_step)(_q->nco);
        nco_crcf_mix_down(_q->nco, _q->symsync_buf[i], &v);

        // equalizer/decimator
        EQLMS(_push)(_q->eq, v);

        // decimate result, noting that symsync outputs at exactly 2 samples/symbol
        _q->symsync_index++;
        if ( !(_q->symsync_index % 2) )
            continue;

        // increment number of symbols received
        _q->num_syms_rx++;

        // compute equalizer output
        TO d_hat;
        EQLMS(_execute)(_q->eq, &d_hat);

        // demodulate result, apply phase correction
        unsigned int sym_out;
        MODEM(_demodulate)(_q->demod, d_hat, &sym_out);
        float phase_error = MODEM(_get_demodulator_phase_error)(_q->demod);

        // update equalizer independent of the signal: estimate error
        // assuming constant modulus signal
        // TODO: use decision-directed feedback when modulation scheme is known
        // TODO: check lock conditions of previous object to determine when to run equalizer
        if (_q->num_syms_rx > 200)
            EQLMS(_step)(_q->eq, d_hat/cabsf(d_hat), d_hat);

        // update pll
        NCO(_pll_step)(_q->nco, phase_error);

        // save result to output
        _y[num_outputs++] = d_hat;
    }

#if DEBUG_SYMTRACK
    printf("symsync wrote %u samples, %u outputs\n", nw, num_outputs);
#endif

    //
    *_ny = num_outputs;
}

// execute synchronizer on input data array
//  _q      : synchronizer object
//  _x      : input data array
//  _nx     : number of input samples
//  _y      : output data array
//  _ny     : number of samples written to output buffer
void SYMTRACK(_execute_block)(SYMTRACK()     _q,
                              TI *           _x,
                              unsigned int   _nx,
                              TO *           _y,
                              unsigned int * _ny)
{
    //
    unsigned int i;
    unsigned int num_written = 0;

    //
    for (i=0; i<_nx; i++) {
        unsigned int nw = 0;
        SYMTRACK(_execute)(_q, _x[i], &_y[num_written], &nw);

        num_written += nw;
    }

    //
    *_ny = num_written;
}


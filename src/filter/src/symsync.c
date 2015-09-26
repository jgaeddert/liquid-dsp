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
// Symbol synchronizer
//
// References:
//  [Mengali:1997] Umberto Mengali and Aldo N. D'Andrea,
//      "Synchronization Techniques for Digital Receivers,"
//      Plenum Press, New York & London, 1997.
//  [harris:2001] frederic j. harris and Michael Rice,
//      "Multirate Digital Filters for Symbol Timing Synchronization
//      in Software Defined Radios," IEEE Journal on Selected Areas
//      of Communications, vol. 19, no. 12, December, 2001, pp.
//      2346-2357.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_SYMSYNC           0
#define DEBUG_SYMSYNC_PRINT     0
#define DEBUG_SYMSYNC_FILENAME  "symsync_internal_debug.m"
#define DEBUG_BUFFER_LEN        (1024)

// 
// forward declaration of internal methods
//

// step synchronizer
//  _q      : symsync object
//  _x      : input sample
//  _y      : output sample array pointer
//  _ny     : number of output samples written
void SYMSYNC(_step)(SYMSYNC()      _q,
                    TI             _x,
                    TO *           _y,
                    unsigned int * _ny);

// advance synchronizer's internal loop filter
//  _q      : synchronizer object
//  _mf     : matched-filter output
//  _dmf    : derivative matched-filter output
void SYMSYNC(_advance_internal_loop)(SYMSYNC() _q,
                                     TO        _mf,
                                     TO        _dmf);

// print results to output debugging file
//  _q          : synchronizer object
//  _filename   : output filename
void SYMSYNC(_output_debug_file)(SYMSYNC()    _q,
                                 const char * _filename);

// internal structure
struct SYMSYNC(_s) {
    unsigned int h_len;         // matched filter length
    unsigned int k;             // samples/symbol (input)
    unsigned int k_out;         // samples/symbol (output)

    unsigned int decim_counter; // decimation counter
    int is_locked;              // synchronizer locked flag

    float rate;                 // internal resampling rate
    float del;                  // fractional delay step

    // floating-point timing phase
    float tau;                  // accumulated timing phase (0 <= tau <= 1)
    float tau_decim;            // timing phase, retained for get_tau() method
    float bf;                   // soft filterbank index
    int   b;                    // filterbank index

    // loop filter
    float q;                    // instantaneous timing error
    float q_hat;                // filtered timing error
    float B[3];                 // loop filter feed-forward coefficients
    float A[3];                 // loop filter feed-back coefficients
    iirfiltsos_rrrf pll;        // loop filter object (iir filter)
    float rate_adjustment;      // internal rate adjustment factor

    unsigned int npfb;          // number of filters in the bank
    FIRPFB()      mf;           // matched filter
    FIRPFB()     dmf;           // derivative matched filter

#if DEBUG_SYMSYNC
    windowf debug_rate;
    windowf debug_del;
    windowf debug_tau;
    windowf debug_bsoft;
    windowf debug_b;
    windowf debug_q_hat;
#endif
};

// create synchronizer object from external coefficients
//  _k      : samples per symbol
//  _M      : number of filters in the bank
//  _h      : matched filter coefficients
//  _h_len  : length of matched filter
SYMSYNC() SYMSYNC(_create)(unsigned int _k,
                           unsigned int _M,
                           TC *         _h,
                           unsigned int _h_len)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: symsync_%s_create(), input sample rate must be at least 2\n", EXTENSION_FULL);
        exit(1);
    } else if (_h_len == 0) {
        fprintf(stderr,"error: symsync_%s_create(), filter length must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if ( (_h_len-1) % _M ) {
        fprintf(stderr,"error: symsync_%s_create(), filter length must be of the form: h_len = m*_k*_M + 1 \n", EXTENSION_FULL);
        exit(1);
    } else if (_M == 0) {
        fprintf(stderr,"error: symsync_%s_create(), number of filter banks must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // create main object
    SYMSYNC() q = (SYMSYNC()) malloc(sizeof(struct SYMSYNC(_s)));

    // set internal properties
    q->k    = _k;  // input samples per symbol
    q->npfb = _M;  // number of filters in the polyphase filter bank

    // set output rate (nominally 1, full decimation)
    SYMSYNC(_set_output_rate)(q, 1);

    // set internal sub-filter length
    q->h_len = (_h_len-1)/q->npfb;
    
    // compute derivative filter
    TC dh[_h_len];
    float hdh_max = 0.0f;
    unsigned int i;
    for (i=0; i<_h_len; i++) {
        if (i==0) {
            dh[i] = _h[i+1] - _h[_h_len-1];
        } else if (i==_h_len-1) {
            dh[i] = _h[0]   - _h[i-1];
        } else {
            dh[i] = _h[i+1] - _h[i-1];
        }

        // find maximum of h*dh
        if ( fabsf(_h[i]*dh[i]) > hdh_max || i==0 )
            hdh_max = fabsf(_h[i]*dh[i]);
    }

    // apply scaling factor for normalized response
    for (i=0; i<_h_len; i++)
        dh[i] *= 0.06f / hdh_max;
    
    q->mf  = FIRPFB(_create)(q->npfb, _h, _h_len);
    q->dmf = FIRPFB(_create)(q->npfb, dh, _h_len);

    // reset state and initialize loop filter
    q->A[0] = 1.0f;     q->B[0] = 0.0f;
    q->A[1] = 0.0f;     q->B[1] = 0.0f;
    q->A[2] = 0.0f;     q->B[2] = 0.0f;
    q->pll = iirfiltsos_rrrf_create(q->B, q->A);
    SYMSYNC(_reset)(q);
    SYMSYNC(_set_lf_bw)(q, 0.01f);

    // set output rate nominally at 1 sample/symbol (full decimation)
    SYMSYNC(_set_output_rate)(q, 1);

    // unlock loop control
    SYMSYNC(_unlock)(q);

#if DEBUG_SYMSYNC
    q->debug_rate  = windowf_create(DEBUG_BUFFER_LEN);
    q->debug_del   = windowf_create(DEBUG_BUFFER_LEN);
    q->debug_tau   = windowf_create(DEBUG_BUFFER_LEN);
    q->debug_bsoft = windowf_create(DEBUG_BUFFER_LEN);
    q->debug_b     = windowf_create(DEBUG_BUFFER_LEN);
    q->debug_q_hat = windowf_create(DEBUG_BUFFER_LEN);
#endif

    // return main object
    return q;
}

// create square-root Nyquist symbol synchronizer
//  _type   : filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _M      : number of filters in the bank
SYMSYNC() SYMSYNC(_create_rnyquist)(int          _type,
                                    unsigned int _k,
                                    unsigned int _m,
                                    float        _beta,
                                    unsigned int _M)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: symsync_%s_create_rnyquist(), samples/symbol must be at least 2\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: symsync_%s_create_rnyquist(), filter delay (m) must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: symsync_%s_create_rnyquist(), filter excess bandwidth must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate memory for filter coefficients
    unsigned int H_len = 2*_M*_k*_m + 1;
    float Hf[H_len];

    // design square-root Nyquist pulse-shaping filter
    liquid_firdes_prototype(_type, _k*_M, _m, _beta, 0, Hf);

    // copy coefficients to type-specific array
    TC H[H_len];
    unsigned int i;
    for (i=0; i<H_len; i++)
        H[i] = Hf[i];

    // create object and return
    return SYMSYNC(_create)(_k, _M, H, H_len);
}

// create symsync using Kaiser filter interpolator; useful
// when the input signal has matched filter applied already
//  _k      : input samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor, beta in (0,1]
//  _M      : number of filters in the bank
SYMSYNC() SYMSYNC(_create_kaiser)(unsigned int _k,
                                  unsigned int _m,
                                  float        _beta,
                                  unsigned int _M)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: symsync_%s_create_kaiser(), samples/symbol must be at least 2\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: symsync_%s_create_kaiser(), filter delay (m) must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: symsync_%s_create_kaiser(), filter excess bandwidth must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate memory for filter coefficients
    unsigned int H_len = 2*_M*_k*_m + 1;
    float Hf[H_len];

    // design interpolating filter whose bandwidth is outside the cut-off
    // frequency of input signal
    // TODO: use _beta to compute more accurate cut-off frequency
    float fc = 0.75f;   // filter cut-off frequency (nominal)
    float As = 40.0f;   // filter stop-band attenuation
    liquid_firdes_kaiser(H_len, fc / (float)(_k*_M), As, 0.0f, Hf);

    // copy coefficients to type-specific array, adjusting to relative
    // filter gain
    unsigned int i;
    TC H[H_len];
    for (i=0; i<H_len; i++)
        H[i] = Hf[i] * 2.0f * fc;

    // create object and return
    return SYMSYNC(_create)(_k, _M, H, H_len);
}

// destroy symsync object, freeing all internal memory
void SYMSYNC(_destroy)(SYMSYNC() _q)
{
#if DEBUG_SYMSYNC
    // output debugging file
    SYMSYNC(_output_debug_file)(_q, DEBUG_SYMSYNC_FILENAME);

    // destroy internal window objects
    windowf_destroy(_q->debug_rate);
    windowf_destroy(_q->debug_del);
    windowf_destroy(_q->debug_tau);
    windowf_destroy(_q->debug_bsoft);
    windowf_destroy(_q->debug_b);
    windowf_destroy(_q->debug_q_hat);
#endif

    // destroy filterbank objects
    FIRPFB(_destroy)(_q->mf);
    FIRPFB(_destroy)(_q->dmf);

    // destroy timing phase-locked loop filter
    iirfiltsos_rrrf_destroy(_q->pll);

    // free main object memory
    free(_q);
}

// print symsync object's parameters
void SYMSYNC(_print)(SYMSYNC() _q)
{
    printf("symsync_%s [rate: %f]\n", EXTENSION_FULL, _q->rate);
    FIRPFB(_print)(_q->mf);
}

// reset symsync internal state
void SYMSYNC(_reset)(SYMSYNC() _q)
{
    // reset polyphase filterbank
    FIRPFB(_reset)(_q->mf);

    // reset counters, etc.
    _q->rate          = (float)_q->k / (float)_q->k_out;
    _q->del           = _q->rate;
    _q->b             =   0;    // filterbank index
    _q->bf            = 0.0f;   // filterbank index (soft value)
    _q->tau           = 0.0f;   // instantaneous timing estimate
    _q->tau_decim     = 0.0f;   // instantaneous timing estaimte (decimated)
    _q->q             = 0.0f;   // timing error
    _q->q_hat         = 0.0f;   // filtered timing error
    _q->decim_counter = 0;      // decimated output counter

    // reset timing phase-locked loop filter
    iirfiltsos_rrrf_reset(_q->pll);
}

// lock synchronizer object
void SYMSYNC(_lock)(SYMSYNC() _q)
{
    _q->is_locked = 1;
}

// unlock synchronizer object
void SYMSYNC(_unlock)(SYMSYNC() _q)
{
    _q->is_locked = 0;
}

// set synchronizer output rate (samples/symbol)
//  _q      :   synchronizer object
//  _k_out  :   output samples/symbol
void SYMSYNC(_set_output_rate)(SYMSYNC()    _q,
                               unsigned int _k_out)
{
    // validate input
    if (_k_out == 0) {
        fprintf(stderr,"error: symsync_%s_output_rate(), output rate must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    }

    // set output rate
    _q->k_out = _k_out;

    _q->rate = (float)_q->k / (float)_q->k_out;
    _q->del  = _q->rate;
}

// set synchronizer loop filter bandwidth
//  _q      :   synchronizer object
//  _bt     :   loop bandwidth
void SYMSYNC(_set_lf_bw)(SYMSYNC() _q,
                         float     _bt)
{
    // validate input
    if (_bt < 0.0f || _bt > 1.0f) {
        fprintf(stderr,"error: symsync_%s_set_lf_bt(), bandwidth must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // compute filter coefficients from bandwidth
    float alpha = 1.000f - _bt;
    float beta  = 0.220f * _bt;
    float a     = 0.500f;
    float b     = 0.495f;

    _q->B[0] = beta;
    _q->B[1] = 0.00f;
    _q->B[2] = 0.00f;

    _q->A[0] = 1.00f - a*alpha;
    _q->A[1] = -b*alpha;
    _q->A[2] = 0.00f;

    // set internal parameters of 2nd-order IIR filter
    iirfiltsos_rrrf_set_coefficients(_q->pll, _q->B, _q->A);
    
    // update rate adjustment factor
    _q->rate_adjustment = 0.5*_bt;
}

// return instantaneous fractional timing offset estimate
float SYMSYNC(_get_tau)(SYMSYNC() _q)
{
    return _q->tau_decim;
}

// execute synchronizer on input data array
//  _q      : synchronizer object
//  _x      : input data array
//  _nx     : number of input samples
//  _y      : output data array
//  _ny     : number of samples written to output buffer
void SYMSYNC(_execute)(SYMSYNC()      _q,
                       TI *           _x,
                       unsigned int   _nx,
                       TO *           _y,
                       unsigned int * _ny)
{
    unsigned int i, ny=0, k=0;
    for (i=0; i<_nx; i++) {
        SYMSYNC(_step)(_q, _x[i], &_y[ny], &k);
        ny += k;
        //printf("%u\n",k);
    }
    *_ny = ny;
}

//
// internal methods
//

// step synchronizer with single input sample
//  _q      : symsync object
//  _x      : input sample
//  _y      : output sample array pointer
//  _ny     : number of output samples written
void SYMSYNC(_step)(SYMSYNC()      _q,
                    TI             _x,
                    TO *           _y,
                    unsigned int * _ny)
{
    // push sample into MF and dMF filterbanks
    FIRPFB(_push)(_q->mf,  _x);
    FIRPFB(_push)(_q->dmf, _x);
    
    // matched and derivative matched-filter outputs
    TO  mf; // matched filter output
    TO dmf; // derivative matched filter output

    unsigned int n=0;
    
    // continue loop until filterbank index rolls over
    while (_q->b < _q->npfb) {

#if DEBUG_SYMSYNC_PRINT
        printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n, _q->tau, _q->b, _q->bf);
#endif

        // compute filterbank output
        FIRPFB(_execute)(_q->mf, _q->b, &mf);

        // scale output by samples/symbol
        _y[n] = mf / (float)(_q->k);

        // check output count and determine if this is 'ideal' timing output
        if (_q->decim_counter == _q->k_out) {
            // reset counter
            _q->decim_counter = 0;

#if DEBUG_SYMSYNC
            // save debugging variables
            windowf_push(_q->debug_rate,   _q->rate);
            windowf_push(_q->debug_del,    _q->del);
            windowf_push(_q->debug_tau,    _q->tau);
            windowf_push(_q->debug_bsoft,  _q->bf);
            windowf_push(_q->debug_b,      _q->b);
            windowf_push(_q->debug_q_hat,  _q->q_hat);
#endif

            // if synchronizer is locked, don't update internal timing offset
            if (_q->is_locked)
                continue;

            // compute dMF output
            FIRPFB(_execute)(_q->dmf, _q->b, &dmf);
            
            // update internal state
            SYMSYNC(_advance_internal_loop)(_q, mf, dmf);
            _q->tau_decim = _q->tau;    // save return value
        }

        // increment decimation counter
        _q->decim_counter++;

        // update states
        _q->tau += _q->del;                     // instantaneous fractional offset
        _q->bf  = _q->tau * (float)(_q->npfb);  // filterbank index (soft)
        _q->b   = (int)roundf(_q->bf);          // filterbank index
        n++;                                    // number of output samples
    }

    // filterbank index rolled over; update states
    _q->tau -= 1.0f;                // instantaneous fractional offset
    _q->bf  -= (float)(_q->npfb);   // filterbank index (soft)
    _q->b   -= _q->npfb;            // filterbank index

    // set output number of samples written
    *_ny = n;
}

// advance synchronizer's internal loop filter
//  _q      : synchronizer object
//  _mf     : matched-filter output
//  _dmf    : derivative matched-filter output
void SYMSYNC(_advance_internal_loop)(SYMSYNC() _q,
                                     TO        _mf,
                                     TO        _dmf)
{
    //  1. compute timing error signal, clipping large levels
#if 0
    _q->q = crealf(_mf)*crealf(_dmf) + cimagf(_mf)*cimagf(_dmf);
#else
    // TODO : use more efficient method to compute this
    _q->q = crealf( conjf(_mf)*_dmf );  // [Mengali:1997] Eq.~(8.3.5)
#endif
    // constrain timing error
    if      (_q->q >  1.0f) _q->q =  1.0f;  // clip large positive values
    else if (_q->q < -1.0f) _q->q = -1.0f;  // clip large negative values

    //  2. filter error signal through timing loop filter: retain large
    //     portion of old estimate and small percent of new estimate
    iirfiltsos_rrrf_execute(_q->pll, _q->q, &_q->q_hat);

    // 3. update rate and timing phase
    _q->rate += _q->rate_adjustment * _q->q_hat;
    _q->del   = _q->rate + _q->q_hat;

#if DEBUG_SYMSYNC_PRINT
    printf("q : %12.8f, rate : %12.8f, del : %12.8f, q_hat : %12.8f\n", _q->q, _q->rate, _q->del, _q->q_hat);
#endif
}

// print results to output debugging file
//  _q          : synchronizer object
//  _filename   : output filename
void SYMSYNC(_output_debug_file)(SYMSYNC()    _q,
                                 const char * _filename)
{
    FILE * fid = fopen(_filename, "w");
    if (!fid) {
        fprintf(stderr,"error: symsync_%s_output_debug_file(), could not open '%s' for writing\n", EXTENSION_FULL, _filename);
        return;
    }
    fprintf(fid,"%% %s, auto-generated file\n\n", DEBUG_SYMSYNC_FILENAME);
    fprintf(fid,"\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    fprintf(fid,"npfb = %u;\n",_q->npfb);
    fprintf(fid,"k    = %u;\n",_q->k);
    fprintf(fid,"\n\n");

#if DEBUG_SYMSYNC
    fprintf(fid,"n = %u;\n", DEBUG_BUFFER_LEN);
    float * r;
    unsigned int i;

    // save filter responses
    FIRPFB(_reset)(_q->mf);
    FIRPFB(_reset)(_q->dmf);
    fprintf(fid,"h = [];\n");
    fprintf(fid,"dh = [];\n");
    fprintf(fid,"h_len = %u;\n", _q->h_len);
    for (i=0; i<_q->h_len; i++) {
        // push impulse
        if (i==0) {
            FIRPFB(_push)(_q->mf,  1.0f);
            FIRPFB(_push)(_q->dmf, 1.0f);
        } else {
            FIRPFB(_push)(_q->mf,  0.0f);
            FIRPFB(_push)(_q->dmf, 0.0f);
        }

        // compute output for all filters
        TO  mf;     // matched filter output
        TO dmf;     // derivative matched filter output

        unsigned int n;
        for (n=0; n<_q->npfb; n++) {
            FIRPFB(_execute)(_q->mf,  n, &mf);
            FIRPFB(_execute)(_q->dmf, n, &dmf);

            fprintf(fid,"h(%4u) = %12.8f; dh(%4u) = %12.8f;\n", i*_q->npfb+n+1, crealf(mf), i*_q->npfb+n+1, crealf(dmf));
        }
    }
    // plot response
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"th = [0:(h_len*npfb-1)]/(k*npfb) - h_len/(2*k);\n");
    fprintf(fid,"m  = abs(round(th(1)));\n");
    //fprintf(fid,"plot(t,h,t,dh);\n");
    fprintf(fid,"subplot(3,1,1),\n");
    fprintf(fid,"  plot(th, h, 'LineWidth', 2, 'Color', [0 0.5 0.2]);\n");
    fprintf(fid,"  ylabel('MF');\n");
    fprintf(fid,"  axis([-m m -0.25 1.25]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2),\n");
    fprintf(fid,"  plot(th, dh, 'LineWidth', 2, 'Color', [0 0.2 0.5]);\n");
    fprintf(fid,"  ylabel('dMF');\n");
    fprintf(fid,"  axis([-m m -0.10 0.10]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3),\n");
    fprintf(fid,"  plot(th,-h.*dh, 'LineWidth', 2, 'Color', [0.5 0 0]);\n");
    fprintf(fid,"  xlabel('Symbol Index');\n");
    fprintf(fid,"  ylabel('-MF*dMF');\n");
    fprintf(fid,"  axis([-m m -0.08 0.08]);\n");
    fprintf(fid,"  grid on;\n");

    // print rate buffer
    fprintf(fid,"rate = zeros(1,n);\n");
    windowf_read(_q->debug_rate, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"rate(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print del buffer
    fprintf(fid,"del = zeros(1,n);\n");
    windowf_read(_q->debug_del, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"del(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print tau buffer
    fprintf(fid,"tau = zeros(1,n);\n");
    windowf_read(_q->debug_tau, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"tau(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print bsoft buffer
    fprintf(fid,"bf = zeros(1,n);\n");
    windowf_read(_q->debug_bsoft, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"bf(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print b (filterbank index) buffer
    fprintf(fid,"b = zeros(1,n);\n");
    windowf_read(_q->debug_b, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"b(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    // print filtered error signal
    fprintf(fid,"q_hat = zeros(1,n);\n");
    windowf_read(_q->debug_q_hat, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"q_hat(%4u) = %12.8f;\n", i+1, r[i]);
    fprintf(fid,"\n\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"t=1:n;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,b,'Color',[0.5 0.5 0.5]);\n");
    fprintf(fid,"  plot(t,bf,'LineWidth',2,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([t(1) t(end) -1 npfb]);\n");
    fprintf(fid,"  legend('b','b (soft)',0);\n");
    fprintf(fid,"  xlabel('Symbol Index')\n");
    fprintf(fid,"  ylabel('Polyphase Filter Index')\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"  plot(t,rate,'LineWidth',2,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([t(1) t(end) 0.99 1.01]);\n");
    fprintf(fid,"  xlabel('Symbol Index')\n");
    fprintf(fid,"  ylabel('Rate')\n");
    fprintf(fid,"%% done.\n");
#endif

    fclose(fid);
    printf("symsync: internal results written to '%s'\n", _filename);
}



/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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
// Least mean-squares (LMS) equalizer
//

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define DEBUG

struct EQLMS(_s) {
    unsigned int h_len;     // filter length
    float        mu;        // LMS step size

    // internal matrices
    T *          h0;        // initial coefficients
    T *          w0;        // weights [px1]
    T *          w1;        // weights [px1]

    unsigned int count;     // input sample count
    int          buf_full;  // input buffer full flag
    WINDOW()     buffer;    // input buffer
    wdelayf      x2;        // buffer of |x|^2 values
    float        x2_sum;    // sum{ |x|^2 }
};

// update sum{|x|^2}
void EQLMS(_update_sumsq)(EQLMS() _q, T _x);

// create least mean-squares (LMS) equalizer object
//  _h      :   initial coefficients [size: _h_len x 1], default if NULL
//  _p      :   equalizer length (number of taps)
EQLMS() EQLMS(_create)(T *          _h,
                       unsigned int _h_len)
{
    EQLMS() q = (EQLMS()) malloc(sizeof(struct EQLMS(_s)));

    // set filter order, other params
    q->h_len = _h_len;
    q->mu    = 0.5f;

    q->h0 = (T*) malloc((q->h_len)*sizeof(T));
    q->w0 = (T*) malloc((q->h_len)*sizeof(T));
    q->w1 = (T*) malloc((q->h_len)*sizeof(T));
    q->buffer = WINDOW(_create)(q->h_len);
    q->x2     = wdelayf_create(q->h_len);

    // copy coefficients (if not NULL)
    if (_h == NULL) {
        // initial coefficients with delta at first index
        unsigned int i;
        for (i=0; i<q->h_len; i++)
            q->h0[i] = (i==0) ? 1.0 : 0.0;
    } else {
        // copy user-defined initial coefficients
        memmove(q->h0, _h, (q->h_len)*sizeof(T));
    }

    // reset equalizer object
    EQLMS(_reset)(q);

    // return main object
    return q;
}

// create square-root Nyquist interpolator
//  _type   :   filter type (e.g. LIQUID_FIRFILT_RRC)
//  _k      :   samples/symbol _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
//  _dt     :   fractional sample delay, 0 <= _dt < 1
EQLMS() EQLMS(_create_rnyquist)(int          _type,
                                unsigned int _k,
                                unsigned int _m,
                                float        _beta,
                                float        _dt)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: eqlms_%s_create_rnyquist(), samples/symbol must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: eqlms_%s_create_rnyquist(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: eqlms_%s_create_rnyquist(), filter excess bandwidth factor must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    } else if (_dt < -1.0f || _dt > 1.0f) {
        fprintf(stderr,"error: eqlms_%s_create_rnyquist(), filter fractional sample delay must be in [-1,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate square-root Nyquist filter
    unsigned int h_len = 2*_k*_m + 1;
    float h[h_len];
    liquid_firdes_prototype(_type,_k,_m,_beta,_dt,h);

    // copy coefficients to type-specific array (e.g. float complex)
    // and scale by samples/symbol
    unsigned int i;
    T hc[h_len];
    for (i=0; i<h_len; i++)
        hc[i] = h[i] / (float)_k;

    // return equalizer object
    return EQLMS(_create)(hc, h_len);
}

// create LMS EQ initialized with low-pass filter
//  _h_len  : filter length
//  _fc     : filter cut-off, _fc in (0,0.5]
EQLMS() EQLMS(_create_lowpass)(unsigned int _h_len,
                               float        _fc)
{
    // validate input
    if (_h_len == 0) {
        fprintf(stderr,"error: eqlms_%s_create_lowpass(), filter length must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_fc <= 0.0f || _fc > 0.5f) {
        fprintf(stderr,"error: eqlms_%s_create_rnyquist(), filter cutoff must be in (0,0.5]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate low-pass filter prototype
    float h[_h_len];
    liquid_firdes_kaiser(_h_len, _fc, 40.0f, 0.0f, h);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    T hc[_h_len];
    for (i=0; i<_h_len; i++)
        hc[i] = h[i];

    // return equalizer object
    return EQLMS(_create)(hc, _h_len);
}

// re-create least mean-squares (LMS) equalizer object
//  _q      :   old equalizer object
//  _h      :   initial coefficients [size: _p x 1], default if NULL
//  _p      :   equalizer length (number of taps)
EQLMS() EQLMS(_recreate)(EQLMS()      _q,
                         T *          _h,
                         unsigned int _p)
{
    if (_q->h_len == _p) {
        // length hasn't changed; copy default coefficients
        // and return object
        unsigned int i;
        for (i=0; i<_q->h_len; i++)
            _q->h0[i] = _h[i];
        return _q;
    }

    // completely destroy old equalizer object
    EQLMS(_destroy)(_q);

    // create new one and return
    return EQLMS(_create)(_h,_p);
}


// destroy eqlms object
void EQLMS(_destroy)(EQLMS() _q)
{
    free(_q->h0);
    free(_q->w0);
    free(_q->w1);

    WINDOW(_destroy)(_q->buffer);
    wdelayf_destroy(_q->x2);
    free(_q);
}

// reset equalizer
void EQLMS(_reset)(EQLMS() _q)
{
    // copy default coefficients
    memmove(_q->w0, _q->h0, (_q->h_len)*sizeof(T));

    WINDOW(_reset)(_q->buffer);
    wdelayf_reset(_q->x2);

    // reset input count
    _q->count = 0;
    _q->buf_full = 0;

    // reset squared magnitude sum
    _q->x2_sum = 0;
}

// print eqlms object internals
void EQLMS(_print)(EQLMS() _q)
{
    printf("equalizer (LMS):\n");
    printf("    order:      %u\n", _q->h_len);
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        printf("  h(%3u) = %12.4e + j*%12.4e;\n", i+1, creal(_q->w0[i]), cimag(_q->w0[i]));
}

// get learning rate of equalizer
float EQLMS(_get_bw)(EQLMS() _q)
{
    return _q->mu;
}

// set learning rate of equalizer
//  _q      :   equalizer object
//  _mu     :   LMS learning rate (should be near 0), 0 < _mu < 1
void EQLMS(_set_bw)(EQLMS() _q,
                    float   _mu)
{
    if (_mu < 0.0f) {
        fprintf(stderr,"error: eqlms_%s_set_bw(), learning rate cannot be less than zero\n", EXTENSION_FULL);
        exit(1);
    }

    _q->mu = _mu;
}

// push sample into equalizer internal buffer
//  _q      :   equalizer object
//  _x      :   received sample
void EQLMS(_push)(EQLMS() _q,
                  T _x)
{
    // push value into buffer
    WINDOW(_push)(_q->buffer, _x);

    // update sum{|x|^2}
    EQLMS(_update_sumsq)(_q, _x);

    // increment count
    _q->count++;
}

// push sample into equalizer internal buffer as block
//  _q      :   equalizer object
//  _x      :   input sample array
//  _n      :   input sample array length
void EQLMS(_push_block)(EQLMS()      _q,
                        T *          _x,
                        unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        EQLMS(_push)(_q, _x[i]);
}

// execute internal dot product
//  _q      :   equalizer object
//  _y      :   output sample
void EQLMS(_execute)(EQLMS() _q,
                     T *     _y)
{
    T y = 0;    // temporary accumulator
    T * r;      // read buffer
    WINDOW(_read)(_q->buffer, &r);

    // compute conjugate vector dot product
    //DOTPROD(_run)(_q->w0, r, _q->h_len, &y);
    unsigned int i;
    for (i=0; i<_q->h_len; i++) {
        T sum = conj(_q->w0[i])*r[i];
        y += sum;
    }

    // set output
    *_y = y;
}

// execute equalizer with block of samples using constant
// modulus algorithm, operating on a decimation rate of _k
// samples.
//  _q      :   equalizer object
//  _k      :   down-sampling rate
//  _x      :   input sample array [size: _n x 1]
//  _n      :   input sample array length
//  _y      :   output sample array [size: _n x 1]
void EQLMS(_execute_block)(EQLMS()      _q,
                           unsigned int _k,
                           T *          _x,
                           unsigned int _n,
                           T *          _y)
{
    if (_k == 0) {
        fprintf(stderr,"error: eqlms_%s_execute_block(), down-sampling rate 'k' must be greater than 0\n", EXTENSION_FULL);
        exit(-1);
    }

    unsigned int i;
    T d_hat;
    for (i=0; i<_n; i++) {
        // push input sample
        EQLMS(_push)(_q, _x[i]);

        // compute output sample
        EQLMS(_execute)(_q, &d_hat);

        // store result in output
        _y[i] = d_hat;

        // decimate by _k
        if ( ((_q->count+_k-1) % _k) == 0 ) {
            // update equalizer independent of the signal: estimate error
            // assuming constant modulus signal
            EQLMS(_step_blind)(_q, d_hat);
        }
    }
}

// step through one cycle of equalizer training
//  _q      :   equalizer object
//  _d      :   desired output
//  _d_hat  :   filtered output
void EQLMS(_step)(EQLMS() _q,
                  T       _d,
                  T       _d_hat)
{
    // check count; only run step when buffer is full
    if (!_q->buf_full) {
        if (_q->count < _q->h_len)
            return;
        else
            _q->buf_full = 1;
    }

    unsigned int i;

    // compute error (a priori)
    T alpha = _d - _d_hat;

    // read buffer
    T * r;      // read buffer
    WINDOW(_read)(_q->buffer, &r);

    // update weighting vector
    // w[n+1] = w[n] + mu*conj(d-d_hat)*x[n]/(x[n]' * conj(x[n]))
    for (i=0; i<_q->h_len; i++)
        _q->w1[i] = _q->w0[i] + (_q->mu)*conj(alpha)*r[i]/_q->x2_sum;

#ifdef DEBUG
    printf("w0: \n");
    for (i=0; i<_q->h_len; i++) {
        PRINTVAL(_q->w0[i]);
        printf("\n");
    }
    printf("w1: \n");
    for (i=0; i<_q->h_len; i++) {
        PRINTVAL(_q->w1[i]);
        printf("\n");
    }
#endif

    // copy old values
    memmove(_q->w0, _q->w1, _q->h_len*sizeof(T));
}

// step through one cycle of equalizer training
//  _q      :   equalizer object
//  _d_hat  :   filtered output
void EQLMS(_step_blind)(EQLMS() _q,
                        T       _d_hat)
{
    // update equalizer using constant modulus method
#if T_COMPLEX
    T d = _d_hat / cabsf(_d_hat);
#else
    T d = _d_hat > 0 ? 1 : -1;
#endif
    EQLMS(_step)(_q, d, _d_hat);
}

// retrieve internal filter coefficients
void EQLMS(_get_weights)(EQLMS() _q, T * _w)
{
    // copy output weight vector
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        _w[i] = conj(_q->w0[_q->h_len-i-1]);
}

// train equalizer object
//  _q      :   equalizer object
//  _w      :   initial weights / output weights
//  _x      :   received sample vector
//  _d      :   desired output vector
//  _n      :   vector length
void EQLMS(_train)(EQLMS()      _q,
                   T *          _w,
                   T *          _x,
                   T *          _d,
                   unsigned int _n)
{
    unsigned int p=_q->h_len;
    if (_n < _q->h_len) {
        fprintf(stderr,"warning: eqlms_%s_train(), traning sequence less than filter order\n", EXTENSION_FULL);
    }

    unsigned int i;

    // reset equalizer state
    EQLMS(_reset)(_q);

    // copy initial weights into buffer
    for (i=0; i<p; i++)
        _q->w0[i] = _w[p - i - 1];

    T d_hat;
    for (i=0; i<_n; i++) {
        // push sample into internal buffer
        EQLMS(_push)(_q, _x[i]);

        // execute vector dot product
        EQLMS(_execute)(_q, &d_hat);

        // step through training cycle
        EQLMS(_step)(_q, _d[i], d_hat);
    }

    // copy output weight vector
    EQLMS(_get_weights)(_q, _w);
}

// 
// internal methods
//

// update sum{|x|^2}
void EQLMS(_update_sumsq)(EQLMS() _q, T _x)
{
    // update estimate of signal magnitude squared
    // |x[n-1]|^2 (input sample)
    float x2_n = crealf(_x * conjf(_x));

    // |x[0]  |^2 (oldest sample)
    float x2_0;

    // push newest sample
    wdelayf_push(_q->x2, x2_n);

    // read oldest sample
    wdelayf_read(_q->x2, &x2_0);

    // update sum( |x|^2 ) of last 'n' input samples
    _q->x2_sum = _q->x2_sum + x2_n - x2_0;
}


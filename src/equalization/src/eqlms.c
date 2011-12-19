/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// Least mean-squares (LMS) equalizer
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define DEBUG

struct EQLMS(_s) {
    unsigned int p;     // filter order
    float mu;           // LMS step size

    // internal matrices
    T * h0;             // initial coefficients
    T * w0, * w1;       // weights [px1]

    unsigned int n;     // input counter
    WINDOW() buffer;    // input buffer
    windowf ex2_buffer; // input energy buffer
};

// create least mean-squares (LMS) equalizer object
//  _h      :   initial coefficients [size: _p x 1], default if NULL
//  _p      :   equalizer length (number of taps)
EQLMS() EQLMS(_create)(T * _h,
                       unsigned int _p)
{
    EQLMS() eq = (EQLMS()) malloc(sizeof(struct EQLMS(_s)));

    // set filter order, other params
    eq->p = _p;
    eq->mu = 0.5f;

    eq->h0 = (T*) malloc((eq->p)*sizeof(T));
    eq->w0 = (T*) malloc((eq->p)*sizeof(T));
    eq->w1 = (T*) malloc((eq->p)*sizeof(T));
    eq->buffer = WINDOW(_create)(eq->p);
    eq->ex2_buffer = windowf_create(eq->p);

    // copy coefficients (if not NULL)
    if (_h == NULL) {
        // initial coefficients with delta at first index
        unsigned int i;
        for (i=0; i<eq->p; i++)
            eq->h0[i] = (i==0) ? 1.0 : 0.0;
    } else {
        // copy user-defined initial coefficients
        memmove(eq->h0, _h, (eq->p)*sizeof(T));
    }

    // reset equalizer object
    EQLMS(_reset)(eq);

    return eq;
}

// create square-root Nyquist interpolator
//  _type   :   filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _k      :   samples/symbol _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
//  _dt     :   fractional sample delay, 0 <= _dt < 1
EQLMS() EQLMS(_create_rnyquist)(int _type,
                                unsigned int _k,
                                unsigned int _m,
                                float _beta,
                                float _dt)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: eqlms_xxxt_create_rnyquist(), samples/symbol must be greater than 1\n");
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: eqlms_xxxt_create_rnyquist(), filter delay must be greater than 0\n");
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: eqlms_xxxt_create_rnyquist(), filter excess bandwidth factor must be in [0,1]\n");
        exit(1);
    } else if (_dt < -1.0f || _dt > 1.0f) {
        fprintf(stderr,"error: eqlms_xxxt_create_rnyquist(), filter fractional sample delay must be in [-1,1]\n");
        exit(1);
    }

    // generate square-root Nyquist filter
    unsigned int h_len = 2*_k*_m + 1;
    float h[h_len];
    liquid_firdes_rnyquist(_type,_k,_m,_beta,_dt,h);

    // copy coefficients to type-specific array (e.g. float complex)
    // and scale by samples/symbol
    unsigned int i;
    T hc[h_len];
    for (i=0; i<h_len; i++)
        hc[i] = h[i] / (float)_k;

    // return equalizer object
    return EQLMS(_create)(hc, h_len);
}

// re-create least mean-squares (LMS) equalizer object
//  _eq     :   old equalizer object
//  _h      :   initial coefficients [size: _p x 1], default if NULL
//  _p      :   equalizer length (number of taps)
EQLMS() EQLMS(_recreate)(EQLMS() _eq,
                         T * _h,
                         unsigned int _p)
{
    if (_eq->p == _p) {
        // length hasn't changed; copy default coefficients
        // and return object
        unsigned int i;
        for (i=0; i<_eq->p; i++)
            _eq->h0[i] = _h[i];
        return _eq;
    }

    // completely destroy old equalizer object
    EQLMS(_destroy)(_eq);

    // create new one and return
    return EQLMS(_create)(_h,_p);
}


// destroy eqlms object
void EQLMS(_destroy)(EQLMS() _eq)
{
    free(_eq->h0);
    free(_eq->w0);
    free(_eq->w1);

    WINDOW(_destroy)(_eq->buffer);
    windowf_destroy(_eq->ex2_buffer);
    free(_eq);
}

// print eqlms object internals
void EQLMS(_print)(EQLMS() _eq)
{
    printf("equalizer (LMS):\n");
    printf("    order:      %u\n", _eq->p);
    unsigned int i;
    for (i=0; i<_eq->p; i++)
        printf("  h(%3u) = %12.4e + j*%12.4e;\n", i+1, creal(_eq->w0[i]), cimag(_eq->w0[i]));
}

// set learning rate of equalizer
//  _eq     :   equalizer object
//  _mu     :   LMS learning rate (should be near 0), 0 < _mu < 1
void EQLMS(_set_bw)(EQLMS() _eq,
                    float _mu)
{
    if (_mu < 0.0f) {
        fprintf(stderr,"error: eqlms_xxxt_set_bw(), learning rate cannot be less than zero\n");
        exit(1);
    }

    _eq->mu = _mu;
}

// get learning rate of equalizer
float EQLMS(_get_bw)(EQLMS() _eq)
{
    return _eq->mu;
}

// reset equalizer
void EQLMS(_reset)(EQLMS() _eq)
{
    // copy default coefficients
    memmove(_eq->w0, _eq->h0, (_eq->p)*sizeof(T));

    WINDOW(_clear)(_eq->buffer);
    windowf_clear(_eq->ex2_buffer);
    _eq->n=0;
}

// push sample into equalizer internal buffer
//  _eq     :   equalizer object
//  _x      :   received sample
void EQLMS(_push)(EQLMS() _eq,
                  T _x)
{
    // push value into buffer
    WINDOW(_push)(_eq->buffer, _x);

    // push |x|^2
    windowf_push(_eq->ex2_buffer, crealf(_x*conj(_x)));
}

// execute internal dot product
//  _eq     :   equalizer object
//  _y      :   output sample
void EQLMS(_execute)(EQLMS() _eq,
                     T * _y)
{
    T y = 0;    // temporary accumulator
    T * r;      // read buffer
    WINDOW(_read)(_eq->buffer, &r);

    // compute conjugate vector dot product
    //DOTPROD(_run)(_eq->w0, r, _eq->p, &y);
    unsigned int i;
    for (i=0; i<_eq->p; i++)
        y += conj(_eq->w0[i])*r[i];

    // set output
    *_y = y;
}

// step through one cycle of equalizer
//  _eq     :   equalizer object
//  _x      :   received sample
//  _d      :   desired output
//  _d_hat  :   filtered output
void EQLMS(_step)(EQLMS() _eq,
                  T _d,
                  T _d_hat)
{
    unsigned int i;
    unsigned int p=_eq->p;

    // compute error (a priori)
    T alpha = _d - _d_hat;

    // compute signal energy
    float *x2, ex2=0.0f;
    windowf_read(_eq->ex2_buffer, &x2);
    for (i=0; i<p; i++)
        ex2 += x2[i];

    // read buffer
    T * r;      // read buffer
    WINDOW(_read)(_eq->buffer, &r);

    // update weighting vector
    // w[n+1] = w[n] + mu*conj(d-d_hat)*x[n]/(x[n]' * conj(x[n]))
    for (i=0; i<p; i++)
        _eq->w1[i] = _eq->w0[i] + (_eq->mu)*conj(alpha)*r[i]/ex2;

#ifdef DEBUG
    printf("w0: \n");
    for (i=0; i<p; i++) {
        PRINTVAL(_eq->w0[i]);
        printf("\n");
    }
    printf("w1: \n");
    for (i=0; i<p; i++) {
        PRINTVAL(_eq->w1[i]);
        printf("\n");
    }
#endif

    // copy old values
    memmove(_eq->w0, _eq->w1,   p*sizeof(T));
}

// retrieve internal filter coefficients
void EQLMS(_get_weights)(EQLMS() _eq, T * _w)
{
    // copy output weight vector
    unsigned int i, p=_eq->p;
    for (i=0; i<p; i++)
        _w[i] = conj(_eq->w0[p-i-1]);
}

// train equalizer object
//  _eq     :   equalizer object
//  _w      :   initial weights / output weights
//  _x      :   received sample vector
//  _d      :   desired output vector
//  _n      :   vector length
void EQLMS(_train)(EQLMS() _eq,
                   T * _w,
                   T * _x,
                   T * _d,
                   unsigned int _n)
{
    unsigned int p=_eq->p;
    if (_n < _eq->p) {
        fprintf(stderr,"warning: eqlms_xxxt_train(), traning sequence less than filter order\n");
    }

    unsigned int i;

    // reset equalizer state
    EQLMS(_reset)(_eq);

    // copy initial weights into buffer
    for (i=0; i<p; i++)
        _eq->w0[i] = _w[p - i - 1];

    T d_hat;
    for (i=0; i<_n; i++) {
        // push sample into internal buffer
        EQLMS(_push)(_eq, _x[i]);

        // execute vector dot product
        EQLMS(_execute)(_eq, &d_hat);

        // step through training cycle
        EQLMS(_step)(_eq, _d[i], d_hat);
    }

    // copy output weight vector
    EQLMS(_get_weights)(_eq, _w);
}


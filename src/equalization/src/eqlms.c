/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
    T * w0, * w1;       // weights [px1]

    unsigned int n;     // input counter
    WINDOW() buffer;    // input buffer
    fwindow ex2_buffer; // input energy buffer
};

// create least mean-squares (LMS) equalizer object
//  _p      :   equalizer length (number of taps)
EQLMS() EQLMS(_create)(unsigned int _p)
{
    EQLMS() eq = (EQLMS()) malloc(sizeof(struct EQLMS(_s)));

    // set filter order, other params
    eq->p = _p;
    eq->mu = 0.5f;

    eq->w0 = (T*) malloc((eq->p)*sizeof(T));
    eq->w1 = (T*) malloc((eq->p)*sizeof(T));
    eq->buffer = WINDOW(_create)(eq->p);
    eq->ex2_buffer = fwindow_create(eq->p);

    EQLMS(_reset)(eq);

    return eq;
}

// destroy eqlms object
void EQLMS(_destroy)(EQLMS() _eq)
{
    free(_eq->w0);
    free(_eq->w1);

    WINDOW(_destroy)(_eq->buffer);
    fwindow_destroy(_eq->ex2_buffer);
    free(_eq);
}

// print eqlms object internals
void EQLMS(_print)(EQLMS() _eq)
{
    printf("equalizer (LMS):\n");
    printf("    order:      %u\n", _eq->p);
}

// set learning rate of equalizer
//  _eq     :   equalizer object
//  _mu     :   LMS learning rate (should be near 0), 0 < _mu < 1
void EQLMS(_set_bw)(EQLMS() _eq,
                    float _mu)
{
    if (_mu < 0.0f) {
        printf("error: eqlms_xxxt_set_bw(), learning rate must be positive\n");
        exit(1);
    }

    _eq->mu = _mu;
}

// reset equalizer
void EQLMS(_reset)(EQLMS() _eq)
{
    unsigned int i;
    for (i=0; i<_eq->p; i++)
        _eq->w0[i] = 0;

    WINDOW(_clear)(_eq->buffer);
    fwindow_clear(_eq->ex2_buffer);
    _eq->n=0;
}

// execute cycle of equalizer, filtering output
//  _eq     :   equalizer object
//  _x      :   received sample
//  _d      :   desired output
//  _d_hat  :   filtered output
void EQLMS(_execute)(EQLMS() _eq,
                     T _x,
                     T _d,
                     T * _d_hat)
{
    unsigned int i;
    unsigned int p=_eq->p;

    // push value into buffer
    WINDOW(_push)(_eq->buffer, _x);
    T * x;

    // push |x|^2
    fwindow_push(_eq->ex2_buffer, crealf(_x*conj(_x)));

    // check to see if buffer is full, return if not
    _eq->n++;
    if (_eq->n < _eq->p) {
        *_d_hat = 0;
        return;
    }

    // compute d_hat (dot product, estimated output)
    T d_hat = 0;
    WINDOW(_read)(_eq->buffer, &x);
    //DOTPROD(_run)(_eq->w0, x, p, &d_hat);
    for (i=0; i<p; i++)
        d_hat += conj(_eq->w0[i])*x[i];
    *_d_hat = d_hat;

    // compute error (a priori)
    T alpha = _d - d_hat;

    // compute signal energy
    float *x2, ex2=0.0f;
    fwindow_read(_eq->ex2_buffer, &x2);
    for (i=0; i<p; i++)
        ex2 += x2[i];

    // update weighting vector
    // w[n+1] = w[n] + mu*conj(d-d_hat)*x[n]/(x[n]' * conj(x[n]))
    for (i=0; i<p; i++)
        _eq->w1[i] = _eq->w0[i] + (_eq->mu)*conj(alpha)*x[i]/ex2;

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
        _w[i] = conj(_eq->w1[p-i-1]);
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
    unsigned int i, p=_eq->p;
    if (_n < p) {
        printf("warning: eqlms_xxxt_train(), traning sequence less than filter order\n");
        return;
    }

    // reset equalizer state
    EQLMS(_reset)(_eq);

    // copy initial weights into buffer
    for (i=0; i<p; i++)
        _eq->w0[i] = _w[p - i - 1];

    T d_hat;
    for (i=0; i<_n; i++)
        EQLMS(_execute)(_eq, _x[i], _d[i], &d_hat);

    // copy output weight vector
    EQLMS(_get_weights)(_eq, _w);
}


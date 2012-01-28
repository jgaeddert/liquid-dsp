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
// Recursive least-squares (RLS) equalizer
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define DEBUG

struct EQRLS(_s) {
    unsigned int p;     // filter order
    float lambda;       // RLS forgetting factor
    float delta;        // RLS initialization factor

    // internal matrices
    T * h0;             // initial coefficients
    T * w0, * w1;       // weights [px1]
    T * P0, * P1;       // recursion matrix [pxp]
    T * g;              // gain vector [px1]

    // temporary matrices
    T * xP0;            // [1xp]
    T zeta;             // constant

    T * gxl;            // [pxp]
    T * gxlP0;          // [pxp]

    unsigned int n;     // input counter
    WINDOW() buffer;    // input buffer
};


// create recursive least-squares (RLS) equalizer object
//  _h      :   initial coefficients [size: _p x 1], default if NULL
//  _p      :   equalizer length (number of taps)
EQRLS() EQRLS(_create)(T * _h,
                       unsigned int _p)
{
    EQRLS() eq = (EQRLS()) malloc(sizeof(struct EQRLS(_s)));

    // set filter order, other parameters
    eq->p = _p;
    eq->lambda = 0.99f;
    eq->delta = 0.1f;
    eq->n=0;

    // allocate memory for matrices
    eq->h0 =    (T*) malloc((eq->p)*sizeof(T));
    eq->w0 =    (T*) malloc((eq->p)*sizeof(T));
    eq->w1 =    (T*) malloc((eq->p)*sizeof(T));
    eq->P0 =    (T*) malloc((eq->p)*(eq->p)*sizeof(T));
    eq->P1 =    (T*) malloc((eq->p)*(eq->p)*sizeof(T));
    eq->g =     (T*) malloc((eq->p)*sizeof(T));

    eq->xP0 =   (T*) malloc((eq->p)*sizeof(T));
    eq->gxl =   (T*) malloc((eq->p)*(eq->p)*sizeof(T));
    eq->gxlP0 = (T*) malloc((eq->p)*(eq->p)*sizeof(T));

    eq->buffer = WINDOW(_create)(eq->p);

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

    EQRLS(_reset)(eq);

    return eq;
}


// re-create recursive least-squares (RLS) equalizer object
//  _eq     :   old equalizer object
//  _h      :   initial coefficients [size: _p x 1], default if NULL
//  _p      :   equalizer length (number of taps)
EQRLS() EQRLS(_recreate)(EQRLS() _eq,
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
    EQRLS(_destroy)(_eq);

    // create new one and return
    return EQRLS(_create)(_h,_p);
}

// destroy eqrls object
void EQRLS(_destroy)(EQRLS() _eq)
{
    free(_eq->h0);
    free(_eq->w0);
    free(_eq->w1);
    free(_eq->P0);
    free(_eq->P1);
    free(_eq->g);

    free(_eq->xP0);
    free(_eq->gxl);
    free(_eq->gxlP0);

    WINDOW(_destroy)(_eq->buffer);
    free(_eq);
}

// print eqrls object internals
void EQRLS(_print)(EQRLS() _eq)
{
    printf("equalizer (RLS):\n");
    printf("    order:      %u\n", _eq->p);

#ifdef DEBUG
    unsigned int r,c,p=_eq->p;
    printf("P0:\n");
    for (r=0; r<p; r++) {
        for (c=0; c<p; c++) {
            PRINTVAL(matrix_access(_eq->P0,p,r,c));
        }
        printf("\n");
    }

    printf("P1:\n");
    for (r=0; r<p; r++) {
        for (c=0; c<p; c++) {
            PRINTVAL(matrix_access(_eq->P1,p,p,r,c));
        }
        printf("\n");
    }

    printf("gxl:\n");
    for (r=0; r<p; r++) {
        for (c=0; c<p; c++) {
            PRINTVAL(matrix_access(_eq->gxl,p,p,r,c));
        }
        printf("\n");
    }
#endif
}

// set learning rate of equalizer
//  _eq     :   equalizer object
//  _lambda :   RLS learning rate (should be close to 1.0), 0 < _lambda < 1
void EQRLS(_set_bw)(EQRLS() _eq,
                    float _lambda)
{
    if (_lambda < 0.0f || _lambda > 1.0f) {
        printf("error: eqrls_%s_set_bw(), learning rate must be in (0,1)\n", EXTENSION_FULL);
        exit(1);
    }

    _eq->lambda = _lambda;
}

// get learning rate of equalizer
float EQRLS(_get_bw)(EQRLS() _eq)
{
    return _eq->lambda;
}

// reset equalizer
void EQRLS(_reset)(EQRLS() _eq)
{
    unsigned int i, j;
    // initialize...
    for (i=0; i<_eq->p; i++) {
        for (j=0; j<_eq->p; j++) {
            if (i==j)   _eq->P0[(_eq->p)*i + j] = 1 / (_eq->delta);
            else        _eq->P0[(_eq->p)*i + j] = 0;
        }
    }

    // copy default coefficients
    memmove(_eq->w0, _eq->h0, (_eq->p)*sizeof(T));

    WINDOW(_clear)(_eq->buffer);
}

// push sample into equalizer internal buffer
//  _eq     :   equalizer object
//  _x      :   received sample
void EQRLS(_push)(EQRLS() _eq,
                  T _x)
{
    // push value into buffer
    WINDOW(_push)(_eq->buffer, _x);
}

// execute internal dot product
//  _eq     :   equalizer object
//  _y      :   output sample
void EQRLS(_execute)(EQRLS() _eq,
                     T * _y)
{
    // compute vector dot product
    T * r;      // read buffer
    WINDOW(_read)(_eq->buffer, &r);
    DOTPROD(_run)(_eq->w0, r, _eq->p, _y);
}

// execute cycle of equalizer, filtering output
//  _eq     :   equalizer object
//  _x      :   received sample
//  _d      :   desired output
//  _d_hat  :   filtered output
void EQRLS(_step)(EQRLS() _eq,
                 T _d,
                 T _d_hat)
{
    unsigned int i,r,c;
    unsigned int p=_eq->p;

    // compute error (a priori)
    T alpha = _d - _d_hat;

    // read buffer
    T * x;
    WINDOW(_read)(_eq->buffer, &x);

    // compute gain vector
    for (c=0; c<p; c++) {
        _eq->xP0[c] = 0;
        for (r=0; r<p; r++) {
            _eq->xP0[c] += x[r] * matrix_access(_eq->P0,p,p,r,c);
        }
    }

#ifdef DEBUG
    printf("x: ");
    for (i=0; i<p; i++)
        PRINTVAL(x[i]);
    printf("\n");

    DEBUG_PRINTF_CFLOAT(stdout,"    d",0,_d);
    DEBUG_PRINTF_CFLOAT(stdout,"d_hat",0,d_hat);
    DEBUG_PRINTF_CFLOAT(stdout,"error",0,alpha);

    printf("xP0: ");
    for (c=0; c<p; c++)
        PRINTVAL(_eq->xP0[c]);
    printf("\n");
#endif
    // zeta = lambda + [x.']*[P0]*[conj(x)]
    _eq->zeta = 0;
    for (c=0; c<p; c++)
        _eq->zeta += _eq->xP0[c] * conj(x[c]);
    _eq->zeta += _eq->lambda;
#ifdef DEBUG
    printf("zeta : ");
    PRINTVAL(_eq->zeta);
    printf("\n");
#endif
    for (r=0; r<p; r++) {
        _eq->g[r] = 0;
        for (c=0; c<p; c++) {
            _eq->g[r] += matrix_access(_eq->P0,p,p,r,c) * conj(x[c]);
        }
        _eq->g[r] /= _eq->zeta;
    }
#ifdef DEBUG
    printf("g: ");
    for (i=0; i<p; i++)
        PRINTVAL(_eq->g[i]);
        //printf("%6.3f ", _eq->g[i]);
    printf("\n");
#endif

    // update recursion matrix
    for (r=0; r<p; r++) {
        for (c=0; c<p; c++) {
            // gxl = [g] * [x.'] / lambda
            matrix_access(_eq->gxl,p,p,r,c) = _eq->g[r] * x[c] / _eq->lambda;
        }
    }
    // multiply two [pxp] matrices: gxlP0 = gxl * P0
    MATRIX(_mul)(_eq->gxl,  p,p,
                 _eq->P0,   p,p,
                 _eq->gxlP0,p,p);

    for (i=0; i<p*p; i++)
        _eq->P1[i] = _eq->P0[i] / _eq->lambda - _eq->gxlP0[i];

    // update weighting vector
    for (i=0; i<p; i++)
        _eq->w1[i] = _eq->w0[i] + alpha*(_eq->g[i]);

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
    EQRLS(_print)(_eq);
    }
    //if (_eq->n == 7)
    //    exit(0);

#endif

    // copy old values
    memmove(_eq->w0, _eq->w1,   p*sizeof(T));
    memmove(_eq->P0, _eq->P1, p*p*sizeof(T));

}

// retrieve internal filter coefficients
void EQRLS(_get_weights)(EQRLS() _eq, T * _w)
{
    // copy output weight vector
    unsigned int i, p=_eq->p;
    for (i=0; i<p; i++)
        _w[i] = _eq->w1[p-i-1];
}

// train equalizer object
//  _eq     :   equalizer object
//  _w      :   initial weights / output weights
//  _x      :   received sample vector
//  _d      :   desired output vector
//  _n      :   vector length
void EQRLS(_train)(EQRLS() _eq,
                   T * _w,
                   T * _x,
                   T * _d,
                   unsigned int _n)
{
    unsigned int i, p=_eq->p;
    if (_n < p) {
        printf("warning: eqrls_%s_train(), traning sequence less than filter order\n", EXTENSION_FULL);
        return;
    }

    // reset equalizer state
    EQRLS(_reset)(_eq);

    // copy initial weights into buffer
    for (i=0; i<p; i++)
        _eq->w0[i] = _w[p - i - 1];

    T d_hat;
    for (i=0; i<_n; i++) {
        // push sample into internal buffer
        EQRLS(_push)(_eq, _x[i]);

        // execute vector dot product
        EQRLS(_execute)(_eq, &d_hat);

        // step through training cycle
        EQRLS(_step)(_eq, _d[i], d_hat);
    }

    // copy output weight vector
    EQRLS(_get_weights)(_eq, _w);
}

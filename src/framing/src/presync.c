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
// Binary pre-demod synchronizer
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.internal.h"

struct PRESYNC(_s) {
    unsigned int n;     // sequence length
    unsigned int m;     // number of binary synchronizers
    
    WINDOW() rx_i;      // received pattern (in-phase)
    WINDOW() rx_q;      // received pattern (quadrature)
    
    float * dphi;       // array of frequency offsets [size: m x 1]
    DOTPROD() * sync_i; // synchronization pattern (in-phase)
    DOTPROD() * sync_q; // synchronization pattern (quadrature)

    float * rxy;        // output correlation [size: m x 1]

    float n_inv;        // 1/n (pre-computed for speed)
};

/* create binary pre-demod synchronizer                     */
/*  _v          :   baseband sequence                       */
/*  _n          :   baseband sequence length                */
/*  _dphi_max   :   maximum absolute frequency deviation    */
/*  _m          :   number of correlators                   */
PRESYNC() PRESYNC(_create)(TC *         _v,
                           unsigned int _n,
                           float        _dphi_max,
                           unsigned int _m)
{
    // validate input
    if (_n < 1) {
        fprintf(stderr, "error: bpresync_%s_create(), invalid input length\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr, "error: bpresync_%s_create(), number of correlators must be at least 1\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate main object memory and initialize
    PRESYNC() _q = (PRESYNC()) malloc(sizeof(struct PRESYNC(_s)));
    _q->n = _n;
    _q->m = _m;

    _q->n_inv = 1.0f / (float)(_q->n);

    unsigned int i;

    // create internal receive buffers
    _q->rx_i = WINDOW(_create)(_q->n);
    _q->rx_q = WINDOW(_create)(_q->n);

    // create internal array of frequency offsets
    _q->dphi = (float*) malloc( _q->m*sizeof(float) );

    // create internal synchronizers
    _q->sync_i = (DOTPROD()*) malloc( _q->m*sizeof(DOTPROD()) );
    _q->sync_q = (DOTPROD()*) malloc( _q->m*sizeof(DOTPROD()) );

    // buffer
    T vi_prime[_n];
    T vq_prime[_n];
    for (i=0; i<_q->m; i++) {

        // generate signal with frequency offset
        _q->dphi[i] = (float)i / (float)(_q->m-1)*_dphi_max;
        unsigned int k;
        for (k=0; k<_q->n; k++) {
            vi_prime[k] = REAL( _v[k] * cexpf(-_Complex_I*k*_q->dphi[i]) );
            vq_prime[k] = IMAG( _v[k] * cexpf(-_Complex_I*k*_q->dphi[i]) );
        }

        _q->sync_i[i] = DOTPROD(_create)(vi_prime, _q->n);
        _q->sync_q[i] = DOTPROD(_create)(vq_prime, _q->n);
    }

    // allocate memory for cross-correlation
    _q->rxy = (float*) malloc( _q->m*sizeof(float) );

    // reset object
    PRESYNC(_reset)(_q);

    return _q;
}

void PRESYNC(_destroy)(PRESYNC() _q)
{
    unsigned int i;

    // free received symbol buffers
    WINDOW(_destroy)(_q->rx_i);
    WINDOW(_destroy)(_q->rx_q);

    // free internal syncrhonizer objects
    for (i=0; i<_q->m; i++) {
        DOTPROD(_destroy)(_q->sync_i[i]);
        DOTPROD(_destroy)(_q->sync_q[i]);
    }
    free(_q->sync_i);
    free(_q->sync_q);

    // free internal frequency offset array
    free(_q->dphi);

    // free internal cross-correlation array
    free(_q->rxy);

    // free main object memory
    free(_q);
}

void PRESYNC(_print)(PRESYNC() _q)
{
    printf("bpresync_%s: %u samples\n", EXTENSION_FULL, _q->n);
}

void PRESYNC(_reset)(PRESYNC() _q)
{
    WINDOW(_clear)(_q->rx_i);
    WINDOW(_clear)(_q->rx_q);
}

// correlate input sequence with particular 
//  _q          :   pre-demod synchronizer object
//  _id         :   ...
void PRESYNC(_correlatex)(PRESYNC()       _q,
                          unsigned int    _id,
                          float complex * _rxy0,
                          float complex * _rxy1)
{
    // validate input...
    if (_id >= _q->m) {
        fprintf(stderr,"error: bpresync_%s_correlatex(), invalid id\n", EXTENSION_FULL);
        exit(1);
    }

    // get buffer pointers
    T * ri = NULL;
    T * rq = NULL;
    WINDOW(_read)(_q->rx_i, &ri);
    WINDOW(_read)(_q->rx_q, &rq);

    // compute correlations
    T rxy_ii;   DOTPROD(_execute)(_q->sync_i[_id], ri, &rxy_ii);
    T rxy_qq;   DOTPROD(_execute)(_q->sync_q[_id], rq, &rxy_qq);
    T rxy_iq;   DOTPROD(_execute)(_q->sync_i[_id], rq, &rxy_iq);
    T rxy_qi;   DOTPROD(_execute)(_q->sync_q[_id], ri, &rxy_qi);

    // non-conjugated
    T rxy_i0 = rxy_ii - rxy_qq;
    T rxy_q0 = rxy_iq + rxy_qi;
    *_rxy0 = (rxy_i0 + rxy_q0 * _Complex_I) * _q->n_inv;

    // conjugated
    T rxy_i1 = rxy_ii + rxy_qq;
    T rxy_q1 = rxy_iq - rxy_qi;
    *_rxy1 = (rxy_i1 + rxy_q1 * _Complex_I) * _q->n_inv;
}

/* push input sample into pre-demod synchronizer            */
/*  _q          :   pre-demod synchronizer object           */
/*  _x          :   input sample                            */
void PRESYNC(_push)(PRESYNC() _q,
                    TI        _x)
{
    // push symbol into buffers
    WINDOW(_push)(_q->rx_i, REAL(_x));
    WINDOW(_push)(_q->rx_q, REAL(_x));
}

/* correlate input sequence                                 */
/*  _q          :   pre-demod synchronizer object           */
/*  _rxy        :   output cross correlation                */
/*  _dphi_hat   :   output frequency offset estimate        */
void PRESYNC(_correlate)(PRESYNC() _q,
                         TO *      _rxy,
                         float *   _dphi_hat)
{
    unsigned int i;
    float complex rxy_max = 0;  // maximum cross-correlation
    float abs_rxy_max = 0;      // absolute value of rxy_max
    float complex rxy0;
    float complex rxy1;
    float dphi_hat = 0.0f;
    for (i=0; i<_q->m; i++)  {

        PRESYNC(_correlatex)(_q, i, &rxy0, &rxy1);

        // check non-conjugated value
        if ( ABS(rxy0) > abs_rxy_max ) {
            rxy_max     = rxy0;
            abs_rxy_max = ABS(rxy0);
            dphi_hat    = _q->dphi[i];
        }

        // check conjugated value
        if ( ABS(rxy1) > abs_rxy_max ) {
            rxy_max     = rxy1;
            abs_rxy_max = ABS(rxy1);
            dphi_hat    = -_q->dphi[i];
        }
    }

    *_rxy      = rxy_max;
    *_dphi_hat = dphi_hat;
}


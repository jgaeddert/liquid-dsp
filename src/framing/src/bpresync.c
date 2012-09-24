/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
// Binary pre-demod synchronizer
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.internal.h"

struct BPRESYNC(_s) {
    unsigned int n;     // sequence length
    unsigned int m;     // number of binary synchronizers

    float * dphi;       // array of frequency offsets [size: m x 1]
    BSYNC() * sync;     // binary synchronizers [size: m x 1]
};

/* create binary pre-demod synchronizer                     */
/*  _v          :   baseband sequence                       */
/*  _n          :   baseband sequence length                */
/*  _dphi_max   :   maximum absolute frequency deviation    */
/*  _m          :   number of correlators                   */
BPRESYNC() BPRESYNC(_create)(TC *         _v,
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
    BPRESYNC() _q = (BPRESYNC()) malloc(sizeof(struct BPRESYNC(_s)));
    _q->n = _n;
    _q->m = _m;

    unsigned int i;

    // create internal array of frequency offsets
    _q->dphi = (float*) malloc( _q->m*sizeof(float) );
    for (i=0; i<_q->m; i++)
        _q->dphi[i] = 0.0f;

    // create internal synchronizers
    _q->sync = (BSYNC()*) malloc( _q->m*sizeof(BSYNC()) );
    TC v_prime[_q->n];
    for (i=0; i<_q->m; i++) {
        unsigned int k;
        for (k=0; k<_q->n; k++)
            v_prime[k] = _v[k] * cexpf(_Complex_I*0.0f);

        _q->sync[i] = BSYNC(_create)(_q->n, v_prime);
    }

    return _q;
}

void BPRESYNC(_destroy)(BPRESYNC() _q)
{
    unsigned int i;

    // free internal syncrhonizer objects
    for (i=0; i<_q->m; i++)
        BSYNC(_destroy)(_q->sync[i]);
    free(_q->sync);

    // free internal frequency offset array
    free(_q->dphi);

    // free main object memory
    free(_q);
}

void BPRESYNC(_print)(BPRESYNC() _q)
{
    printf("bpresync_xxxt:\n");
}

/* correlate input sequence                                 */
/*  _q          :   pre-demod synchronizer object           */
/*  _x          :   input sample                            */
/*  _rxy        :   output cross correlation                */
/*  _dphi_hat   :   output frequency offset estiamte        */
void BPRESYNC(_correlate)(BPRESYNC() _q,
                          TI         _sym,
                          TO *       _rxy,
                          float *    _dphi_hat)
{
    // push symbol into buffers
    *_rxy = 0.0f;
    *_dphi_hat = 0.0f;
}


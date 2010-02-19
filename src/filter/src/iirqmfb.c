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
// infinite impulse response quadrature mirror filterbank (IIRQMFB)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

// defined:
//  IIRQMFB()       name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  IIR_FILTER()    iir filter macro
//  PRINTVAL()      print macro

struct IIRQMFB(_s) {
    unsigned int order; // filter order
    float beta;         // filter bandwidth

    int type;           // analyzer/synthesizer
    int prototype;      // butterworth/ellip/etc.

    unsigned int r;     // order % 2
    unsigned int L;     // (order-r)/2

    IIR_FILTER() A0;    // all-pass filter (lower branch)
    IIR_FILTER() A1;    // all-pass filter (upper branch)

    TI v;               // delay element
};

// forward declaration
//  _order  :   filter order
//  _zd     :   digital zeros [1 x _order]
//  _pd     :   digital poles [1 x _order]
//
// r = _order % 2
// L = (_order-r)/2
//  _zd0    :   digital zeros, upper all-pass filter [1 x L]
//  _pd0    :   digital poles, upper all-pass filter [1 x L]
//  _zd1    :   digital zeros, lower all-pass filter [1 x L+r]
//  _pd1    :   digital poles, lower all-pass filter [1 x L+r]
void IIRQMFB(_allpass_zpk)(unsigned int _order,
                           float complex * _zd,
                           float complex * _pd,
                           float complex * _zd0,
                           float complex * _pd0,
                           float complex * _zd1,
                           float complex * _pd1);

IIRQMFB() IIRQMFB(_create)(unsigned int _order,
                           float _beta,
                           int _type,
                           int _prototype)
{
    IIRQMFB() q = (IIRQMFB()) malloc(sizeof(struct IIRQMFB(_s)));
    q->order = _order;
    q->type  = _type;

    // design all-pass filters
    q->r = q->order % 2;        // odd order?
    q->L = (q->order - q->r)/2; // floor(order/2)

    // analog poles/zeros/gain
    float complex pa[q->order];
    float complex za[q->order];
    float complex ka;

    // TODO : use design other than Butterworth based on input prototype
    butter_azpkf(q->order,0.25f,za,pa,&ka);
    unsigned int npa = q->order;
    unsigned int nza = 0;

    // complex digital poles/zeros/gain
    float complex zd[q->order];
    float complex pd[q->order];
    float complex kd;
    float fc = 0.25f;   // half-band
    float m = 1.0f / tanf(M_PI * fc);   // m = 1
    bilinear_zpkf(za,    nza,
                  pa,    npa,
                  ka,    m,
                  zd, pd, &kd);
    printf("kd : %12.8f + j*%12.8f\n", crealf(kd), cimagf(kd));

    // search for appropriate pairing
    unsigned int order_A0 = q->L;
    unsigned int order_A1 = q->L + q->r;
    printf("order(A0) : %u\n", order_A0);
    printf("order(A1) : %u\n", order_A1);
    float complex zd0[order_A0];
    float complex pd0[order_A0];
    float complex zd1[order_A1];
    float complex pd1[order_A1];
    IIRQMFB(_allpass_zpk)(q->order,
                          zd,  pd,
                          zd0, pd0,
                          zd1, pd1);
#if 0
    unsigned int i;
    printf("zeros (upper)\n");
    for (i=0; i<order_A0; i++)
        printf("  zd0[%3u] : %12.8f + j*%12.8f\n", i, crealf(zd0[i]), cimagf(zd0[i]));
    printf("poles (upper)\n");
    for (i=0; i<order_A0; i++)
        printf("  pd0[%3u] : %12.8f + j*%12.8f\n", i, crealf(pd0[i]), cimagf(pd0[i]));

    printf("\n");
    printf("zeros (lower)\n");
    for (i=0; i<order_A1; i++)
        printf("  zd1[%3u] : %12.8f + j*%12.8f\n", i, crealf(zd1[i]), cimagf(zd1[i]));
    printf("poles (lower)\n");
    for (i=0; i<order_A1; i++)
        printf("  pd1[%3u] : %12.8f + j*%12.8f\n", i, crealf(pd1[i]), cimagf(pd1[i]));
#endif

    // create filters
    unsigned int r0 = order_A0 % 2;
    unsigned int L0 = (order_A0 - r0)/2;
    unsigned int r1 = order_A1 % 2;
    unsigned int L1 = (order_A1 - r1)/2;
    float B[3*(L1 + r1)];
    float A[3*(L1 + r1)];

    // split complex digital gain between filters
    float kd0 = sqrtf(crealf(kd));
    float kd1 = sqrtf(crealf(kd));

    // create A0 (upper branch)
    iirdes_dzpk2sosf(zd0,pd0,order_A0,kd0,B,A);
    q->A0 = IIR_FILTER(_create_sos)(B, A, L0+r0);

    // create A1 (lower branch)
    iirdes_dzpk2sosf(zd1,pd1,order_A1,kd1,B,A);
    q->A1 = IIR_FILTER(_create_sos)(B, A, L1+r1);

    // clear the object's internal state
    IIRQMFB(_clear)(q);

    return q;
}

IIRQMFB() IIRQMFB(_recreate)(IIRQMFB() _q,
                             unsigned int _h_len)
{
    // TODO implement this method
    printf("error: iirqmfb_xxxt_recreate(), method not supported yet\n");
    exit(1);
    return NULL;
}

void IIRQMFB(_destroy)(IIRQMFB() _q)
{
    IIR_FILTER(_destroy)(_q->A0);
    IIR_FILTER(_destroy)(_q->A1);
    free(_q);
}

void IIRQMFB(_print)(IIRQMFB() _q)
{
    printf("iir quadrature mirror filterbank (%s):\n", 
           _q->type == LIQUID_QMFB_ANALYZER ? "analysis" : "synthesis");
    printf("    order   :   %u\n", _q->order);
    IIR_FILTER(_print)(_q->A0);
    IIR_FILTER(_print)(_q->A1);
}

void IIRQMFB(_clear)(IIRQMFB() _q)
{
    IIR_FILTER(_clear)(_q->A0);
    IIR_FILTER(_clear)(_q->A1);
    _q->v = 0;
}


void IIRQMFB(_execute)(IIRQMFB() _q,
                       TI   _x0,
                       TI   _x1,
                       TO * _y0,
                       TO * _y1)
{
    if (_q->type == LIQUID_QMFB_ANALYZER)
        IIRQMFB(_analysis_execute)(_q,_x0,_x1,_y0,_y1);
    else
        IIRQMFB(_synthesis_execute)(_q,_x0,_x1,_y0,_y1);
}

void IIRQMFB(_analysis_execute)(IIRQMFB() _q,
                                TI   _x0,
                                TI   _x1,
                                TO * _y0,
                                TO * _y1)
{
    // run upper branch
    TO t0;
    IIR_FILTER(_execute)(_q->A0, _x0, &t0);

    // run lower branch
    TO t1;
    IIR_FILTER(_execute)(_q->A1, _x1, &t1);

    // compute outputs
    *_y0 = 0.5f*(t0 + t1);
    *_y1 = 0.5f*(t0 - t1);
}

void IIRQMFB(_synthesis_execute)(IIRQMFB() _q,
                                 TI   _x0,
                                 TI   _x1,
                                 TO * _y0,
                                 TO * _y1)
{
    TI t0 = _x0 + _x1;
    TI t1 = _x0 - _x1;

    IIR_FILTER(_execute)(_q->A0, t0, _y0);
    IIR_FILTER(_execute)(_q->A1, t1, _y1);
}

// internal

//  _order  :   filter order
//  _zd     :   digital zeros [1 x _order]
//  _pd     :   digital poles [1 x _order]
//
// r = _order % 2
// L = (_order-r)/2
//  _zd0    :   digital zeros, upper all-pass filter [1 x L]
//  _pd0    :   digital poles, upper all-pass filter [1 x L]
//  _zd1    :   digital zeros, lower all-pass filter [1 x L+r]
//  _pd1    :   digital poles, lower all-pass filter [1 x L+r]
void IIRQMFB(_allpass_zpk)(unsigned int _order,
                           float complex * _zd,
                           float complex * _pd,
                           float complex * _zd0,
                           float complex * _pd0,
                           float complex * _zd1,
                           float complex * _pd1)
{
    //
    unsigned int r = _order % 2;
    unsigned int L = (_order - r)/2;
    unsigned int p = L % 2;
    unsigned int M = (L-p)/2;
    printf("r   : %u\n", r);
    printf("L   : %u\n", L);

    // sort
    float tol = 1e-4f;

    // find/group complex conjugate pairs (zeros, poles)
    float complex zdcc[_order];
    float complex pdcc[_order];
    liquid_cplxpair(_zd,_order,tol,zdcc);
    liquid_cplxpair(_pd,_order,tol,pdcc);

    unsigned int i;
    for (i=0; i<_order; i++) {
        printf("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(pdcc[i]), cimagf(pdcc[i]));

        if (fabsf(crealf(pdcc[i])) > tol) {
            fprintf(stderr,"warning: iirqmfb_xxxt_allpass_zpk(), not all poles lie on imaginary axis\n");
        }
    }

    // TODO : ensure values are sorted properly

    unsigned int k0=0;
    unsigned int k1=0;
    for (i=0; i<L; i++) {
        if ( (i%2)==0 ) {
            _zd0[k0  ] = zdcc[2*i+0];
            _pd0[k0++] = pdcc[2*i+0];

            _zd0[k0  ] = zdcc[2*i+1];
            _pd0[k0++] = pdcc[2*i+1];
        } else {
            _zd1[k1  ] = zdcc[2*i+0];
            _pd1[k1++] = pdcc[2*i+0];

            _zd1[k1  ] = zdcc[2*i+1];
            _pd1[k1++] = pdcc[2*i+1];
        }
    }

    if (r) {
        _zd1[k1  ] = zdcc[2*L];
        _pd1[k1++] = pdcc[2*L];
    }

    printf("k0 : %3u (%3u)\n", k0, 2*(M+p));
    printf("k1 : %3u (%3u)\n", k1, 2*(M+0)+r);

    assert(k0 == 2*(M+p)+0);
    assert(k1 == 2*(M+0)+r);

#if 0
    printf("zeros (upper)\n");
    for (i=0; i<L; i++)
        printf("  zd0[%3u] : %12.8f + j*%12.8f\n", i, crealf(_zd0[i]), cimagf(_zd0[i]));
    printf("poles (upper)\n");
    for (i=0; i<L; i++)
        printf("  pd0[%3u] : %12.8f + j*%12.8f\n", i, crealf(_pd0[i]), cimagf(_pd0[i]));

    printf("\n");
    printf("zeros (lower)\n");
    for (i=0; i<L+r; i++)
        printf("  zd1[%3u] : %12.8f + j*%12.8f\n", i, crealf(_zd1[i]), cimagf(_zd1[i]));
    printf("poles (lower)\n");
    for (i=0; i<L+r; i++)
        printf("  pd1[%3u] : %12.8f + j*%12.8f\n", i, crealf(_pd1[i]), cimagf(_pd1[i]));
#endif


}



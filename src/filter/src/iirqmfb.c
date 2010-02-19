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

// digital zeros/poles/gain to power complementary all-pass filters of
// second-order sections form
//  _order  :   filter order
//  _zd     :   digital zeros [1 x _order]
//  _pd     :   digital poles [1 x _order]
//
// r  = _order % 2
// L  = (_order-r)/2
// rp = L % 2;
// Lp = (L - rp)/2;
//
// second-order sections (filter 0) : Lp + r
// second-order sections (filter 1) : Lp + rp
void IIRQMFB(_allpass_dzpk2sosf)(unsigned int _order,
                                 float complex * _zd,
                                 float complex * _pd,
                                 float * _B0,
                                 float * _A0,
                                 float * _B1,
                                 float * _A1);

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

    if (q->r==0) {
        fprintf(stderr,"error: iirqmfb_xxxt_create(), filter order is even (odd filter order will be supported later)\n");
        exit(1);
    }

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

    // TODO : decrease allocated array size (this is too large)
    unsigned int rp = q->L % 2;
    unsigned int Lp = (q->L-rp)/2;
    unsigned int order_A0 = Lp;
    unsigned int order_A1 = Lp + rp;
    float B0[3*order_A0];
    float A0[3*order_A0];
    float B1[3*order_A1];
    float A1[3*order_A1];
    IIRQMFB(_allpass_dzpk2sosf)(q->order, zd, pd,
                                B0, A0,
                                B1, A1);
    q->A0 = IIR_FILTER(_create_sos)(B0, A0, order_A0);
    q->A1 = IIR_FILTER(_create_sos)(B1, A1, order_A1);

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
    // compute upper branch
    TO t0;
    IIR_FILTER(_execute)(_q->A0, _x0, &t0);

    // compute lower branch (delayed input)
    TO t1;
    IIR_FILTER(_execute)(_q->A1, _q->v, &t1);
    _q->v = _x1;

    // compute output
    *_y0 = 0.5f*(t0 + t1);
    *_y1 = 0.5f*(t0 - t1);
}

void IIRQMFB(_synthesis_execute)(IIRQMFB() _q,
                                 TI   _x0,
                                 TI   _x1,
                                 TO * _y0,
                                 TO * _y1)
{
    // compute upper branch (delayed output)
    TI t0 = _x0 + _x1;
    *_y0 = _q->v;
    IIR_FILTER(_execute)(_q->A1, t0, &_q->v);

    // compute lower branch
    TI t1 = _x0 - _x1;
    IIR_FILTER(_execute)(_q->A0, t1, _y1);
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

// digital zeros/poles/gain to power complementary all-pass filters of
// second-order sections form
//  _order  :   filter order
//  _zd     :   digital zeros [1 x _order]
//  _pd     :   digital poles [1 x _order]
//
// r = _order % 2
// L = (_order-r)/2
// NOTE: special conditions exist for this to work
void IIRQMFB(_allpass_dzpk2sosf)(unsigned int _order,
                                 float complex * _zd,
                                 float complex * _pd,
                                 float * _B0,
                                 float * _A0,
                                 float * _B1,
                                 float * _A1)
{
    // NOTE : zeros and gain are ignored

    // find/group complex conjugate pairs (poles)
    float tol = 1e-4f;
    float complex pdcc[_order];
    liquid_cplxpair(_pd,_order,tol,pdcc);

    // TODO : ensure values are sorted properly
    unsigned int r = _order % 2;    // number of first-order sections
    unsigned int L = (_order-r)/2;  // total number of second-order sections

    unsigned int i;
    printf("iirqmfb_xxxt_allpass_dzpk2sosf() :\n");
    for (i=0; i<_order; i++) {
        printf("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(pdcc[i]), cimagf(pdcc[i]));

        if (fabsf(crealf(pdcc[i])) > tol) {
            fprintf(stderr,"warning: iirqmfb_xxxt_allpass_zpk(), not all poles lie on imaginary axis\n");
        }
    }

    unsigned int k0=0;
    unsigned int k1=0;
    for (i=0; i<L; i++) {
        float complex p0 = -pdcc[2*i+0];
        float complex p1 = -pdcc[2*i+1];

        if ( (i%2)==1 ) {
            _A0[3*k0+0] = _B0[3*k0+2] = 1.0;
            _A0[3*k0+1] = _B0[3*k0+1] = crealf(p0+p1);
            _A0[3*k0+2] = _B0[3*k0+0] = crealf(p0*p1);
            k0++;
        } else {
            _A1[3*k1+0] = _B1[3*k1+2] = 1.0;
            _A1[3*k1+1] = _B1[3*k1+1] = crealf(p0+p1);
            _A1[3*k1+2] = _B1[3*k1+0] = crealf(p0*p1);
            k1++;
        }
    }

#if 0
    if (r) {
        float complex p = -pdcc[2*L+0];

        _A0[3*k0 + 0] = _B0[3*k0 + 1] = 1.0;
        _A0[3*k0 + 1] = _B0[3*k0 + 0] = crealf(p);
        _A0[3*k0 + 2] = _B0[3*k0 + 2] = 0.0;

        k0++;
    }
#endif

    unsigned int rp = L % 2;
    unsigned int Lp = (L-rp)/2;

#if 0
    printf("  n  : %u\n", _order);
    printf("  r  : %u\n", r);
    printf("  L  : %u\n", L);
    printf("  rp : %u\n", rp);
    printf("  Lp : %u\n", Lp);
    printf("\n");

    printf("  k0 : %u\n", k0);
    printf("  k1 : %u\n", k1);
#endif

    unsigned int n0 = Lp;
    unsigned int n1 = Lp + rp;

    printf("  n0 : %u\n", n0);
    printf("  n1 : %u\n", n1);

    assert(k0 == n0);
    assert(k1 == n1);

#if 0
    printf("B0 [3 x %u] :\n", k0);
    for (i=0; i<k0; i++)
        printf("  %12.8f %12.8f %12.8f\n", _B0[3*i+0], _B0[3*i+1], _B0[3*i+2]);
    printf("A0 [3 x %u] :\n", k0);
    for (i=0; i<k0; i++)
        printf("  %12.8f %12.8f %12.8f\n", _A0[3*i+0], _A0[3*i+1], _A0[3*i+2]);

    printf("\n");

    printf("B1 [3 x %u] :\n", k1);
    for (i=0; i<k1; i++)
        printf("  %12.8f %12.8f %12.8f\n", _B1[3*i+0], _B1[3*i+1], _B1[3*i+2]);
    printf("A1 [3 x %u] :\n", k1);
    for (i=0; i<k1; i++)
        printf("  %12.8f %12.8f %12.8f\n", _A1[3*i+0], _A1[3*i+1], _A1[3*i+2]);
#endif
}



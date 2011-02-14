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
//  IIRFILT()    iir filter macro
//  PRINTVAL()      print macro

struct IIRQMFB(_s) {
    unsigned int order; // filter order
    float beta;         // filter bandwidth

    int type;           // analyzer/synthesizer
    int prototype;      // butterworth/ellip/etc.

    unsigned int r;     // order % 2
    unsigned int L;     // (order-r)/2

    IIRFILT() A0;    // all-pass filter (lower branch)
    IIRFILT() A1;    // all-pass filter (upper branch)

    TI v;               // delay element
};

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
    butter_azpkf(q->order,za,pa,&ka);
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
    q->A0 = IIRFILT(_create_sos)(B0, A0, order_A0);
    q->A1 = IIRFILT(_create_sos)(B1, A1, order_A1);

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
    IIRFILT(_destroy)(_q->A0);
    IIRFILT(_destroy)(_q->A1);
    free(_q);
}

void IIRQMFB(_print)(IIRQMFB() _q)
{
    printf("iir quadrature mirror filterbank (%s):\n", 
           _q->type == LIQUID_QMFB_ANALYZER ? "analysis" : "synthesis");
    printf("    order   :   %u\n", _q->order);
    IIRFILT(_print)(_q->A0);
    IIRFILT(_print)(_q->A1);
}

void IIRQMFB(_clear)(IIRQMFB() _q)
{
    IIRFILT(_clear)(_q->A0);
    IIRFILT(_clear)(_q->A1);
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
    IIRFILT(_execute)(_q->A0, _x0, &t0);

    // compute lower branch (delayed input)
    TO t1;
    IIRFILT(_execute)(_q->A1, _q->v, &t1);
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
    IIRFILT(_execute)(_q->A1, t0, &_q->v);

    // compute lower branch
    TI t1 = _x0 - _x1;
    IIRFILT(_execute)(_q->A0, t1, _y1);
}

// internal

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



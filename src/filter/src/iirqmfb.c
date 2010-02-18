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
#include <math.h>

// defined:
//  IIRQMFB()       name-mangling macro
//  TO              output data type
//  TC              coefficient data type
//  TI              input data type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct IIRQMFB(_s) {
    unsigned int order; // filter order
    float beta;         // filter bandwidth

    int type;           // analyzer/synthesizer
    int prototype;      // butterworth/ellip/etc.

    unsigned int r;     // order % 2
    unsigned int L;     // (order-r)/2

    IIR_FILTER() A0;
    IIR_FILTER() A1;
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

    // TODO : search for appropriate pairing

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
    //IIR_FILTER(_destroy)(_q->A0);
    //IIR_FILTER(_destroy)(_q->A1);
    free(_q);
}

void IIRQMFB(_print)(IIRQMFB() _q)
{
    printf("iir quadrature mirror filterbank (%s):\n", 
           _q->type == LIQUID_QMFB_ANALYZER ? "analysis" : "synthesis");
    printf("    order   :   %u\n", _q->order);
    //IIR_FILTER(_print)(_q->A0);
    //IIR_FILTER(_print)(_q->A1);
}

void IIRQMFB(_clear)(IIRQMFB() _q)
{
    //IIR_FILTER(_clear)(_q->A0);
    //IIR_FILTER(_clear)(_q->A1);
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
    *_y0 = 0.;
    *_y1 = 0.;
}

void IIRQMFB(_synthesis_execute)(IIRQMFB() _q,
                                 TI   _x0,
                                 TI   _x1,
                                 TO * _y0,
                                 TO * _y1)
{
    *_y0 = 0.;
    *_y1 = 0.;
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

    // sort
    float tol = 1e-4f;

    // find/group complex conjugate pairs (zeros, poles)
    float complex zdcc[_order];
    float complex pdcc[_order];
    liquid_cplxpair(_zd,_order,tol,zdcc);
    liquid_cplxpair(_pd,_order,tol,pdcc);

    unsigned int i;
    for (i=0; i<L; i++) {
    }
}



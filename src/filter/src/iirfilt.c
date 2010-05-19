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
// iirfilt : Infinite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  IIRFILT()       name-mangling macro
//  TO              output type
//  TC              coefficients type
//  TI              input type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

#define LIQUID_IIRFILT_USE_DOTPROD   0

struct IIRFILT(_s) {
    TC * b;         // feedforward coefficients
    TC * a;         // feedback coefficients
    TI * v;         // internal filter state
    unsigned int n; // filter length

    unsigned int nb;
    unsigned int na;

    enum {
        IIRFILT_TYPE_NORM=0,
        IIRFILT_TYPE_SOS
    } type;

#if LIQUID_IIRFILT_USE_DOTPROD
    DOTPROD() dpa;
    DOTPROD() dpb;
#endif

    // second-order sections 
    IIRFILTSOS() * fsos;    // second-order sections filters
    unsigned int L;         // number of second-order sections
};

// create iirfilt (infinite impulse response filter) object
//  _b      :   numerator, feed-forward coefficients [size: _nb x 1]
//  _nb     :   length of numerator
//  _a      :   denominator, feed-back coefficients [size: _na x 1]
//  _na     :   length of denominator
IIRFILT() IIRFILT(_create)(TC * _b,
                           unsigned int _nb,
                           TC * _a,
                           unsigned int _na)
{
    IIRFILT() f = (IIRFILT()) malloc(sizeof(struct IIRFILT(_s)));
    f->nb = _nb;
    f->na = _na;
    f->n = (f->na > f->nb) ? f->na : f->nb;
    f->type = IIRFILT_TYPE_NORM;

    f->b = (TC *) malloc((f->na)*sizeof(TC));
    f->a = (TC *) malloc((f->nb)*sizeof(TC));

    TC a0 = _a[0];

    unsigned int i;
#if 0
    // read values in reverse order
    for (i=0; i<f->nb; i++)
        f->b[i] = _b[f->nb - i - 1];

    for (i=0; i<f->na; i++)
        f->a[i] = _a[f->na - i - 1];
#else
    for (i=0; i<f->nb; i++)
        f->b[i] = _b[i] / a0;

    for (i=0; i<f->na; i++)
        f->a[i] = _a[i] / a0;
#endif

    f->v = (TI *) malloc((f->n)*sizeof(TI));
    for (i=0; i<f->n; i++)
        f->v[i] = 0;

#if LIQUID_IIRFILT_USE_DOTPROD
    f->dpa = DOTPROD(_create)(f->a+1, f->na-1);
    f->dpb = DOTPROD(_create)(f->b,   f->nb);
#endif
    
    return f;
}

// create iirfilt (infinite impulse response filter) object based
// on second-order sections form
//  _B      :   numerator, feed-forward coefficients [size: _L x 3]
//  _A      :   denominator, feed-back coefficients [size: _L x 3]
//  _L      :   number of second-order sections
IIRFILT() IIRFILT(_create_sos)(TC * _B,
                               TC * _A,
                               unsigned int _L)
{
    IIRFILT() f = (IIRFILT()) malloc(sizeof(struct IIRFILT(_s)));

    f->type = IIRFILT_TYPE_SOS;
    f->L = _L;
    f->fsos = (IIRFILTSOS()*) malloc( (f->L)*sizeof(IIRFILTSOS()) );

    // create coefficients array and copy over
    f->b = (TC *) malloc(3*(f->L)*sizeof(TC));
    f->a = (TC *) malloc(3*(f->L)*sizeof(TC));
    memmove(f->b, _B, 3*(f->L)*sizeof(TC));
    memmove(f->a, _A, 3*(f->L)*sizeof(TC));

    TC at[3];
    TC bt[3];
    unsigned int i,k;
    for (i=0; i<f->L; i++) {
        for (k=0; k<3; k++) {
            at[k] = f->a[3*i+k];
            bt[k] = f->b[3*i+k];
        }
        f->fsos[i] = IIRFILTSOS(_create)(bt,at);
        //f->fsos[i] = IIRFILT(_create)(f->b+3*i,3,f->a+3*i,3);
    }
    return f;
}


IIRFILT() IIRFILT(_create_prototype)(unsigned int _n)
{
    printf("warning: iirfilt_create_prototype(), not yet implemented\n");
    return NULL;
}

// destroy iirfilt object
void IIRFILT(_destroy)(IIRFILT() _q)
{
#if LIQUID_IIRFILT_USE_DOTPROD
    DOTPROD(_destroy)(_q->dpa);
    DOTPROD(_destroy)(_q->dpb);
#endif
    free(_q->b);
    free(_q->a);
    // if filter is comprised of cascaded second-order sections,
    // delete sub-filters separately
    if (_q->type == IIRFILT_TYPE_SOS) {
        unsigned int i;
        for (i=0; i<_q->L; i++)
            IIRFILTSOS(_destroy)(_q->fsos[i]);
        free(_q->fsos);
    } else {
        free(_q->v);
    }

    free(_q);
}

// print iirfilt object internals
void IIRFILT(_print)(IIRFILT() _q)
{
    printf("iir filter [%s]:\n", _q->type == IIRFILT_TYPE_NORM ? "normal" : "sos");
    unsigned int i;

    if (_q->type == IIRFILT_TYPE_SOS) {
        for (i=0; i<_q->L; i++)
            IIRFILTSOS(_print)(_q->fsos[i]);
    } else {

        printf("  b :");
        for (i=0; i<_q->nb; i++)
            PRINTVAL_TC(_q->b[i],%12.8f);
        printf("\n");

        printf("  a :");
        for (i=0; i<_q->na; i++)
            PRINTVAL_TC(_q->a[i],%12.8f);
        printf("\n");

#if 0
        printf("  v :");
        for (i=0; i<_q->n; i++)
            PRINTVAL(_q->v[i]);
        printf("\n");
#endif
    }
}

// clear
void IIRFILT(_clear)(IIRFILT() _q)
{
    unsigned int i;

    if (_q->type == IIRFILT_TYPE_SOS) {
        // clear second-order sections
        for (i=0; i<_q->L; i++) {
            IIRFILTSOS(_clear)(_q->fsos[i]);
        }
    } else {
        // set internal buffer to zero
        for (i=0; i<_q->n; i++)
            _q->v[i] = 0;
    }
}

// execute normal iir filter (not second-order sections form)
//  _q      :   iirfilt object
//  _x      :   input sample
//  _y      :   output sample
void IIRFILT(_execute_norm)(IIRFILT() _q,
                            TI _x,
                            TO *_y)
{
    unsigned int i;

    // advance buffer
    for (i=_q->n-1; i>0; i--)
        _q->v[i] = _q->v[i-1];

#if LIQUID_IIRFILT_USE_DOTPROD
    // compute new v
    TI v0;
    DOTPROD(_execute)(_q->dpa, _q->v+1, & v0);
    v0 = _x - v0;
    _q->v[0] = v0;

    // compute new y
    DOTPROD(_execute)(_q->dpb, _q->v, _y);
#else
    // compute new v
    TI v0 = _x;
    for (i=1; i<_q->na; i++)
        v0 -= _q->a[i] * _q->v[i];
    _q->v[0] = v0;

    // compute new y
    TO y0 = 0;
    for (i=0; i<_q->nb; i++)
        y0 += _q->b[i] * _q->v[i];

    // set return value
    *_y = y0;
#endif
}

// execute iir filter using second-order sections form
//  _q      :   iirfilt object
//  _x      :   input sample
//  _y      :   output sample
void IIRFILT(_execute_sos)(IIRFILT() _q,
                           TI _x,
                           TO *_y)
{
    TI t0 = _x;     // intermediate input
    TO t1;          // intermediate output
    unsigned int i;
    for (i=0; i<_q->L; i++) {
        // run each filter separately
        IIRFILTSOS(_execute)(_q->fsos[i], t0, &t1);

        // output becomes input
        t0 = t1;
    }
    *_y = t1;
}

// execute iir filter, switching to type-specific function
//  _q      :   iirfilt object
//  _x      :   input sample
//  _y      :   output sample
void IIRFILT(_execute)(IIRFILT() _q,
                       TI _x,
                       TO *_y)
{
    if (_q->type == IIRFILT_TYPE_NORM)
        IIRFILT(_execute_norm)(_q,_x,_y);
    else
        IIRFILT(_execute_sos)(_q,_x,_y);
}

// get filter length (order + 1)
unsigned int IIRFILT(_get_length)(IIRFILT() _q)
{
    return _q->n;
}


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
// Infinite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  IIR_FILTER()    name-mangling macro
//  TO              output type
//  TC              coefficients type
//  TI              input type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

#define LIQUID_IIR_FILTER_USE_DOTPROD   0

struct IIR_FILTER(_s) {
    TC * b;         // feedforward coefficients
    TC * a;         // feedback coefficients
    TI * v;         // internal filter state
    unsigned int n; // filter length

    unsigned int nb;
    unsigned int na;

    enum {
        IIR_FILTER_TYPE_NORM=0,
        IIR_FILTER_TYPE_SOS
    } type;

#if LIQUID_IIR_FILTER_USE_DOTPROD
    DOTPROD() dpa;
    DOTPROD() dpb;
#endif

    // second-order sections 
    IIR_FILTER() * fsos;    // filters
    unsigned int nsos;      // number of sections
};

IIR_FILTER() IIR_FILTER(_create)(TC * _b, unsigned int _nb, TC * _a, unsigned int _na)
{
    IIR_FILTER() f = (IIR_FILTER()) malloc(sizeof(struct IIR_FILTER(_s)));
    f->nb = _nb;
    f->na = _na;
    f->n = (f->na > f->nb) ? f->na : f->nb;
    f->type = IIR_FILTER_TYPE_NORM;

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

#if LIQUID_IIR_FILTER_USE_DOTPROD
    f->dpa = DOTPROD(_create)(f->a+1, f->na-1);
    f->dpb = DOTPROD(_create)(f->b,   f->nb);
#endif
    
    return f;
}

IIR_FILTER() IIR_FILTER(_create_sos)(TC * _B,
                                     TC * _A,
                                     unsigned int _nsos)
{
    IIR_FILTER() f = (IIR_FILTER()) malloc(sizeof(struct IIR_FILTER(_s)));

    f->type = IIR_FILTER_TYPE_SOS;
    f->nsos = _nsos;
    f->fsos = (IIR_FILTER()*) malloc( (f->nsos)*sizeof(IIR_FILTER()) );

    // create coefficients array and copy over
    f->b = (TC *) malloc(3*(f->nsos)*sizeof(TC));
    f->a = (TC *) malloc(3*(f->nsos)*sizeof(TC));
    memmove(f->b, _B, 3*(f->nsos)*sizeof(TC));
    memmove(f->a, _A, 3*(f->nsos)*sizeof(TC));

    TC at[3];
    TC bt[3];
    unsigned int i,k;
    for (i=0; i<f->nsos; i++) {
        for (k=0; k<3; k++) {
            at[k] = f->a[3*i+k];
            bt[k] = f->b[3*i+k];
        }
        f->fsos[i] = IIR_FILTER(_create)(bt,3,at,3);
        //f->fsos[i] = IIR_FILTER(_create)(f->b+3*i,3,f->a+3*i,3);
    }
    return f;
}


IIR_FILTER() IIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: iir_filter_create_prototype(), not yet implemented\n");
    return NULL;
}

void IIR_FILTER(_destroy)(IIR_FILTER() _f)
{
#if LIQUID_IIR_FILTER_USE_DOTPROD
    DOTPROD(_destroy)(_f->dpa);
    DOTPROD(_destroy)(_f->dpb);
#endif
    free(_f->b);
    free(_f->a);
    // if filter is comprised of cascaded second-order sections,
    // delete sub-filters separately
    if (_f->type == IIR_FILTER_TYPE_SOS) {
        unsigned int i;
        for (i=0; i<_f->nsos; i++)
            IIR_FILTER(_destroy)(_f->fsos[i]);
        free(_f->fsos);
    } else {
        free(_f->v);
    }

    free(_f);
}

void IIR_FILTER(_print)(IIR_FILTER() _f)
{
    printf("iir filter [%s]:\n", _f->type == IIR_FILTER_TYPE_NORM ? "normal" : "sos");
    unsigned int i;

    if (_f->type == IIR_FILTER_TYPE_SOS) {
        for (i=0; i<_f->nsos; i++)
            IIR_FILTER(_print)(_f->fsos[i]);
    } else {

        printf("  b :");
        for (i=0; i<_f->nb; i++)
            PRINTVAL(_f->b[i]);
        printf("\n");

        printf("  a :");
        for (i=0; i<_f->na; i++)
            PRINTVAL(_f->a[i]);
        printf("\n");

#if 0
        printf("  v :");
        for (i=0; i<_f->n; i++)
            PRINTVAL(_f->v[i]);
        printf("\n");
#endif
    }
}

void IIR_FILTER(_clear)(IIR_FILTER() _f)
{
    // set to zero
    unsigned int i;
    for (i=0; i<_f->n; i++)
        _f->v[i] = 0;
}

void IIR_FILTER(_execute_norm)(IIR_FILTER() _f,
                               TI _x,
                               TO *_y)
{
    unsigned int i;

    // advance buffer
    for (i=_f->n-1; i>0; i--)
        _f->v[i] = _f->v[i-1];

#if LIQUID_IIR_FILTER_USE_DOTPROD
    // compute new v
    TI v0;
    DOTPROD(_execute)(_f->dpa, _f->v+1, & v0);
    v0 = _x - v0;
    _f->v[0] = v0;

    // compute new y
    DOTPROD(_execute)(_f->dpb, _f->v, _y);
#else
    // compute new v
    TI v0 = _x;
    for (i=1; i<_f->na; i++)
        v0 -= _f->a[i] * _f->v[i];
    _f->v[0] = v0;

    // compute new y
    TO y0 = 0;
    for (i=0; i<_f->nb; i++)
        y0 += _f->b[i] * _f->v[i];

    // set return value
    *_y = y0;
#endif
}

void IIR_FILTER(_execute_sos)(IIR_FILTER() _f, TI _x, TO *_y)
{
    TI t0 = _x;     // intermediate input
    TO t1;          // intermediate output
    unsigned int i;
    for (i=0; i<_f->nsos; i++) {
        // run each filter separately
        IIR_FILTER(_execute_norm)(_f->fsos[i], t0, &t1);

        // output becomes input
        t0 = t1;
    }
    *_y = t1;
}

void IIR_FILTER(_execute)(IIR_FILTER() _f,
                          TI _x,
                          TO *_y)
{
    if (_f->type == IIR_FILTER_TYPE_NORM)
        IIR_FILTER(_execute_norm)(_f,_x,_y);
    else
        IIR_FILTER(_execute_sos)(_f,_x,_y);
}


unsigned int IIR_FILTER(_get_length)(IIR_FILTER() _f)
{
    return _f->n;
}


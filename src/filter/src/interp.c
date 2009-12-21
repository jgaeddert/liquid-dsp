/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Interpolator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined:
//  INTERP()    name-mangling macro
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct INTERP(_s) {
    TC * h;
    unsigned int h_len;
    unsigned int M;

    fir_prototype p;
    WINDOW() w;
    //DOTPROD() dp;
};

INTERP() INTERP(_create)(unsigned int _M, TC *_h, unsigned int _h_len)
{
    INTERP() q = (INTERP()) malloc(sizeof(struct INTERP(_s)));
    q->h_len = _h_len;
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));
    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    q->M = _M;

    q->w = WINDOW(_create)(q->h_len);
    WINDOW(_clear)(q->w);

    return q;
}


INTERP() INTERP(_create_rrc)(unsigned int _k, unsigned int _m, float _beta, float _dt)
{
    // generate rrc filter
    unsigned int h_len = 2*_k*_m + 1;
    float h[h_len];
    design_rrc_filter(_k,_m,_beta,_dt,h);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC ht[h_len];
    for (i=0; i<h_len; i++)
        ht[i] = h[i];

    return INTERP(_create)(_k, ht, h_len);
}

#if 0
INTERP() INTERP(_create_prototype)(fir_prototype _p, void *_opt)
{
    struct fir_prototype_s * s = (struct fir_prototype_s*) _opt;
    unsigned int h_len = 2*(s->k)*(s->m) + 1;
    float h[h_len];

    switch (_p) {
    case FIR_RRCOS:
        design_rrc_filter(s->k, s->m, s->beta, s->dt, h);
        break;
    default:
        printf("error: interp_create_prototype(), unknown/unsupported prototype\n");
        exit(0);
    }

    // copy filter into type-specific array
    TC ht[h_len];
    unsigned int i;
    for (i=0; i<h_len; i++)
        ht[i] = h[i];

    unsigned int M = s->k;
    INTERP() q = INTERP(_create)(M, ht, h_len);

    return q;
}
#endif

void INTERP(_destroy)(INTERP() _q)
{
    WINDOW(_destroy)(_q->w);
    free(_q->h);
    free(_q);
}

void INTERP(_print)(INTERP() _q)
{
    printf("interp() [%u] :\n", _q->M);
    printf("  window:\n");
    WINDOW(_print)(_q->w);
}

void INTERP(_clear)(INTERP() _q)
{
    WINDOW(_clear)(_q->w);
}

// TODO : only compute necessary multiplications in interp_xxt_execute
void INTERP(_execute)(INTERP() _q, TI _x, TO *_y)
{
    TI * r; // read pointer

    unsigned int i;
    for (i=0; i<_q->M; i++) {
        if (i == 0)
            WINDOW(_push)(_q->w,_x);
        else
            WINDOW(_push)(_q->w,0);

        WINDOW(_read)(_q->w,&r);
        DOTPROD(_run4)(_q->h, r, _q->h_len, &_y[i]);
    }
}


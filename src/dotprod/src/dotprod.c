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
// Generic dot product
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// basic dot product

void DOTPROD(_run)(TC *_h, TI *_x, unsigned int _n, TO * _y)
{
    TO r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    *_y = r;
}

void DOTPROD(_run4)(TC *_h, TI *_x, unsigned int _n, TO * _y)
{
    TO r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _h[i]   * _x[i];
        r += _h[i+1] * _x[i+1];
        r += _h[i+2] * _x[i+2];
        r += _h[i+3] * _x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _h[i] * _x[i];

    *_y = r;
}

//
// structured dot product
//

DOTPROD() DOTPROD(_create)(TC * _h, unsigned int _n)
{
    DOTPROD() q = (DOTPROD()) malloc(sizeof(struct DOTPROD(_s)));
    q->n = _n;
    q->h = (TC*) malloc((q->n)*sizeof(TC));
    memmove(q->h, _h, (q->n)*sizeof(TC));
    return q;
}

DOTPROD() DOTPROD(_create_rev)(TC * _h, unsigned int _n)
{
    DOTPROD() q = (DOTPROD()) malloc(sizeof(struct DOTPROD(_s)));
    q->n = _n;
    q->h = (TC*) malloc((q->n)*sizeof(TC));

    // load coefficients in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        q->h[i-1] = _h[_n-i];
    
    return q;
}

void DOTPROD(_destroy)(DOTPROD() _q)
{
    free(_q->h);
    free(_q);
}

void DOTPROD(_print)(DOTPROD() _q)
{
    printf("dotprod [%u elements]:\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++) {
        printf("  %4u: %12.8f + j*%12.8f\n", i, crealf(_q->h[i]), cimagf(_q->h[i]));
    }
}

void DOTPROD(_execute)(DOTPROD() _q, TI * _x, TO * _y)
{
    DOTPROD(_run)(_q->h, _x, _q->n, _y);
}


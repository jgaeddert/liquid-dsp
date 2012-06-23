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
// Infinite impulse response filter (second-order section)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  IIRFILTSOS()    name-mangling macro
//  TO              output type
//  TC              coefficients type
//  TI              input type
//  PRINTVAL()      print macro(s)

// create iirfiltsos object
IIRFILTSOS() IIRFILTSOS(_create)(TC * _b,
                                 TC * _a)
{
    // create filter object
    IIRFILTSOS() q = (IIRFILTSOS()) malloc(sizeof(struct IIRFILTSOS(_s)));

    // set the internal coefficients
    IIRFILTSOS(_set_coefficients)(q, _b, _a);

    // clear filter state
    IIRFILTSOS(_clear)(q);

    return q;
}

// set internal filter coefficients
// NOTE : this does not reset the internal state of the filter and
//        could result in instability if executed on existing filter!
void IIRFILTSOS(_set_coefficients)(IIRFILTSOS() _q,
                                   TC *         _b,
                                   TC *         _a)
{
    // retain a0 coefficient for normalization
    TC a0 = _a[0];

#if defined LIQUID_FIXED && TC_COMPLEX==0
    // copy feed-forward coefficients (numerator)
    _q->b[0] = qtype_float_to_fixed( qtype_fixed_to_float(_b[0]) / qtype_fixed_to_float(a0) );
    _q->b[1] = qtype_float_to_fixed( qtype_fixed_to_float(_b[1]) / qtype_fixed_to_float(a0) );
    _q->b[2] = qtype_float_to_fixed( qtype_fixed_to_float(_b[2]) / qtype_fixed_to_float(a0) );

    // copy feed-back coefficients (denominator)
    _q->a[0] = qtype_float_to_fixed( qtype_fixed_to_float(_a[0]) / qtype_fixed_to_float(a0) );
    _q->a[1] = qtype_float_to_fixed( qtype_fixed_to_float(_a[1]) / qtype_fixed_to_float(a0) );
    _q->a[2] = qtype_float_to_fixed( qtype_fixed_to_float(_a[2]) / qtype_fixed_to_float(a0) );
#elif defined LIQUID_FIXED && TC_COMPLEX==1
    // copy feed-forward coefficients (numerator)
    _q->b[0] = cqtype_float_to_fixed( cqtype_fixed_to_float(_b[0]) / cqtype_fixed_to_float(a0) );
    _q->b[1] = cqtype_float_to_fixed( cqtype_fixed_to_float(_b[1]) / cqtype_fixed_to_float(a0) );
    _q->b[2] = cqtype_float_to_fixed( cqtype_fixed_to_float(_b[2]) / cqtype_fixed_to_float(a0) );

    // copy feed-back coefficients (denominator)
    _q->a[0] = cqtype_float_to_fixed( cqtype_fixed_to_float(_a[0]) / cqtype_fixed_to_float(a0) );
    _q->a[1] = cqtype_float_to_fixed( cqtype_fixed_to_float(_a[1]) / cqtype_fixed_to_float(a0) );
    _q->a[2] = cqtype_float_to_fixed( cqtype_fixed_to_float(_a[2]) / cqtype_fixed_to_float(a0) );
#else
    // copy feed-forward coefficients (numerator)
    _q->b[0] = _b[0] / a0;
    _q->b[1] = _b[1] / a0;
    _q->b[2] = _b[2] / a0;

    // copy feed-back coefficients (denominator)
    _q->a[0] = _a[0] / a0;
    _q->a[1] = _a[1] / a0;
    _q->a[2] = _a[2] / a0;
#endif
}

void IIRFILTSOS(_destroy)(IIRFILTSOS() _q)
{
    free(_q);
}

void IIRFILTSOS(_print)(IIRFILTSOS() _q)
{
    printf("iir filter | sos:\n");

    printf("  b : ");
    PRINTVAL_TC(_q->b[0],%12.8f); printf(",");
    PRINTVAL_TC(_q->b[1],%12.8f); printf(",");
    PRINTVAL_TC(_q->b[2],%12.8f); printf("\n");

    printf("  a : ");
    PRINTVAL_TC(_q->a[0],%12.8f); printf(",");
    PRINTVAL_TC(_q->a[1],%12.8f); printf(",");
    PRINTVAL_TC(_q->a[2],%12.8f); printf("\n");
}

void IIRFILTSOS(_clear)(IIRFILTSOS() _q)
{
#ifdef LIQUID_FIXED
    memset(_q->v, 0x00, sizeof(_q->v));
    memset(_q->x, 0x00, sizeof(_q->x));
    memset(_q->y, 0x00, sizeof(_q->y));
#else
    // set to zero
    _q->v[0] = 0;
    _q->v[1] = 0;
    _q->v[2] = 0;

    _q->x[0] = 0;
    _q->x[1] = 0;
    _q->x[2] = 0;

    _q->y[0] = 0;
    _q->y[1] = 0;
    _q->y[2] = 0;
#endif
}

void IIRFILTSOS(_execute)(IIRFILTSOS() _q,
                          TI   _x,
                          TO * _y)
{
    // execute type-specific code
    IIRFILTSOS(_execute_df2)(_q,_x,_y);
}


// direct form I
void IIRFILTSOS(_execute_df1)(IIRFILTSOS() _q,
                              TI   _x,
                              TO * _y)
{
    // advance buffer x
    _q->x[2] = _q->x[1];
    _q->x[1] = _q->x[0];
    _q->x[0] = _x;

    // advance buffer y
    _q->y[2] = _q->y[1];
    _q->y[1] = _q->y[0];

#if 0
    // compute new v
    TI v = _q->x[0] * _q->b[0] +
           _q->x[1] * _q->b[1] +
           _q->x[2] * _q->b[2];
#else
    // run dot product
    TO v;
    DOTPROD(_run)(_q->b, _q->x, 3, &v);
#endif

    // compute new y[0]
#ifdef LIQUID_FIXED
    TO t1 = MUL_TI_TC(_q->y[1], _q->a[1]);
    TO t2 = MUL_TI_TC(_q->y[2], _q->a[2]);
    _q->y[0] = SUB_TO_TO(v,        t1);
    _q->y[0] = SUB_TO_TO(_q->y[0], t2);

#else
    _q->y[0] = v -
               _q->y[1] * _q->a[1] -
               _q->y[2] * _q->a[2];
#endif

    // set output
    *_y = _q->y[0];
}

// direct form II
void IIRFILTSOS(_execute_df2)(IIRFILTSOS() _q,
                              TI   _x,
                              TO * _y)
{
    // advance buffer
    _q->v[2] = _q->v[1];
    _q->v[1] = _q->v[0];

#ifdef LIQUID_FIXED
#  warning "fixed-point iirfiltsos_xxxt_execute_df2() not yet implemented"
    fprintf(stderr,"error: iirfiltsos_xxxt_execute_df2() not yet implemented for FPM\n");
    exit(1);
#else
    // compute new v[0]
    _q->v[0] = _x - 
               _q->a[1]*_q->v[1] -
               _q->a[2]*_q->v[2];

    // compute output _y
    *_y = _q->b[0]*_q->v[0] +
          _q->b[1]*_q->v[1] +
          _q->b[2]*_q->v[2];
#endif
}

// compute group delay in samples
//  _q      :   filter object
//  _fc     :   frequency
float IIRFILTSOS(_groupdelay)(IIRFILTSOS() _q,
                              float _fc)
{
    // copy coefficients
    float b[3];
    float a[3];
    unsigned int i;
    for (i=0; i<3; i++) {
#if defined LIQUID_FIXED && TC_COMPLEX==0
        b[i] = qtype_fixed_to_float(_q->b[i]);
        a[i] = qtype_fixed_to_float(_q->a[i]);
#elif defined LIQUID_FIXED && TC_COMPLEX==1
        b[i] = qtype_fixed_to_float(_q->b[i].real);
        a[i] = qtype_fixed_to_float(_q->a[i].real);
#else
        b[i] = crealf(_q->b[i]);
        a[i] = crealf(_q->a[i]);
#endif
    }
    return iir_group_delay(b, 3, a, 3, _fc) + 2.0;
}


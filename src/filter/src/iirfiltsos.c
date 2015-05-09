/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
    IIRFILTSOS(_reset)(q);

    return q;
}

// set internal filter coefficients
// NOTE : this does not reset the internal state of the filter and
//        could result in instability if executed on existing filter!
// explicitly set 2nd-order IIR filter coefficients
//  _q      : iirfiltsos object
//  _b      : feed-forward coefficients [size: _3 x 1]
//  _a      : feed-back coefficients    [size: _3 x 1]
void IIRFILTSOS(_set_coefficients)(IIRFILTSOS() _q,
                                   TC *         _b,
                                   TC *         _a)
{
    // retain a0 coefficient for normalization
    TC a0 = _a[0];

#if defined LIQUID_FIXED && TC_COMPLEX==0
    // copy feed-forward coefficients (numerator)
    _q->b[0] = Q(_div)(_b[0], a0);
    _q->b[1] = Q(_div)(_b[1], a0);
    _q->b[2] = Q(_div)(_b[2], a0);

    // copy feed-back coefficients (denominator)
    _q->a[0] = Q(_div)(_a[0], a0);
    _q->a[1] = Q(_div)(_a[1], a0);
    _q->a[2] = Q(_div)(_a[2], a0);
#elif defined LIQUID_FIXED && TC_COMPLEX==1
    // copy feed-forward coefficients (numerator)
    _q->b[0] = CQ(_div)(_b[0], a0);
    _q->b[1] = CQ(_div)(_b[1], a0);
    _q->b[2] = CQ(_div)(_b[2], a0);

    // copy feed-back coefficients (denominator)
    _q->a[0] = CQ(_div)(_a[0], a0);
    _q->a[1] = CQ(_div)(_a[1], a0);
    _q->a[2] = CQ(_div)(_a[2], a0);
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

// destroy iirfiltsos object, freeing all internal memory
void IIRFILTSOS(_destroy)(IIRFILTSOS() _q)
{
    free(_q);
}

// print iirfiltsos object properties to stdout
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

// clear/reset iirfiltsos object internals
void IIRFILTSOS(_reset)(IIRFILTSOS() _q)
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

// compute filter output
//  _q      : iirfiltsos object
//  _x      : input sample
//  _y      : output sample pointer
void IIRFILTSOS(_execute)(IIRFILTSOS() _q,
                          TI           _x,
                          TO *         _y)
{
    // execute type-specific code
    IIRFILTSOS(_execute_df2)(_q,_x,_y);
}


// compute filter output, direct form I method
//  _q      : iirfiltsos object
//  _x      : input sample
//  _y      : output sample pointer
void IIRFILTSOS(_execute_df1)(IIRFILTSOS() _q,
                              TI           _x,
                              TO *         _y)
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

// compute filter output, direct form I method
//  _q      : iirfiltsos object
//  _x      : input sample
//  _y      : output sample pointer
void IIRFILTSOS(_execute_df2)(IIRFILTSOS() _q,
                              TI           _x,
                              TO *         _y)
{
    // advance buffer
    _q->v[2] = _q->v[1];
    _q->v[1] = _q->v[0];

#if defined LIQUID_FIXED
    // compute new v[0]
    TO va1 = MUL_TI_TC( _q->v[1], _q->a[1] );
    TO va2 = MUL_TI_TC( _q->v[2], _q->a[2] );
    _q->v[0] = SUB_TO_TO( _x, ADD_TO_TO(va1, va2) );

    // compute output _y
    TO vb0 = MUL_TI_TC( _q->v[0], _q->b[0] );
    TO vb1 = MUL_TI_TC( _q->v[1], _q->b[1] );
    TO vb2 = MUL_TI_TC( _q->v[2], _q->b[2] );
    *_y = ADD_TO_TO( vb0, ADD_TO_TO(vb1, vb2) );
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
                              float        _fc)
{
    // copy coefficients
    float b[3];
    float a[3];
    unsigned int i;
    for (i=0; i<3; i++) {
#if defined LIQUID_FIXED && TC_COMPLEX==0
        b[i] = Q(_fixed_to_float)(_q->b[i]);
        a[i] = Q(_fixed_to_float)(_q->a[i]);
#elif defined LIQUID_FIXED && TC_COMPLEX==1
        b[i] = Q(_fixed_to_float)(_q->b[i].real);
        a[i] = Q(_fixed_to_float)(_q->a[i].real);
#else
        b[i] = crealf(_q->b[i]);
        a[i] = crealf(_q->a[i]);
#endif
    }
    return iir_group_delay(b, 3, a, 3, _fc) + 2.0;
}


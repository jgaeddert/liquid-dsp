/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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
// ordfilt : order-statistics filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  ORDFILT()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

#define LIQUID_ORDFILT_USE_WINDOW 1

#if LIQUID_ORDFILT_USE_WINDOW
int ordfilt_sort_compf(const void * _v1, const void * _v2)
{
    return *(float*)_v1 > *(float*)_v2 ? 1 : -1;
}
#endif

// ordfilt object structure
struct ORDFILT(_s) {
    unsigned int    n;          // buffer length
    unsigned int    k;          // sample index of order statistic
#if LIQUID_ORDFILT_USE_WINDOW
    // simple to implement but much slower
    WINDOW()        buf;        // input buffer
    TI *            buf_sorted; // input buffer (sorted)
#else
    // trickier to implement but faster
    TI *            buf;        // input buffer
    uint16_t *      buf_index;  // buffer index of sorted values
#endif
};

// Create a order-statistic filter (ordfilt) object by specifying
// the buffer size and appropriate sample index of order statistic.
//  _n      : buffer size
//  _k      : sample index for order statistic, 0 <= _k < _n
ORDFILT() ORDFILT(_create)(unsigned int _n,
                           unsigned int _k)
{
    // validate input
    if (_n == 0) {
        fprintf(stderr,"error: ordfilt_%s_create(), filter length must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_k >= _n) {
        fprintf(stderr,"error: ordfilt_%s_create(), filter index must be in [0,n-1]\n", EXTENSION_FULL);
        exit(1);
    }

    // create filter object and initialize
    ORDFILT() q = (ORDFILT()) malloc(sizeof(struct ORDFILT(_s)));
    q->n = _n;
    q->k = _k;

#if LIQUID_ORDFILT_USE_WINDOW
    // create window (internal buffer)
    q->buf        = WINDOW(_create)(q->n);
    q->buf_sorted = (TI*) malloc(q->n * sizeof(TI));
#else
#endif

    // reset filter state (clear buffer)
    ORDFILT(_reset)(q);
    return q;
}

// Create a median filter by specifying buffer semi-length.
//  _m      : buffer semi-length
ORDFILT() ORDFILT(_create_medfilt)(unsigned int _m)
{
    return ORDFILT(_create)(2*_m+1, _m);
}

// destroy ordfilt object
void ORDFILT(_destroy)(ORDFILT() _q)
{
#if LIQUID_ORDFILT_USE_WINDOW
    WINDOW(_destroy)(_q->buf);
    free(_q->buf_sorted);
#else
#endif
    free(_q);
}

// reset internal state of filter object
void ORDFILT(_reset)(ORDFILT() _q)
{
#if LIQUID_ORDFILT_USE_WINDOW
    WINDOW(_reset)(_q->buf);
#else
#endif
}

// print filter object internals (taps, buffer)
void ORDFILT(_print)(ORDFILT() _q)
{
    printf("ordfilt_%s:\n", EXTENSION_FULL);
}

// push sample into filter object's internal buffer
//  _q      :   filter object
//  _x      :   input sample
void ORDFILT(_push)(ORDFILT() _q,
                    TI        _x)
{
#if LIQUID_ORDFILT_USE_WINDOW
    WINDOW(_push)(_q->buf, _x);
#else
#endif
}

// Write block of samples into object's internal buffer
//  _q      : filter object
//  _x      : array of input samples, [size: _n x 1]
//  _n      : number of input elements
void ORDFILT(_write)(ORDFILT()    _q,
                     TI *         _x,
                     unsigned int _n)
{
#if LIQUID_ORDFILT_USE_WINDOW
    WINDOW(_write)(_q->buf, _x, _n);
#else
#endif
}

// compute output sample (dot product between internal
// filter coefficients and internal buffer)
//  _q      :   filter object
//  _y      :   output sample pointer
void ORDFILT(_execute)(ORDFILT() _q,
                       TO *      _y)
{
#if LIQUID_ORDFILT_USE_WINDOW
    // read buffer (retrieve pointer to aligned memory array)
    TI *r;
    WINDOW(_read)(_q->buf, &r);

    // copy to buffer and sort
    memmove(_q->buf_sorted, r, _q->n*sizeof(TI));

    // sort results
    qsort((void*)_q->buf_sorted, _q->n, sizeof(TI), &ordfilt_sort_compf);

    // save output
    *_y = _q->buf_sorted[_q->k];
#else
#endif
}

// execute the filter on a block of input samples; the
// input and output buffers may be the same
//  _q      : filter object
//  _x      : pointer to input array [size: _n x 1]
//  _n      : number of input, output samples
//  _y      : pointer to output array [size: _n x 1]
void ORDFILT(_execute_block)(ORDFILT()    _q,
                             TI *         _x,
                             unsigned int _n,
                             TO *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // push sample into filter
        ORDFILT(_push)(_q, _x[i]);

        // compute output sample
        ORDFILT(_execute)(_q, &_y[i]);
    }
}


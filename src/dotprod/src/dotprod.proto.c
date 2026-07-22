/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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
// Generic dot product
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// forward declaration of internal methods

// select runtime execution method
int DOTPROD(_runtime_detect)(DOTPROD() _q);

// execution methods: always defined but only really implemented on
// specific architectures
int DOTPROD(_execute_port)  (DOTPROD() _q, TI * _x, TO * _y);
int DOTPROD(_execute_neon)  (DOTPROD() _q, TI * _x, TO * _y);
int DOTPROD(_execute_sse)   (DOTPROD() _q, TI * _x, TO * _y);
int DOTPROD(_execute_avx)   (DOTPROD() _q, TI * _x, TO * _y);
int DOTPROD(_execute_avx512)(DOTPROD() _q, TI * _x, TO * _y);

// portable structured dot product object
struct DOTPROD(_s) {
    // coefficients array; note that complex coefficients are
    // split into real/imag to facilitate SIMD operations
#if TC_COMPLEX==0
    T * h;      // real-only coefficients
#else
    T * hi;     // complex in-phase component
    T * hq;     // complex quadrature component
#endif

    // number of coefficients
    unsigned int n;

    // runtime execution method
    int (*execute)(DOTPROD() _q, TI * _x, TO * _y);
};

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int DOTPROD(_run)(TC *         _h,
                  TI *         _x,
                  unsigned int _n,
                  TO *         _y)
{
    // initialize accumulator
    TO r=0;

    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];

    // return result
    *_y = r;
    return LIQUID_OK;
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int DOTPROD(_run4)(TC *         _h,
                   TI *         _x,
                   unsigned int _n,
                   TO *         _y)
{
    // initialize accumulator
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

    // return result
    *_y = r;
    return LIQUID_OK;
}

//
// structured dot product
//

// create vector dot product object
//  _h      : coefficients array, shape: (_n,)
//  _n      : dot product length
//  _rev    : flag indicating if coefficients should be reversed
DOTPROD() DOTPROD(_create_opt)(TC *         _h,
                               unsigned int _n,
                               int          _rev)
{
    DOTPROD() q = (DOTPROD()) malloc(sizeof(struct DOTPROD(_s)));
    q->n = _n;

    // allocate memory for coefficients
    unsigned int i;
#if TC_COMPLEX==0 && TI_COMPLEX==0
    // real-only coefficients
    q->h = (T*) liquid_aligned_alloc(64, (q->n)*sizeof(T));

    // copy coefficients
    for (i=0; i<q->n; i++)
        q->h[i] = _h[_rev ? q->n-i-1 : i];
#elif TC_COMPLEX==0 && TI_COMPLEX==1
    // real-only coefficients (double length for SIMD)
    q->h = (T*) liquid_aligned_alloc(64, 2*(q->n)*sizeof(T));

    // copy coefficients
    for (i=0; i<q->n; i++) {
        unsigned int k = _rev ? q->n-i-1 : i;
        q->h[2*i+0] = _h[k];
        q->h[2*i+1] = _h[k];
    }
#else
    // complex coefficients
    q->hi = (T*) liquid_aligned_alloc(64, 2*(q->n)*sizeof(T));
    q->hq = (T*) liquid_aligned_alloc(64, 2*(q->n)*sizeof(T));

    // set coefficients, repeated
    //  hi = { crealf(_h[0]), crealf(_h[0]), ... crealf(_h[n-1]), crealf(_h[n-1])}
    //  hq = { cimagf(_h[0]), cimagf(_h[0]), ... cimagf(_h[n-1]), cimagf(_h[n-1])}
    for (i=0; i<q->n; i++) {
        unsigned int k = _rev ? q->n-i-1 : i;
        q->hi[2*i+0] = crealf(_h[k]);
        q->hi[2*i+1] = crealf(_h[k]);

        q->hq[2*i+0] = cimagf(_h[k]);
        q->hq[2*i+1] = cimagf(_h[k]);
    }
#endif

    // detect runtime method, defaulting to portable method
    DOTPROD(_runtime_detect)(q);

    // return object
    return q;
}

// create vector dot product object
DOTPROD() DOTPROD(_create)(TC *         _h,
                           unsigned int _n)
{
    return DOTPROD(_create_opt)(_h, _n, 0);
}

// create vector dot product object with time-reversed coefficients
DOTPROD() DOTPROD(_create_rev)(TC *         _h,
                               unsigned int _n)
{
    return DOTPROD(_create_opt)(_h, _n, 1);
}

// re-create dot product object
//  _q      :   old dot dot product object
//  _h      :   new coefficients [size: 1 x _n]
//  _n      :   new dot product size
DOTPROD() DOTPROD(_recreate)(DOTPROD()    _q,
                             TC *         _h,
                             unsigned int _n)
{
    DOTPROD(_destroy)(_q);
    return DOTPROD(_create)(_h, _n);
}

// re-create dot product object with coefficients in reverse order
//  _q      :   old dot dot product object
//  _h      :   time-reversed new coefficients [size: 1 x _n]
//  _n      :   new dot product size
DOTPROD() DOTPROD(_recreate_rev)(DOTPROD()    _q,
                                 TC *         _h,
                                 unsigned int _n)
{
    DOTPROD(_destroy)(_q);
    return DOTPROD(_create_rev)(_h, _n);
}

// copy object
DOTPROD() DOTPROD(_copy)(DOTPROD() q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("dotprod_%s_copy(), dotprod object cannot be NULL", EXTENSION_FULL);

    // create new base object and copy parameters
    DOTPROD() q_copy = (DOTPROD()) malloc(sizeof(struct DOTPROD(_s)));
    q_copy->n = q_orig->n;

    // allocate memory and copy coefficients
#if TC_COMPLEX==0 && TI_COMPLEX==0
    // real-only coefficients
    q_copy->h = (T*) liquid_aligned_alloc(64, (q_copy->n)*sizeof(T));
    memmove(q_copy->h, q_orig->h, (q_copy->n)*sizeof(T));
#elif TC_COMPLEX==0 && TI_COMPLEX==1
    // real-only coefficients (double length)
    q_copy->h = (T*) liquid_aligned_alloc(64, 2*(q_copy->n)*sizeof(T));
    memmove(q_copy->h, q_orig->h, 2*(q_copy->n)*sizeof(T));
#else
    // complex coefficients
    q_copy->hi = (T*) liquid_aligned_alloc(64, 2*(q_copy->n)*sizeof(T));
    q_copy->hq = (T*) liquid_aligned_alloc(64, 2*(q_copy->n)*sizeof(T));

    // copy coefficients
    memmove(q_copy->hi, q_orig->hi, 2*(q_copy->n)*sizeof(T));
    memmove(q_copy->hq, q_orig->hq, 2*(q_copy->n)*sizeof(T));
#endif

    // copy execution method
    q_copy->execute = q_orig->execute;

    // return new object
    return q_copy;
}

// destroy dot product object
int DOTPROD(_destroy)(DOTPROD() _q)
{
    // free coefficients memory
#if TC_COMPLEX==0
    liquid_aligned_free(_q->h);
#else
    liquid_aligned_free(_q->hi);
    liquid_aligned_free(_q->hq);
#endif

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

// print dot product object
int DOTPROD(_print)(DOTPROD() _q)
{
    printf("dotprod [portable, %u coefficients]:\n", _q->n);
    return LIQUID_OK;
}

// select runtime execution method
int DOTPROD(_runtime_select)(DOTPROD()        _q,
                             liquid_runtime_t _select)
{
    switch (_select) {
    case LIQUID_RUNTIME_PORT:
        liquid_log_trace("dotprod_%s_runtime_select(), port", EXTENSION_FULL);
        _q->execute = &DOTPROD(_execute_port);
        return LIQUID_OK;
    case LIQUID_RUNTIME_NEON:
        liquid_log_trace("dotprod_%s_runtime_select(), neon", EXTENSION_FULL);
        _q->execute = &DOTPROD(_execute_neon);
        return LIQUID_OK;
    case LIQUID_RUNTIME_SSE:
        liquid_log_trace("dotprod_%s_runtime_select(), sse", EXTENSION_FULL);
        _q->execute = &DOTPROD(_execute_sse);
        return LIQUID_OK;
    case LIQUID_RUNTIME_AVX:
        liquid_log_trace("dotprod_%s_runtime_select(), avx", EXTENSION_FULL);
        _q->execute = &DOTPROD(_execute_avx);
        return LIQUID_OK;
    case LIQUID_RUNTIME_AVX512:
        liquid_log_trace("dotprod_%s_runtime_select(), avx512", EXTENSION_FULL);
        _q->execute = &DOTPROD(_execute_avx512);
        return LIQUID_OK;
    default:;
    }

    _q->execute = &DOTPROD(_execute_port);
    return liquid_error(LIQUID_EINT,
        "dotprod_%s_runtime_select(), invalid selection or mode not available (%d), falling back to portable version",
        EXTENSION_FULL, _select);
}

// execute structured dot product
//  _q      :   dot product object
//  _x      :   input array [size: 1 x _n]
//  _y      :   output dot product
int DOTPROD(_execute)(DOTPROD() _q,
                      TI *      _x,
                      TO *      _y)
{
    // invoke runtime-specific method
    return _q->execute(_q, _x, _y);
}

//
// internal
//

// detect runtime execution method
int DOTPROD(_runtime_detect)(DOTPROD() _q)
{
    // implementations
    struct liquid_cpuinfo_s impl =
    {
        .altivec = 0,
        .neon    = true,
        .mmx     = 0,
        .sse     = true,
        .sse2    = 0,
        .sse3    = 0,
        .ssse3   = 0,
        .sse41   = 0,
        .sse42   = 0,
        .avx     = true,
        .fma3    = 0,
        .avx2    = 0,
        .avx512  = true,
        .amx     = 0,
        .amx101  = 0,
        .amx102  = 0,
    };

    // given implementations, find best method
    liquid_runtime_t selection = liquid_runtime_detect(&impl);

    // invoke selection method
    return DOTPROD(_runtime_select)(_q, selection);
}

// execute structured dot product (portable version)
//  _q      :   dot product object
//  _x      :   input array [size: 1 x _n]
//  _y      :   output dot product
int DOTPROD(_execute_port)(DOTPROD() _q,
                           TI *      _x,
                           TO *      _y)
{
    unsigned int i;
#if TC_COMPLEX==0 && TI_COMPLEX==0
    // accumulate
    TO r=0;
    for (i=0; i<_q->n; i++)
        r += _q->h[i] * _x[i];

    // return result
    *_y = r;
#elif TC_COMPLEX==0 && TI_COMPLEX==1
    // accumulate (stride coefficients by 2)
    TO r=0;
    for (i=0; i<_q->n; i++)
        r += _q->h[2*i+0] * _x[i];

    // return result
    *_y = r;
#else
    // type-cast input pointer as real-valued samples
    T* x = (T*)_x;

    // initialize accumulators
    T ci0=0, ci1=0, cq0=0, cq1=0;

    // cross-multiply with duplicated coefficents
    // { x[0].real,  x[0].imag,  x[1].real,  x[1].imag, ...}
    // {hi[0].real, hi[0].real, hi[1].real, hi[1].real, ...}
    // {hq[0].imag, hq[0].imag, hq[1].imag, hq[1].imag, ...}
    for (i=0; i<_q->n; i++)
    {
        ci0 += _q->hi[2*i+0] * x[2*i+0];
        ci1 += _q->hi[2*i+1] * x[2*i+1];

        cq0 += _q->hq[2*i+0] * x[2*i+0];
        cq1 += _q->hq[2*i+1] * x[2*i+1];
    }

    // fold down and return result
    *_y = (ci0 - cq1) + (ci1 + cq0)*_Complex_I;
#endif
    return LIQUID_OK;
}


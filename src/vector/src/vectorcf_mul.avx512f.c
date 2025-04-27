/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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
// AVX512 vector multiplication
//

#include "liquid.internal.h"
#include <immintrin.h>

// basic vector multiplication, unrolling loop
//  _x      :   first array  [size: _n x 1]
//  _y      :   second array [size: _n x 1]
//  _n      :   array lengths
//  _z      :   output array pointer [size: _n x 1]
void liquid_vectorcf_mul(float complex *_x,
                         float complex *_y,
                         unsigned int   _n,
                         float complex *_z)
{
    // type cast as floating point array
    float * x = (float*) _x;
    float * y = (float*) _y;
    float * z = (float*) _z;

    // double effective length
    unsigned int n = 2*_n;

    // temporary buffers
    __m512 rx, ry, rz;
    __m512 one = _mm512_set1_ps(1.0f);

    // t = 16*(floor(_n/16))
    unsigned int t = (n >> 4) << 4;

    unsigned int i;
    for (i=0; i<t; i+=16) {
        // Check comments on SSE/AVX versions for more details, this is just AVX but double the size
        rx = _mm512_loadu_ps(&x[i]);
        ry = _mm512_loadu_ps(&y[i]);

        rz = _mm512_moveldup_ps(rx);
        rz = _mm512_mul_ps(rx, ry);
        ry = _mm512_shuffle_ps(ry, ry, _MM_SHUFFLE(2, 3, 0, 1));
        rx = _mm512_movehdup_ps(rx);
        rx = _mm512_mul_ps(rx, ry);
        rz = _mm512_fmaddsub_ps(rz, one, rx); // FIXME: Can probably be optimized to take advantage of fused multiply-add

        _mm512_storeu_ps(&z[i], rz);
    }

    // clean up remaining
    for (i=t/2; i<_n; i++)
        _z[i] = _x[i] * _y[i];
}

// basic vector scalar multiplication, unrolling loop
//  _x      :   input array  [size: _n x 1]
//  _n      :   array length
//  _v      :   scalar
//  _y      :   output array pointer [size: _n x 1]
void liquid_vectorcf_mulscalar(float complex *_x,
                               unsigned int   _n,
                               float complex  _v,
                               float complex *_y)
{
    // type cast as floating point array
    float * x = (float*) _x;
    float * y = (float*) _y;

    // double effective length
    unsigned int n = 2*_n;

    // temporary buffers
    __m512 rx, rz;
    __m512 ry = _mm512_set_ps(cimagf(_v), crealf(_v), cimagf(_v), crealf(_v), cimagf(_v), crealf(_v), cimagf(_v), crealf(_v),
                              cimagf(_v), crealf(_v), cimagf(_v), crealf(_v), cimagf(_v), crealf(_v), cimagf(_v), crealf(_v));
    __m512 ry_shuf = _mm512_shuffle_ps(ry, ry, _MM_SHUFFLE(2, 3, 0, 1));
    __m512 one = _mm512_set1_ps(1.0f);

    // t = 16*(floor(_n/16))
    unsigned int t = (n >> 4) << 4;

    unsigned int i;
    for (i=0; i<t; i+=16) {
        // Check comments on SSE/AVX versions for more details, this is just AVX but double the size
        rx = _mm512_loadu_ps(&x[i]);

        rz = _mm512_moveldup_ps(rx);
        rz = _mm512_mul_ps(rx, ry);
        rx = _mm512_movehdup_ps(rx);
        rx = _mm512_mul_ps(rx, ry_shuf);
        rz = _mm512_fmaddsub_ps(rz, one, rx); // FIXME: Can probably be optimized to take advantage of fused multiply-add

        _mm512_storeu_ps(&y[i], rz);
    }

    // clean up remaining
    for (i=t/2; i<_n; i++)
        _y[i] = _x[i] * _v;
}
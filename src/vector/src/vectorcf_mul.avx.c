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
// AVX vector multiplication
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
    __m256 rx, ry, rz;

    // t = 8*(floor(_n/8))
    unsigned int t = (n >> 3) << 3;

    unsigned int i;
    for (i=0; i<t; i+=8) {
        // rx = {b3, a3, b2, a2, b1, a1, b0, a0}
        rx = _mm256_loadu_ps(&x[i]);
        // ry = {d3, c3, d2, c2, d1, c1, d0, c0}
        ry = _mm256_loadu_ps(&y[i]);

        // rz = {a3, a3, a2, a2, a1, a1, a0, a0}
        rz = _mm256_moveldup_ps(rx);
        // rz = {a3*d3, a3*c3, a2*d2, a2*c2, a1*d1, a1*c1, a0*d0, a0*c0}
        rz = _mm256_mul_ps(rx, ry);
        // ry = {c3, d3, c2, d2, c1, d1, c0, d0}
        ry = _mm256_shuffle_ps(ry, ry, _MM_SHUFFLE(2, 3, 0, 1));
        // rx = {b3, b3, b2, b2, b1, b1, b0, b0}
        rx = _mm256_movehdup_ps(rx);
        // rx = {b3*c3, b3*d3, b2*c2, b2*d2, b1*c1, b1*d1, b0*c0, b0*d0}
        rx = _mm256_mul_ps(rx, ry);
        // rz = {a3*d3+b3*c3, a3*c3-b3*d3, a2*d2+b2*c2, a2*c2-b2*d2, a1*d1+b1*c1, a1*c1-b1*d1, a0*d0+b0*c0, a0*c0-b0*d0}
        rz = _mm256_addsub_ps(rz, rx);

        _mm256_storeu_ps(&z[i], rz);
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
    __m256 rx, rz;
    // ry = {d1, c1, d0, c0}
    __m256 ry = _mm256_set_ps(cimagf(_v), crealf(_v), cimagf(_v), crealf(_v), cimagf(_v), crealf(_v), cimagf(_v), crealf(_v));
    // ry_shuf = {c1, d1, c0, d0}
    __m256 ry_shuf = _mm256_shuffle_ps(ry, ry, _MM_SHUFFLE(2, 3, 0, 1));

    // t = 8*(floor(_n/8))
    unsigned int t = (n >> 3) << 3;

    unsigned int i;
    for (i=0; i<t; i+=8) {
        // rx = {b3, a3, b2, a2, b1, a1, b0, a0}
        rx = _mm256_loadu_ps(&x[i]);

        // rz = {a3, a3, a2, a2, a1, a1, a0, a0}
        rz = _mm256_moveldup_ps(rx);
        // rz = {a3*d3, a3*c3, a2*d2, a2*c2, a1*d1, a1*c1, a0*d0, a0*c0}
        rz = _mm256_mul_ps(rx, ry);
        // rx = {b3, b3, b2, b2, b1, b1, b0, b0}
        rx = _mm256_movehdup_ps(rx);
        // rx = {b3*c3, b3*d3, b2*c2, b2*d2, b1*c1, b1*d1, b0*c0, b0*d0}
        rx = _mm256_mul_ps(rx, ry_shuf);
        // rz = {a3*d3+b3*c3, a3*c3-b3*d3, a2*d2+b2*c2, a2*c2-b2*d2, a1*d1+b1*c1, a1*c1-b1*d1, a0*d0+b0*c0, a0*c0-b0*d0}
        rz = _mm256_addsub_ps(rz, rx);

        _mm256_storeu_ps(&y[i], rz);
    }

    // clean up remaining
    for (i=t/2; i<_n; i++)
        _y[i] = _x[i] * _v;
}
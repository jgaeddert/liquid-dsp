/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

// vector multiplication (AVX)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <immintrin.h>

void liquid_vectorf_mul(const float * _v0,
                        const float * _v1,
                        unsigned int _n,
                        float *      _y)
{
    // AVX registers
    __m256 v0;  // input vector 0
    __m256 v1;  // input vector 1
    __m256 y;   // output

    // t = 8*(floor(_n/8))
    unsigned int t = (_n >> 3) << 3;

    // operate in blocks of 8 samples (register size)
    unsigned int i;
    for (i=0; i<t; i+=8) {
        // load inputs into register (unaligned)
        v0 = _mm256_loadu_ps(&_v0[i]);
        v1 = _mm256_loadu_ps(&_v1[i]);

        // compute product
        y = _mm256_mul_ps(v0, v1);

        // unload packed array (unaligned)
        _mm256_storeu_ps(&_y[i], y);
    }

    // cleanup (residual multiplications that do not fit in register)
    for (; i<_n; i++)
        _y[i] = _v0[i] * _v1[i];
}

// basic vector scalar multiplication, unrolling loop
void liquid_vectorf_mulscalar(const float * _v,
                              unsigned int _n,
                              float        _s,
                              float *      _y)
{
    // AVX registers
    __m256 v0;  // input vector
    __m256 v1;  // scalar vector
    __m256 y;   // output

    // t = 8*(floor(_n/8))
    unsigned int t = (_n >> 3) << 3;

    // aligned scalar array
    float s[8] __attribute__((aligned(32))) = {_s,_s,_s,_s,_s,_s,_s,_s,};
    v1 = _mm256_load_ps(s);

    // operate in blocks of 8 samples (register size)
    unsigned int i;
    for (i=0; i<t; i+=8) {
        // load inputs into register (unaligned)
        v0 = _mm256_loadu_ps(&_v[i]);

        // compute product
        y = _mm256_mul_ps(v0, v1);

        // unload packed array (unaligned)
        _mm256_storeu_ps(&_y[i], y);
    }

    // cleanup (residual multiplications that do not fit in register)
    for (; i<_n; i++)
        _y[i] = _v[i] * _s;
}


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

// vector multiplication (ARM Neon)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arm_neon.h>

// vector multiplication
// equivalent to:
// {
//    unsigned int i;
//    for (i=0; i<_n; i++)
//        _y[i] = _v0[i] * _v1[i];
// }
void liquid_vectorcf_mul(liquid_float_complex * _v0,
                         liquid_float_complex * _v1,
                         unsigned int    _n,
                         liquid_float_complex * _y)
{
    // type cast input as floating point array
    float * v0 = (float*) _v0;
    float * v1 = (float*) _v1;
    float * y  = (float*) _y;

    // double effective length
    unsigned int n = 2*_n;

    // output accumulators
    LIQUID_VLA(float, w0, 4);
    LIQUID_VLA(float, w1, 4);

    // t = 4*(floor(_n/4))
    unsigned int t = (n >> 2) << 2;
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load input
        // r0: {v0[0].real, v0[0].imag, v0[1].real, v0[1].imag} = {a, b, e, f}
        // r1: {v1[0].real, v1[0].imag, v1[1].real, v1[1].imag} = {c, d, g, h}
        float32x4_t _r0 = vld1q_f32(&v0[i]);
        float32x4_t _r1 = vld1q_f32(&v1[i]);

        // NOTE: vref64q_f32(0,1,2,3) -> (1,0,3,2)
        // w0: {a*c, b*d, e*g, f*h}
        // w1: {a*d, b*c, e*h, f*g}
        float32x4_t _w0 = vmulq_f32(_r0, _r1);
        float32x4_t _w1 = vmulq_f32(_r0, vrev64q_f32(_r1));

        // unload and combine
        vst1q_f32(w0, _w0);
        vst1q_f32(w1, _w1);

        y[i+0] = w0[0] - w0[1]; // a*c - b*d
        y[i+1] = w1[0] + w1[1]; // a*d + b*c
        y[i+2] = w0[2] - w0[3]; // e*g - f*h
        y[i+3] = w1[2] + w1[3]; // e*h + f*g
    }

    // cleanup
    for (i=t/2; i<_n; i++)
        _y[i] = _v0[i] * _v1[i];
}

// vector scalar multiplication
void liquid_vectorcf_mulscalar(liquid_float_complex * _v,
                               unsigned int    _n,
                               liquid_float_complex   _s,
                               liquid_float_complex * _y)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        _y[i] = _v[i] * _s;
}


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

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: Kaiser-Bessel derived window
//
void liquid_kbd_window_test(unsigned int _n,
                            float _beta)
{
    unsigned int i;
    float tol = 1e-3f;

    // compute window
    float w[_n];
    liquid_kbd_window(_n,_beta,w);

    // square window
    float w2[_n];
    for (i=0; i<_n; i++)
        w2[i] = w[i]*w[i];

    // ensure w[i]^2 + w[i+M]^2 == 1
    unsigned int M = _n/2;
    for (i=0; i<M; i++)
        CONTEND_DELTA(w2[i]+w2[(i+M)%_n], 1.0f, tol);

    // ensure sum(w[i]^2) == _n/2
    float sum=0.0f;
    for (i=0; i<_n; i++)
        sum += w2[i];
    CONTEND_DELTA(sum, 0.5f*_n, tol);
}

void autotest_kbd_n16() { liquid_kbd_window_test(16, 10.0f); }
void autotest_kbd_n32() { liquid_kbd_window_test(32, 20.0f); }
void autotest_kbd_n48() { liquid_kbd_window_test(48, 12.0f); }


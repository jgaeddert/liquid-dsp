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
#include "liquid.internal.h"

void autotest_iirfiltsos_rrrq16_n2()
{
    // error tolerance
    float tol = 1e-2f;

    // initialize filter with 2nd-order low-pass butterworth filter
    q16_t a[3] = {
        q16_float_to_fixed( 1.000000000000000),
        q16_float_to_fixed(-0.942809041582063),
        q16_float_to_fixed( 0.333333333333333)};

    q16_t b[3] = {
        q16_float_to_fixed( 0.0976310729378175),
        q16_float_to_fixed( 0.1952621458756350),
        q16_float_to_fixed( 0.0976310729378175)};

    // create filter object
    iirfiltsos_rrrq16 q = iirfiltsos_rrrq16_create(b,a);

    // initialize oracle; expected output (generated with octave)
    float test[15] = {
       9.76310729378175e-02,
       2.87309604180767e-01,
       3.35965474513536e-01,
       2.20981418970514e-01,
       9.63547883225231e-02,
       1.71836926400291e-02,
      -1.59173219853878e-02,
      -2.07348926322729e-02,
      -1.42432702548109e-02,
      -6.51705310050832e-03,
      -1.39657983602602e-03,
       8.55642936806248e-04,
       1.27223450919543e-03,
       9.14259886013424e-04,
       4.37894317157432e-04};

    unsigned int i;
    q16_t x, y;

    // hit filter with impulse, compare output
    for (i=0; i<15; i++) {
        // set input (impulse)
        x = (i==0) ? q16_one : q16_zero;

        // compute output
        iirfiltsos_rrrq16_execute(q, x, &y);

        // convert to floating-point
        float yf = q16_fixed_to_float(y);

        // check result
        CONTEND_DELTA(test[i], yf, tol);
    }

    // destroy object
    iirfiltsos_rrrq16_destroy(q);
}


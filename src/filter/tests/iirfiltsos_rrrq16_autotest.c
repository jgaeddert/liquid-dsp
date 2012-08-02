/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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


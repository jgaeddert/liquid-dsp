/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

void autotest_iirfiltsos_impulse_n2()
{
    // initialize filter with 2nd-order low-pass butterworth filter
    float a[3] = {
        1.000000000000000,
       -0.942809041582063,
        0.333333333333333};

    float b[3] = {
        0.0976310729378175,
        0.1952621458756350,
        0.0976310729378175};

    iirfiltsos_rrrf f = iirfiltsos_rrrf_create(b,a);

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
    float v, y;
    float tol=1e-4f;

    // hit filter with impulse, compare output
    for (i=0; i<15; i++) {
        v = (i==0) ? 1.0f : 0.0f;

        iirfiltsos_rrrf_execute(f, v, &y);

        CONTEND_DELTA(test[i], y, tol);
    }

    iirfiltsos_rrrf_destroy(f);
}


void autotest_iirfiltsos_step_n2()
{
    // initialize filter with 2nd-order low-pass butterworth filter
    float a[3] = {
        1.000000000000000,
       -0.942809041582063,
        0.333333333333333};

    float b[3] = {
        0.0976310729378175,
        0.1952621458756350,
        0.0976310729378175};

    iirfiltsos_rrrf f = iirfiltsos_rrrf_create(b,a);

    float test[15] = {
       0.0976310729378175,
       0.3849406771185847,
       0.7209061516321208,
       0.9418875706026352,
       1.0382423589251584,
       1.0554260515651877,
       1.0395087295798000,
       1.0187738369475272,
       1.0045305666927162,
       0.9980135135922078,
       0.9966169337561817,
       0.9974725766929878,
       0.9987448112021832,
       0.9996590710881966,
       1.0000969654053542};

    unsigned int i;
    float y;
    float tol=1e-4f;

    // hit filter with step, compare output
    for (i=0; i<15; i++) {
        iirfiltsos_rrrf_execute(f, 1.0f, &y);

        CONTEND_DELTA(test[i], y, tol );
    }

    iirfiltsos_rrrf_destroy(f);
}


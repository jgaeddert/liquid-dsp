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

void autotest_compand_float() {
    float x = -1.0f;
    float mu=255.0f;
    unsigned int n=30;

    float dx = 2/(float)(n);
    float y;
    float x_hat;
    float tol = 1e-6f;

    unsigned int i;
    for (i=0; i<n; i++) {
        y = compress_mulaw(x,mu);
        x_hat = expand_mulaw(y,mu);

        if (liquid_autotest_verbose)
            printf("%8.4f -> %8.4f -> %8.4f\n", x, y, x_hat);

        CONTEND_DELTA(x,x_hat,tol);

        x += dx;
        x = (x > 1.0f) ? 1.0f : x;
    }
}


void autotest_compand_cfloat() {
    float complex x = -0.707f - 0.707f*_Complex_I;
    float mu=255.0f;
    unsigned int n=30;

    float complex dx = 2*(0.707f +0.707f* _Complex_I)/(float)(n);
    float complex y;
    float complex z;
    float tol = 1e-6f;

    unsigned int i;
    for (i=0; i<n; i++) {
        compress_cf_mulaw(x,mu,&y);
        expand_cf_mulaw(y,mu,&z);

        if (liquid_autotest_verbose) {
            printf("%8.4f +j%8.4f > ", crealf(x), cimagf(x));
            printf("%8.4f +j%8.4f > ", crealf(y), cimagf(y));
            printf("%8.4f +j%8.4f\n",  crealf(z), cimagf(z));
        }

        CONTEND_DELTA(crealf(x),crealf(z),tol);
        CONTEND_DELTA(cimagf(x),cimagf(z),tol);

        x += dx;
        //x = (x > 1.0f) ? 1.0f : x;
    }
}


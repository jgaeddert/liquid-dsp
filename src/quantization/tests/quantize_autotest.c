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

void autotest_quantize_float_n8() {
    float x = -1.0f;
    unsigned int num_steps=30;
    unsigned int num_bits=8;

    float dx = 2/(float)(num_steps);
    unsigned int q;
    float x_hat;
    float tol = 1.0f / (float)(1<<num_bits);

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        q = quantize_adc(x,num_bits);

        // ensure only num_bits written to value q
        CONTEND_EQUALITY(q>>num_bits, 0);

        x_hat = quantize_dac(q,num_bits);

        if (liquid_autotest_verbose)
            printf("%8.4f > 0x%2.2x > %8.4f\n", x, q, x_hat);

        // ensure original value is recovered withing tolerance
        CONTEND_DELTA(x,x_hat,tol);

        x += dx;
        x = (x > 1.0f) ? 1.0f : x;
    }
}


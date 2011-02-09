/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

//
// math_clogf_test.c
//
// complex logarithm test
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

#define sandbox_randf() ((float) rand() / (float) RAND_MAX)

float complex sandbox_clogf(float complex _z)
{
    return logf(cabsf(_z)) + _Complex_I*cargf(_z);
}

int main() {
    unsigned int n=10;

    unsigned int i;
    for (i=0; i<n; i++) {
        // generate random complex number
        float complex z = 4.0f*(2.0f*sandbox_randf() - 1.0f) +
                          4.0f*(2.0f*sandbox_randf() - 1.0f) * _Complex_I;

        float complex logz = clogf(z);
        float complex logz_hat = sandbox_clogf(z);

        float complex err = logz - logz_hat;

        printf("%3u: z=%6.2f+j%6.2f, log(z)=%6.2f+j%6.2f (%6.2f+j%6.2f) e=%12.4e\n",
                i,
                crealf(z),          cimagf(z),
                crealf(logz),       cimagf(logz),
                crealf(logz_hat),   cimagf(logz_hat),
                cabsf(err));
    }

    printf("done.\n");
    return 0;
}


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
// math_cacosf_test.c
//
// complex arc-cosine test
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

#define sandbox_randf() ((float) rand() / (float) RAND_MAX)

float complex sandbox_cacosf(float complex _z)
{
    // return based on quadrant
    int sign_i = crealf(_z) > 0;
    int sign_q = cimagf(_z) > 0;

    if (sign_i == sign_q) {
        return - _Complex_I*clogf(_z + csqrtf(_z*_z - 1.0f));
    } else {
        return - _Complex_I*clogf(_z - csqrtf(_z*_z - 1.0f));
    }

    // should never get to this state
    return 0.0f;
}

int main() {
    unsigned int n=40;

    float complex err_max = 0.0f;
    unsigned int i;
    for (i=0; i<n; i++) {
        // generate random complex number
        float complex z = 2.0f*(2.0f*sandbox_randf() - 1.0f) +
                          2.0f*(2.0f*sandbox_randf() - 1.0f) * _Complex_I;

        float complex acosz     = cacosf(z);
        float complex acosz_hat = sandbox_cacosf(z);

        float complex err = acosz - acosz_hat;

        printf("%3u: z=%6.2f+j%6.2f, acos(z)=%6.2f+j%6.2f (%6.2f+j%6.2f) e=%12.4e\n",
                i,
                crealf(z),          cimagf(z),
                crealf(acosz),      cimagf(acosz),
                crealf(acosz_hat),  cimagf(acosz_hat),
                cabsf(err));

        if ( cabsf(err) > cabsf(err_max) )
            err_max = err;
    }

    printf("maximum error: %12.4e;\n", cabsf(err_max));

    printf("done.\n");
    return 0;
}


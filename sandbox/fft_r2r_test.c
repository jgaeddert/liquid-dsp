/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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

//
// Real even/odd DFTs (discrete cosine/sine transforms)
//

// compile and run:
//   $ gcc -Wall -I/opt/local/include -I. -I./include -c fft_r2r_test.c -o fft_r2r_test.o 
//   $ gcc fft_r2r_test.o -lm -lc -lfftw3 -o fft_r2r_test 
//   $ ./fft_r2r_test

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

int main(int argc, char*argv[]) {
    unsigned int n = 8;

    // create and initialize data arrays
    double x[n];
    double y[n];
    unsigned int i;
    for (i=0; i<n; i++)
        x[i] = (double)(i);

    // create fftw plans
    fftw_plan plan[8];
    plan[0] = fftw_plan_r2r_1d(n, x, y, FFTW_REDFT00, FFTW_ESTIMATE);
    plan[1] = fftw_plan_r2r_1d(n, x, y, FFTW_REDFT10, FFTW_ESTIMATE);
    plan[2] = fftw_plan_r2r_1d(n, x, y, FFTW_REDFT01, FFTW_ESTIMATE);
    plan[3] = fftw_plan_r2r_1d(n, x, y, FFTW_REDFT11, FFTW_ESTIMATE);

    plan[4] = fftw_plan_r2r_1d(n, x, y, FFTW_RODFT00, FFTW_ESTIMATE);
    plan[5] = fftw_plan_r2r_1d(n, x, y, FFTW_RODFT10, FFTW_ESTIMATE);
    plan[6] = fftw_plan_r2r_1d(n, x, y, FFTW_RODFT01, FFTW_ESTIMATE);
    plan[7] = fftw_plan_r2r_1d(n, x, y, FFTW_RODFT11, FFTW_ESTIMATE);

    printf("executing plans...\n");
    // execute plans and print
    for (i=0; i<8; i++) {
        fftw_execute(plan[i]);

        unsigned int j;
        printf("plan %u:\n", i);
        for (j=0; j<n; j++)
            printf("  %12.8f\n", y[j]);
    }

    // destroy plans
    for (i=0; i<8; i++)
        fftw_destroy_plan(plan[i]);

    printf("done.\n");
    return 0;
}


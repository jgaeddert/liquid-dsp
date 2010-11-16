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
//   $ gcc fft_r2r_test.o -lm -lc -lfftw3f -o fft_r2r_test 
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
    float x[n];
    float y[n];
    unsigned int i;
    for (i=0; i<n; i++)
        x[i] = (float)(i);

    // create fftw plans
    fftwf_plan plan[8];
    plan[0] = fftwf_plan_r2r_1d(n, x, y, FFTW_REDFT00, FFTW_ESTIMATE);
    plan[1] = fftwf_plan_r2r_1d(n, x, y, FFTW_REDFT10, FFTW_ESTIMATE);
    plan[2] = fftwf_plan_r2r_1d(n, x, y, FFTW_REDFT01, FFTW_ESTIMATE);
    plan[3] = fftwf_plan_r2r_1d(n, x, y, FFTW_REDFT11, FFTW_ESTIMATE);

    plan[4] = fftwf_plan_r2r_1d(n, x, y, FFTW_RODFT00, FFTW_ESTIMATE);
    plan[5] = fftwf_plan_r2r_1d(n, x, y, FFTW_RODFT10, FFTW_ESTIMATE);
    plan[6] = fftwf_plan_r2r_1d(n, x, y, FFTW_RODFT01, FFTW_ESTIMATE);
    plan[7] = fftwf_plan_r2r_1d(n, x, y, FFTW_RODFT11, FFTW_ESTIMATE);

    char plan_name[][8] = {"REDFT00",
                           "REDFT10",
                           "REDFT01",
                           "REDFT11",
                           "RODFT00",
                           "RODFT10",
                           "RODFT01",
                           "RODFT11"};

    printf("// %u-point real even/odd dft data\n", n);
    printf("float fftdata_r2r_n%u[] = {\n", n);
    for (i=0; i<n; i++)
        printf("  %16.10f%s\n", x[i], i==(n-1) ? "};" : ",");

    // execute plans and print
    for (i=0; i<8; i++) {
        fftwf_execute(plan[i]);

        unsigned int j;
        printf("\n");
        printf("// %s\n", plan_name[i]);
        printf("float fftdata_r2r_%s_n%u[] = {\n", plan_name[i], n);
        for (j=0; j<n; j++)
            printf("  %16.10f%s\n", y[j], j==(n-1) ? "};" : ",");
    }

    // destroy plans
    for (i=0; i<8; i++)
        fftwf_destroy_plan(plan[i]);

    return 0;
}


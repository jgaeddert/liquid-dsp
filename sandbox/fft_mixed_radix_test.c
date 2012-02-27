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
// 
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <getopt.h>
#include <complex.h>
#include "liquid.h"

// print usage/help message
void usage()
{
    printf("fft_r2r_test -- test real ffts, compare to fftw3\n");
    printf("options (default values in []):\n");
    printf("  u/h   : print usage/help\n");
    printf("  p     : stride (freq)\n");
    printf("  q     : stride (time)\n");
}

int main(int argc, char*argv[]) {
    // transform size: p*q
    unsigned int p = 3;
    unsigned int q = 5;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhp:q:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();          return 0;
        case 'p': p = atoi(optarg); break;
        case 'q': q = atoi(optarg); break;
        default:
            exit(1);
        }
    }

    // transform size
    unsigned int n = p*q;

    // validate input
    if ( n == 0 ) {
        fprintf(stderr,"error: input transform size must be at least 2\n");
        exit(1);
    }

    unsigned int i;
    unsigned int j;
    unsigned int k;

    // create and initialize data arrays
    float complex x[n];
    float complex y[n];
    float complex y_test[n];
    for (i=0; i<n; i++) {
        //x[i] = randnf() + _Complex_I*randnf();
        x[i] = (float)i + _Complex_I*(3 - (float)i);
    }

    // compute output for testing
    fft_run(n, x, y_test, FFT_FORWARD, 0);

    //
    // run Cooley-Tukey FFT
    //

    // compute twiddle factors (roots of unity)
    float complex twiddle[n];
    for (i=0; i<n; i++)
        twiddle[i] = cexpf(_Complex_I*2*M_PI*(float)i / (float)n);

    // compute 'q' DFTs of size 'p' and multiply by twiddle factors
    for (i=0; i<n; i++)
        y[i] = 0.0f;

    // compute 'p' DFTs of size 'q' and transpose


    // 
    // print results
    //
    for (i=0; i<n; i++) {
        printf("  y[%3u] = %12.8f + j*%12.8f (expected %12.8f + j%12.8f)\n",
            i,
            crealf(y[i]),      cimagf(y[i]),
            crealf(y_test[i]), cimagf(y_test[i]));
    }

    // compute error
    float rmse = 0.0f;
    for (i=0; i<n; i++) {
        float e = y[i] - y_test[i];
        rmse += e*conjf(e);
    }
    rmse = sqrtf(rmse / (float)n);
    printf("RMS error : %12.4e\n", rmse);

    return 0;
}


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
// Test mixed-radix FFT algorithm
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <getopt.h>
#include <complex.h>

#define DEBUG 0
#define DFT_FORWARD (-1)
#define DFT_REVERSE ( 1)
#define MAX_FACTORS (32)

// print usage/help message
void usage()
{
    printf("fft_mixed_radix_test -- test mixed-radix DFTs, compare to slow DFT method\n");
    printf("options (default values in []):\n");
    printf("  h     : print usage/help\n");
    printf("  n     : fft size\n");
}

// super slow DFT, but functionally correct
void dft_run(unsigned int    _nfft,
             float complex * _x,
             float complex * _y,
             int             _dir,
             int             _flags);

int main(int argc, char*argv[]) {
    // transform size
    unsigned int nfft = 30;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhn:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();                return 0;
        case 'n':   nfft = atoi(optarg);    break;
        default:
            exit(1);
        }
    }

    // validate input
    if ( nfft == 0 ) {
        fprintf(stderr,"error: input transform size must be at least 2\n");
        exit(1);
    }

    unsigned int i;
    unsigned int k;
    
    // find 'prime' factors
    unsigned int n = nfft;
    unsigned int q[MAX_FACTORS];
    unsigned int p[MAX_FACTORS];
    unsigned int num_factors = 0;

    do {
        for (k=2; k<=n; k++) {
            if ( (n%k)==0 ) {
                q[num_factors] = k;
                p[num_factors] = n/k;
                num_factors++;
                n /= k;
                break;
            }
        }
    } while (n > 1 && num_factors < MAX_FACTORS);

    printf("factors of %u:", nfft);
    for (i=0; i<num_factors; i++)
        printf("  %u(%u)", q[i], p[i]);
    printf("\n");

    return 0;
}


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

// FFT mixed-radix generic butterfly
//  _x          :   input/output buffer pointer [size: _nfft x 1]
//  _twiddle    :   pre-computed twiddle factors [size: _nfft x 1]
//  _nfft       :   original fft size
//  _offset     :   input buffer offset
//  _q          :   input buffer stride
//  _m          :   number of FFTs to compute
//  _p          :   generic (small) FFT size
//
// NOTES : the butterfly decimates in time, storing the output as
//         contiguous samples in the same buffer.
void fftmr_bfly_generic(float complex * _x,
                        float complex * _twiddle,
                        unsigned int    _nfft,
                        unsigned int    _offset,
                        unsigned int    _q,
                        unsigned int    _m,
                        unsigned int    _p);

// FFT mixed-radix recursive function...
//  _x          :   input pointer [size: _nfft x 1]
//  _y          :   output pointer [size: _nfft x 1]
//  _twiddle    :   pre-computed twiddle factors [size: _nfft x 1]
//  _nfft       :   original FFT size
//  _M          :   
//  _P          :   
void fftmr_cycle(float complex * _x,
                 float complex * _y,
                 float complex * _twiddle,
                 unsigned int    _nfft,
                 unsigned int  * _P);
                      

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
    unsigned int p[MAX_FACTORS];
    unsigned int m[MAX_FACTORS];
    unsigned int num_factors = 0;

    do {
        for (k=2; k<=n; k++) {
            if ( (n%k)==0 ) {
                p[num_factors] = k;
                m[num_factors] = n/k;
                num_factors++;
                n /= k;
                break;
            }
        }
    } while (n > 1 && num_factors < MAX_FACTORS);

    printf("factors of %u:\n", nfft);
    for (i=0; i<num_factors; i++)
        printf("  p=%3u, m=%3u\n", p[i], m[i]);

    // compute twiddle factors (roots of unity)
    float complex twiddle[n];
    for (i=0; i<n; i++)
        twiddle[i] = cexpf(-_Complex_I*2*M_PI*(float)i / (float)n);

    return 0;
}


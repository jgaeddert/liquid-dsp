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

//
// Test recursive FFT plan
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <getopt.h>
#include <complex.h>

#include "liquid.internal.h"

// print usage/help message
void usage()
{
    printf("fft_recursive_plan_test -- test planning FFTs\n");
    printf("options (default values in []):\n");
    printf("  h     : print help\n");
    printf("  n     : input fft size\n");
}

// print fft plan
//  _nfft   :   input fft size
//  _level  :   level
void liquid_fft_plan(unsigned int _nfft,
                     unsigned int _level);

int main(int argc, char*argv[])
{
    // options
    unsigned int nfft = 124;

    int dopt;
    while ((dopt = getopt(argc,argv,"hn:")) != EOF) {
        switch (dopt) {
        case 'h': usage();              return 0;
        case 'n': nfft = atoi(optarg);  break;
        default:
            exit(1);
        }
    }

    // validate input
    if ( nfft == 0 ) {
        fprintf(stderr,"error: input transform size must be at least 2\n");
        exit(1);
    }

    // print the FFT plan
    liquid_fft_plan(nfft, 0);

    printf("done.\n");
    return 0;
}

// print fft plan
//  _nfft   :   input fft size
//  _level  :   level
void liquid_fft_plan(unsigned int _nfft,
                     unsigned int _level)
{
    if (_nfft == 0) {
        // invalid length
        fprintf(stderr,"error: liquid_fft_estimate_method(), fft size must be > 0\n");
        exit(1);
    }

    // print appropriate spacing
    unsigned int i;
    for (i=0; i<_level; i++)
        printf("    ");

    // print fft size
    printf("%u, ", _nfft);

    if (_nfft < 8) {
        // use simple DFT codelet
        printf("codelet\n");
        return;

    } else if (fft_is_radix2(_nfft)) {
        // transform is of the form 2^m
        printf("radix-2\n");
        return;

    } else if (liquid_is_prime(_nfft)) {
        // compute prime factors of _nfft-1
        unsigned int factors[LIQUID_MAX_FACTORS];
        unsigned int num_factors=0;
        liquid_factor(_nfft-1,factors,&num_factors);
        
        if (num_factors > 2) {
            // use Rader's algorithm (type I) for size _nfft-1
            printf("Rader's algorithm, Type I > %u\n", _nfft-1);
            liquid_fft_plan(_nfft-1, _level+1);
            return;

        } else {
            // use Rader's algorithm (type II) for radix-2
            // nfft_prime = 2 ^ nextpow2(2*nfft - 4)
            unsigned int nfft_prime = 1 << liquid_nextpow2(2*_nfft-4);
            printf("Rader's algorithm, Type II > %u\n", nfft_prime);
            liquid_fft_plan(nfft_prime, _level+1);
            return;
        }
    }

    // use Cooley-Tukey algorithm
    printf("Cooley-Tukey\n");

    // compute prime factors of _nfft
    unsigned int factors[LIQUID_MAX_FACTORS];
    unsigned int num_factors=0;
    liquid_factor(_nfft,factors,&num_factors);

    for (i=0; i<num_factors; i++)
        liquid_fft_plan(factors[i], _level+1);
}


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
#define MAX_FACTORS (40)

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
                        unsigned int    _p)
{
    printf("  bfly_generic: offset=%3u, stride=%3u, m=%3u, p=%3u\n", _offset, _q, _m, _p);

    // create temporary buffer the size of the FFT
    float complex x_tmp[_p];

    unsigned int i;
    unsigned int k;

    unsigned int n;
    for (n=0; n<_m; n++) {
        printf("    u=%u\n", n);

        // copy input to temporary buffer
        for (i=0; i<_p; i++)
            x_tmp[i] = _x[n + i*_m];
        
        // compute transform...
        unsigned int f = n;
        for (i=0; i<_p; i++) {
            printf("      ----\n");
            float complex y = x_tmp[0];
            unsigned int twidx = 0;
            for (k=1; k<_p; k++) {
                //unsigned int twidx = (n + _q*_m*k) % _nfft;
                //unsigned int twidx = (_q*(n+i*_m)) % _nfft;
                twidx = (twidx + _q*f) % _nfft;
                printf("      twidx = %3u > %12.8f + j%12.8f, %12.8f + j%12.8f\n", twidx, crealf(_twiddle[twidx]), cimagf(_twiddle[twidx]), crealf(x_tmp[k]), cimagf(x_tmp[k]));

                y += x_tmp[k] * _twiddle[twidx];
            }
            f += _m;

            // store output
            printf("      y = %12.6f + j%12.6f\n", crealf(y), cimagf(y));
            _x[n + i*_m] = y;
        }
    }

    // ...
}

// FFT mixed-radix recursive function...
//  _x          :   input pointer [size: _nfft x 1]
//  _y          :   output pointer [size: _nfft x 1]
//  _twiddle    :   pre-computed twiddle factors [size: _nfft x 1]
//  _nfft       :   original FFT size
//  _xoffset    :   input buffer offset
//  _xstride    :   input buffer stride
//  _m          :   
//  _p          :   
void fftmr_cycle(float complex * _x,
                 float complex * _y,
                 float complex * _twiddle,
                 unsigned int    _nfft,
                 unsigned int    _xoffset,
                 unsigned int    _xstride,
                 unsigned int  * _m,
                 unsigned int  * _p)
{
    // de-reference factors
    unsigned int m = _m[0]; // radix
    unsigned int p = _p[0]; // FFT size

    // increment...
    _m++;
    _p++;
    
    printf("fftmr_cycle:    offset=%3u, stride=%3u, p=%3u, m=%3u\n", _xoffset, _xstride, p, m);

    unsigned int i;
    if ( m == 1 ) {
        // copy data to output buffer
        for (i=0; i<p; i++) {
            _y[i] = _x[_xoffset + _xstride*i];
            printf("    copying sample: y[%3u] = x[%3u] = %12.6f + j%12.6f\n", i, _xoffset + _xstride*i, crealf(_y[i]), cimagf(_y[i]));
        }
    } else {
        // call fftmr_cycle() recursively
        // TODO : check offset, stride
        for (i=0; i<p; i++) {
            //unsigned int offset_new = _xoffset + _xstride*i;
            fftmr_cycle(_x, _y+i*m, _twiddle, _nfft, _xoffset + _xstride*i, _xstride*p, _m, _p);
        }
    }

    // run m-point FFT
    fftmr_bfly_generic(_y, _twiddle, _nfft, _xoffset, _xstride, m, p);
}
                      

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
                n /= k;
                p[num_factors] = k;
                m[num_factors] = n;
                num_factors++;
                break;
            }
        }
    } while (n > 1 && num_factors < MAX_FACTORS);

    // NOTE: this is extremely unlikely as the worst case is
    //       nfft=2^MAX_FACTORS in which case we will probably run out
    //       of memory first
    if (num_factors == MAX_FACTORS) {
        fprintf(stderr,"error: could not factor %u with %u factors\n", nfft, MAX_FACTORS);
        exit(1);
    }

    printf("factors of %u:\n", nfft);
    for (i=0; i<num_factors; i++)
        printf("  p=%3u, m=%3u\n", p[i], m[i]);

    // create and initialize data arrays
    float complex x[nfft];
    float complex y[nfft];
    float complex y_test[nfft];
    for (i=0; i<nfft; i++) {
        //x[i] = randnf() + _Complex_I*randnf();
        x[i] = (float)i + _Complex_I*(3 - (float)i);
        y[i] = 0.0f;
    }

    // compute output for testing
    dft_run(nfft, x, y_test, DFT_FORWARD, 0);

    // compute twiddle factors (roots of unity)
    float complex twiddle[nfft];
    for (i=0; i<nfft; i++)
        twiddle[i] = cexpf(-_Complex_I*2*M_PI*(float)i / (float)nfft);

    // call mixed-radix function
    fftmr_cycle(x, y, twiddle, nfft, 0, 1, m, p);

    // 
    // print results
    //
    for (i=0; i<nfft; i++) {
        printf("  y[%3u] = %12.6f + j*%12.6f (expected %12.6f + j%12.6f)\n",
            i,
            crealf(y[i]),      cimagf(y[i]),
            crealf(y_test[i]), cimagf(y_test[i]));
    }

    // compute error
    float rmse = 0.0f;
    for (i=0; i<nfft; i++) {
        float e = y[i] - y_test[i];
        rmse += e*conjf(e);
    }
    rmse = sqrtf(rmse / (float)nfft);
    printf("RMS error : %12.4e (%s)\n", rmse, rmse < 1e-3 ? "pass" : "FAIL");

    return 0;
}

// super slow DFT, but functionally correct
void dft_run(unsigned int    _nfft,
             float complex * _x,
             float complex * _y,
             int             _dir,
             int             _flags)
{
    unsigned int i;
    unsigned int k;

    int d = (_dir == DFT_FORWARD) ? -1 : 1;

    for (i=0; i<_nfft; i++) {
        _y[i] = 0.0f;
        for (k=0; k<_nfft; k++) {
            float phi = 2*M_PI*d*i*k / (float)_nfft;
            _y[i] += _x[k] * cexpf(_Complex_I*phi);
        }
    }
}


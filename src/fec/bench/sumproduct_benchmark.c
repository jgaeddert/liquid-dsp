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
// benchmark sum-product algorithm
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#include "liquid.internal.h"

#define SUMPRODUCT_BENCH_API(M)         \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ sumproduct_bench(_start, _finish, _num_iterations, M); }

// generate half-rate LDPC generator and parity-check matrices
void sumproduct_generate(unsigned int    _m,
                         unsigned char * _G,
                         unsigned char * _H);

// Helper function to keep code base small
void sumproduct_bench(struct rusage *     _start,
                      struct rusage *     _finish,
                      unsigned long int * _num_iterations,
                      unsigned int        _m)
{
    // normalize number of iterations
    // M cycles/trial ~ 2^{ -11.5920 + 2.503*log2(_m) }
    *_num_iterations /= 4*_m*_m;
    if (*_num_iterations < 1)
        *_num_iterations = 1;

    unsigned long int i;

    // derived values
    unsigned int _n = 2*_m;

    // create arrays
    unsigned char Gs[_m*_n]; // generator matrix [m x n]
    unsigned char Hs[_m*_n]; // parity check matrix [m x n]
    sumproduct_generate(_m, Gs, Hs);

    // generate sparse binary matrices
    smatrixb G = smatrixb_create_array(Gs, _n, _m);
    smatrixb H = smatrixb_create_array(Hs, _m, _n);

    // print matrices
    //printf("G:\n"); smatrixb_print_expanded(G);
    //printf("H:\n"); smatrixb_print_expanded(H);

    unsigned char x[_m];     // original message signal
    unsigned char c[_n];     // transmitted codeword
    float LLR[_n];           // log-likelihood ratio
    unsigned char c_hat[_n]; // estimated codeword

    // initialize message array
    for (i=0; i<_m; i++)
        x[i] = rand() % 2;

    // compute encoded message
    smatrixb_vmul(G, x, c);

    // print status
    //printf("x = ["); for (i=0; i<_m; i++) printf("%2u", x[i]); printf(" ];\n");
    //printf("c = ["); for (i=0; i<_n; i++) printf("%2u", c[i]); printf(" ];\n");

    // compute log-likelihood ratio (LLR)
    for (i=0; i<_n; i++)
        LLR[i] = (c[i] == 0 ? 1.0f : -1.0f) + 0.5*randnf();
    
    // start trials
    getrusage(RUSAGE_SELF, _start);
    int parity_pass;
    for (i=0; i<(*_num_iterations); i++) {
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1);
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1);
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1);
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

//
// BENCHMARKS
//
void benchmark_sumproduct_m16   SUMPRODUCT_BENCH_API(16)
void benchmark_sumproduct_m32   SUMPRODUCT_BENCH_API(32)
void benchmark_sumproduct_m64   SUMPRODUCT_BENCH_API(64)
void benchmark_sumproduct_m128  SUMPRODUCT_BENCH_API(128)

// generate half-rate LDPC generator and parity-check matrices
void sumproduct_generate(unsigned int    _m,
                         unsigned char * _G,
                         unsigned char * _H)
{
    unsigned int i;
    unsigned int j;

    // derived values
    unsigned int _n = 2*_m;

    // initial generator polynomial [1 x m]
    unsigned char p[_m];

    // initialize generator polynomial (systematic)
    for (i=0; i<_m; i++)
        p[i] = 0;
    unsigned int t = 0;
    unsigned int k = 2;
    for (i=0; i<_m; i++) {
        t++;
        if (t == k) {
            t = 0;
            k *= 2;
            p[i] = 1;
        }
    }

    // initialze matrices
    for (i=0; i<_m; i++) {
        for (j=0; j<_m; j++) {
            // G = [I(m) P]^T
            _G[j*_m + i]         = (i==j) ? 1 : 0;
            _G[j*_m + i + _m*_m] = p[(i+j)%_m];

            // H = [P^T I(m)]
            _H[i*_n + j + _m] = (i==j) ? 1 : 0;
            _H[i*_n + j]      = p[(i+j)%_m];
        }
    }
}


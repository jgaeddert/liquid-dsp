/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// benchmark sum-product algorithm
//

#include "liquid.benchmark.h"
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

// generate half-rate LDPC generator and parity-check matrices
void sumproduct_generate(unsigned int    _m,
                         unsigned char * _G,
                         unsigned char * _H);

// Helper function to keep code base small
float sumproduct_bench(unsigned long int _num_iterations,
                      unsigned int      _m)
{
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
    unsigned char x[_m];     // original message signal
    unsigned char c[_n];     // transmitted codeword
    float LLR[_n];           // log-likelihood ratio
    unsigned char c_hat[_n]; // estimated codeword
    // initialize message array
    for (i=0; i<_m; i++)
        x[i] = rand() % 2;
    // compute encoded message
    smatrixb_vmul(G, x, c);
    // compute log-likelihood ratio (LLR)
    for (i=0; i<_n; i++)
        LLR[i] = (c[i] == 0 ? 1.0f : -1.0f) + 0.5*randnf();
    // start trials (4 sum-product decodes per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    int parity_pass;
    for (i=0; i<n; i++) {
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1); LLR[i%_m] += parity_pass ? 1 : -1;
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1); LLR[i%_m] += parity_pass ? 1 : -1;
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1); LLR[i%_m] += parity_pass ? 1 : -1;
        parity_pass = fec_sumproduct(_m, _n, H, LLR, c_hat, 1); LLR[i%_m] += parity_pass ? 1 : -1;
    }
    float extime = liquid_toc(timer);
    return extime;
}

LIQUID_BENCHMARK(sumproduct_m16,
    "fec_sumproduct, m=16",
    "fec,sumproduct")
{ return sumproduct_bench(num_iterations, 16); }

LIQUID_BENCHMARK(sumproduct_m32,
    "fec_sumproduct, m=32",
    "fec,sumproduct")
{ return sumproduct_bench(num_iterations, 32); }

LIQUID_BENCHMARK(sumproduct_m64,
    "fec_sumproduct, m=64",
    "fec,sumproduct")
{ return sumproduct_bench(num_iterations, 64); }

LIQUID_BENCHMARK(sumproduct_m128,
    "fec_sumproduct, m=128",
    "fec,sumproduct")
{ return sumproduct_bench(num_iterations, 128); }

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

    // initialize matrices
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


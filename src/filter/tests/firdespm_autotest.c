/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
// References:
//  [McClellan:1973] J. H. McClellan, T. W. Parks, L. R. Rabiner, "A
//      Computer Program for Designing Optimum FIR Linear Phase
//      Digital Filters," IEEE Transactions on Audio and
//      Electroacoustics, vol. AU-21, No. 6, December 1973.

#include "autotest/autotest.h"
#include "liquid.h"

void autotest_firdespm_bandpass_n24()
{
    // [McClellan:1973], Figure 7.

    // Initialize variables
    unsigned int n=24;
    unsigned int num_bands=2;
    float bands[4]  = {0.0f,0.08f,0.16f,0.50f};
    float des[2]    = {1.0f,0.0f};
    float weights[2]= {1.0f,1.0f};
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    float tol = 1e-4f;

    // Initialize pre-determined coefficient array
    float h0[24] = {
        0.33740917e-2f,
        0.14938299e-1f,
        0.10569360e-1f,
        0.25415067e-2f,
       -0.15929392e-1f,
       -0.34085343e-1f,
       -0.38112177e-1f,
       -0.14629169e-1f,
        0.40089541e-1f,
        0.11540713e-0f,
        0.18850752e-0f,
        0.23354606e-0f,
        // symmetry
        0.23354606e-0f,
        0.18850752e-0f,
        0.11540713e-0f,
        0.40089541e-1f,
       -0.14629169e-1f,
       -0.38112177e-1f,
       -0.34085343e-1f,
       -0.15929392e-1f,
        0.25415067e-2f,
        0.10569360e-1f,
        0.14938299e-1f,
        0.33740917e-2f
    };

    // Create filter
    float h[n];
    firdespm_run(n,num_bands,bands,des,weights,NULL,btype,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<n; i++)
        CONTEND_DELTA( h[i], h0[i], tol );
}


void autotest_firdespm_bandpass_n32()
{
    // [McClellan:1973], Figure 9.

    // Initialize variables
    unsigned int n=32;
    unsigned int num_bands = 3;
    float bands[6] = {  0.0f,   0.1f,
                        0.2f,   0.35f,
                        0.425f, 0.5f};
    float des[3] = {0.0f, 1.0f, 0.0f};
    float weights[3] = {10.0f, 1.0f, 10.0f};
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    float tol = 1e-4f;

    // Initialize pre-determined coefficient array
    float h0[32] = {
       -0.57534121e-2f,
        0.99027198e-3f,
        0.75733545e-2f,
       -0.65141192e-2f,
        0.13960525e-1f,
        0.22951469e-2f,
       -0.19994067e-1f,
        0.71369560e-2f,
       -0.39657363e-1f,
        0.11260114e-1f,
        0.66233643e-1f,
       -0.10497223e-1f,
        0.85136133e-1f,
       -0.12024993e+0f,
       -0.29678577e+0f,
        0.30410917e+0f,
        // symmetry
        0.30410917e+0f,
       -0.29678577e+0f,
       -0.12024993e+0f,
        0.85136133e-1f,
       -0.10497223e-1f,
        0.66233643e-1f,
        0.11260114e-1f,
       -0.39657363e-1f,
        0.71369560e-2f,
       -0.19994067e-1f,
        0.22951469e-2f,
        0.13960525e-1f,
       -0.65141192e-2f,
        0.75733545e-2f,
        0.99027198e-3f,
       -0.57534121e-2f
    };

    // Create filter
    float h[n];
    firdespm_run(n,num_bands,bands,des,weights,NULL,btype,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<n; i++)
        CONTEND_DELTA( h[i], h0[i], tol );
}


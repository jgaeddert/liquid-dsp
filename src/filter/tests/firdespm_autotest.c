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


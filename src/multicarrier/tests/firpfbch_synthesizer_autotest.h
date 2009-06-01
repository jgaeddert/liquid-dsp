/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __LIQUID_FIRPFBCH_SYNTHESIZER_AUTOTEST_H__
#define __LIQUID_FIRPFBCH_SYNTHESIZER_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: validate synthesis correctness
//
void autotest_firpfbch_synthesis() {
    unsigned int num_channels = 4;  // number of channels
    unsigned int num_symbols = 8;   // number of symbols per channel
    unsigned int m=2;               // filter delay
    float beta=-40.0f;              // excess bandwidth factor
    float tol=0.05f;                // error tolerance

    unsigned int i;

    float complex x[32] = {
         2.205e-01,  2.205e-01,  2.205e-01,  2.205e-01, 
         7.122e-01,  7.122e-01,  7.122e-01,  7.122e-01, 
         1.000e+00,  1.000e+00,  1.000e+00,  1.000e+00, 
         7.122e-01,  7.122e-01,  7.122e-01,  7.122e-01, 
         2.205e-01,  2.205e-01,  2.205e-01,  2.205e-01, 
         0.000e+00,  0.000e+00,  0.000e+00,  0.000e+00, 
         0.000e+00,  0.000e+00,  0.000e+00,  0.000e+00, 
         0.000e+00,  0.000e+00,  0.000e+00,  0.000e+00
    };

    float complex y_test[32] = {
        -1.726e-18+-0.000e+00*_Complex_I,  4.174e-19+-4.337e-19*_Complex_I, 
         0.000e+00+-1.297e-18*_Complex_I, -2.355e-18+-1.735e-18*_Complex_I, 
         6.080e-19+-2.234e-34*_Complex_I, -1.567e-17+ 4.337e-18*_Complex_I, 
         0.000e+00+ 1.124e-17*_Complex_I, -2.847e-17+ 2.429e-17*_Complex_I, 
         2.205e-01+-1.620e-16*_Complex_I, -2.428e-16+ 9.714e-17*_Complex_I, 
         0.000e+00+ 5.402e-16*_Complex_I, -3.255e-16+ 6.939e-16*_Complex_I, 
         7.122e-01+-7.849e-16*_Complex_I, -1.055e-15+-4.163e-16*_Complex_I, 
         0.000e+00+-4.215e-16*_Complex_I, -4.271e-16+ 1.304e-15*_Complex_I, 
         1.000e+00+-1.469e-15*_Complex_I, -1.362e-15+-3.608e-16*_Complex_I, 
         0.000e+00+ 1.320e-15*_Complex_I,  1.211e-15+ 1.221e-15*_Complex_I, 
         7.122e-01+-2.573e-15*_Complex_I,  1.293e-16+-1.388e-16*_Complex_I, 
         0.000e+00+-4.988e-16*_Complex_I, -6.063e-17+ 5.274e-16*_Complex_I, 
         2.205e-01+-4.859e-16*_Complex_I, -1.964e-16+-1.388e-17*_Complex_I, 
         0.000e+00+ 1.574e-16*_Complex_I,  4.390e-17+ 4.423e-17*_Complex_I, 
         6.080e-19+-4.835e-34*_Complex_I,  8.310e-17+ 0.000e+00*_Complex_I, 
         0.000e+00+ 1.818e-17*_Complex_I, -3.295e-17+-1.691e-17*_Complex_I
    };

    firpfbch c = firpfbch_create(num_channels,
                                 m,
                                 beta,
                                 0.0f,
                                 FIRPFBCH_NYQUIST,
                                 FIRPFBCH_SYNTHESIZER);

    float complex y[32];

    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        firpfbch_execute(c, &x[n], &y[n]);
        n += num_channels;
    }

#if 0
    // print formatted results (octave)
    printf("y=zeros(1,32);\n");
    printf("y_test=zeros(1,32);\n");
    for (i=0; i<32; i++) {
        printf("     y(%2u) = %10.2e + j*%10.2e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        printf("y_test(%2u) = %10.2e + j*%10.2e;\n", i+1, crealf(y_test[i]), cimagf(y_test[i]));
    }
    // plot results
    printf("figure;\n");
    printf("subplot(2,1,1); plot(1:32,real(y_test), 1:32,real(y));\n");
    printf("subplot(2,1,2); plot(1:32,imag(y_test), 1:32,imag(y));\n");
#endif


    for (i=0; i<32; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(y_test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(y_test[i]), tol);
    }

    firpfbch_destroy(c);
}


//
// AUTOTEST: validate synthesis correctness
//
void autotest_firpfbch_synthesis_noise() {
    unsigned int num_channels = 4;  // number of channels
    unsigned int num_symbols = 8;   // number of symbols per channel
    unsigned int m=2;               // filter delay
    float beta=-40.0f;              // excess bandwidth factor
    float tol=0.05f;                // error tolerance

    unsigned int i;

    float complex x[32] = {
         2.101e-01+-3.436e-02*_Complex_I, -2.225e-02+ 6.707e-02*_Complex_I, 
        -7.929e-02+-1.404e-01*_Complex_I, -1.573e-01+ 4.003e-02*_Complex_I, 
         4.426e-02+-1.927e-02*_Complex_I, -7.451e-02+-6.044e-02*_Complex_I, 
         3.396e-02+ 1.280e-01*_Complex_I,  1.146e-01+-2.094e-01*_Complex_I, 
        -5.009e-02+ 8.634e-02*_Complex_I,  3.517e-02+ 1.141e-01*_Complex_I, 
         1.256e-01+-1.558e-02*_Complex_I,  2.088e-02+-1.004e-01*_Complex_I, 
        -7.309e-02+-5.451e-02*_Complex_I,  6.383e-02+-7.361e-02*_Complex_I, 
         2.481e-02+-1.636e-01*_Complex_I, -6.789e-02+ 1.272e-02*_Complex_I, 
         2.313e-01+ 7.870e-02*_Complex_I,  1.394e-01+-3.172e-02*_Complex_I, 
         7.554e-02+ 5.138e-02*_Complex_I,  1.511e-02+-7.650e-02*_Complex_I, 
         1.788e-01+-6.750e-02*_Complex_I, -9.681e-02+ 4.787e-02*_Complex_I, 
         1.063e-03+-1.350e-01*_Complex_I, -1.337e-01+-6.143e-02*_Complex_I, 
        -3.548e-02+-4.487e-02*_Complex_I, -5.897e-03+-1.817e-01*_Complex_I, 
        -1.524e-02+-8.241e-02*_Complex_I, -1.661e-02+-1.732e-01*_Complex_I, 
        -4.875e-02+-4.123e-02*_Complex_I, -1.245e-01+ 1.724e-01*_Complex_I, 
        -1.316e-01+ 8.264e-02*_Complex_I, -1.496e-01+-6.837e-02*_Complex_I
        };

    float complex y_test[32] = {
         9.551e-20+ 1.325e-19*_Complex_I, -2.704e-03+-2.485e-03*_Complex_I, 
        -7.459e-03+ 6.773e-03*_Complex_I, -8.394e-03+ 7.708e-04*_Complex_I, 
        -5.739e-19+-1.591e-19*_Complex_I,  1.785e-02+ 1.856e-02*_Complex_I, 
         4.472e-02+-5.053e-02*_Complex_I,  6.561e-02+-7.523e-03*_Complex_I, 
        -1.220e-02+-1.691e-02*_Complex_I,  5.323e-02+ 3.096e-02*_Complex_I, 
         5.077e-02+ 1.285e-02*_Complex_I,  5.394e-02+ 5.096e-03*_Complex_I, 
         2.958e-02+-4.029e-02*_Complex_I, -6.188e-02+-7.584e-02*_Complex_I, 
         2.075e-03+ 7.459e-02*_Complex_I,  2.016e-02+ 2.286e-02*_Complex_I, 
         3.290e-02+ 2.112e-02*_Complex_I, -8.431e-02+ 4.808e-02*_Complex_I, 
        -8.209e-03+-2.955e-02*_Complex_I, -4.519e-02+ 2.624e-03*_Complex_I, 
        -1.309e-02+-6.974e-02*_Complex_I,  1.404e-02+ 5.847e-02*_Complex_I, 
         5.564e-03+ 1.510e-02*_Complex_I,  2.472e-02+-2.455e-02*_Complex_I, 
         1.154e-01+ 5.465e-03*_Complex_I,  2.921e-02+ 3.312e-02*_Complex_I, 
         8.446e-02+ 5.556e-03*_Complex_I,  7.856e-02+ 2.083e-04*_Complex_I, 
        -1.267e-02+-5.402e-02*_Complex_I,  1.304e-02+ 2.306e-02*_Complex_I, 
         5.027e-02+ 1.462e-03*_Complex_I,  9.807e-04+ 1.279e-02*_Complex_I
        };

    firpfbch c = firpfbch_create(num_channels,
                                 m,
                                 beta,
                                 0.0f,
                                 FIRPFBCH_NYQUIST,
                                 FIRPFBCH_SYNTHESIZER);

    float complex y[32];

    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        firpfbch_execute(c, &x[n], &y[n]);
        n += num_channels;
    }

    for (i=0; i<32; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(y_test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(y_test[i]), tol);
    }

    firpfbch_destroy(c);
}

#endif // __LIQUID_FIRPFBCH_SYNTHESIZER_AUTOTEST_H__


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

#endif // __LIQUID_FIRPFBCH_SYNTHESIZER_AUTOTEST_H__


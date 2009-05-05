#ifndef __LIQUID_FIRPFBCH_AUTOTEST_H__
#define __LIQUID_FIRPFBCH_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: test sub-band energy
//
void xautotest_firpfbch_analysis_x()
{
    unsigned int num_channels=8;
    unsigned int m=2;
    float slsl=-60.0f;
    float tol=0.05f;

    float f;
    nco nco_synth = nco_create();

    firpfbch c = firpfbch_create(num_channels, m, slsl, FIRPFBCH_NYQUIST,FIRPFBCH_ANALYZER);

    unsigned int i, j, k;
    float complex x[num_channels], y[num_channels];

    for (i=0; i<num_channels; i++) {
        // channel center frequency
        f = 2*M_PI * i / (float)num_channels;
        nco_set_frequency(nco_synth, f);

        for (j=0; j<20; j++) {
            // generate frame of data
            for (k=0; k<num_channels; k++) {
                nco_cexpf(nco_synth, &x[k]);
                nco_step(nco_synth);
            }

            // execute analysis filter bank
            firpfbch_execute(c, x, y);

        }

        if (_autotest_verbose) {
            printf("e[%2u] : ", i);
            for (j=0; j<num_channels; j++)
                printf("%6.4f ",cabsf(y[j])/(float)num_channels);
            printf("\n");
        }

        for (j=0; j<num_channels; j++) {
            float e = cabsf(y[j]) / (float)num_channels;
            if (i==j)   {   CONTEND_DELTA( e, 1.0f, tol );  }
            else        {   CONTEND_DELTA( e, 0.0f, tol );  }
        }
    }

    firpfbch_destroy(c);
    nco_destroy(nco_synth);
}

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


//
// AUTOTEST: validate analysis correctness
//
void autotest_firpfbch_analysis() {
    unsigned int num_channels = 4;  // number of channels
    unsigned int m=2;               // filter delay
    float beta=-40.0f;              // excess bandwidth factor
    unsigned int num_symbols = 8;   // number of symbols per channel
    float tol=0.05f;                // error tolerance

    unsigned int i;

    float complex x[32] = {
         3.198e-02+ 0.000e+00*_Complex_I, -8.915e-19+ 1.735e-18*_Complex_I, 
         0.000e+00+ 5.437e-18*_Complex_I,  1.025e-17+ 1.041e-17*_Complex_I, 
         1.620e-01+-5.952e-17*_Complex_I, -1.187e-16+ 2.776e-17*_Complex_I, 
         0.000e+00+ 4.134e-17*_Complex_I, -5.762e-17+ 5.551e-17*_Complex_I, 
         2.500e-01+-1.837e-16*_Complex_I, -1.741e-16+ 6.939e-17*_Complex_I, 
         0.000e+00+ 2.688e-16*_Complex_I, -1.060e-16+ 2.429e-16*_Complex_I, 
         1.620e-01+-1.786e-16*_Complex_I, -1.611e-16+-6.245e-17*_Complex_I, 
         0.000e+00+-4.081e-17*_Complex_I, -2.417e-17+ 7.633e-17*_Complex_I, 
         3.198e-02+-4.699e-17*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+-0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+-0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I
    };

    float complex y_test[32] = {
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
        -0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+-0.000e+00*_Complex_I, 
         5.691e-19+-1.136e-18*_Complex_I,  1.142e-18+ 5.361e-21*_Complex_I, 
         1.225e-18+ 9.058e-20*_Complex_I,  6.521e-19+ 1.040e-18*_Complex_I, 
         3.198e-02+-1.872e-18*_Complex_I,  3.198e-02+-8.044e-18*_Complex_I, 
         3.198e-02+-3.595e-18*_Complex_I,  3.198e-02+ 1.764e-18*_Complex_I, 
         1.620e-01+-2.720e-17*_Complex_I,  1.620e-01+ 1.437e-17*_Complex_I, 
         1.620e-01+-1.083e-16*_Complex_I,  1.620e-01+-1.764e-16*_Complex_I, 
         2.500e-01+ 1.824e-16*_Complex_I,  2.500e-01+-2.960e-16*_Complex_I, 
         2.500e-01+-2.089e-16*_Complex_I,  2.500e-01+-5.042e-16*_Complex_I, 
         1.620e-01+ 1.402e-16*_Complex_I,  1.620e-01+-2.311e-16*_Complex_I, 
         1.620e-01+-2.570e-16*_Complex_I,  1.620e-01+-4.259e-16*_Complex_I, 
         3.198e-02+-7.371e-17*_Complex_I,  3.198e-02+ 2.396e-17*_Complex_I, 
         3.198e-02+-1.239e-16*_Complex_I,  3.198e-02+-2.610e-17*_Complex_I, 
         8.836e-18+-1.602e-18*_Complex_I,  1.030e-17+-8.002e-18*_Complex_I, 
        -9.580e-18+ 9.447e-18*_Complex_I, -1.105e-17+ 1.562e-19*_Complex_I
    };

    // create channelizer
    firpfbch c = firpfbch_create(num_channels,
                                 m,
                                 beta,
                                 FIRPFBCH_NYQUIST,
                                 FIRPFBCH_ANALYZER);

    float complex y[32];

    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        firpfbch_execute(c, &x[n], &y[n]);
        n += num_channels;
    }

#if 0
    // print formatted results (octave)
    printf("y=zeros(%u,%u);\n", num_channels, num_symbols);
    printf("y_test=zeros(%u,%u);\n", num_channels, num_symbols);
    unsigned int j;
    n=0;
    for (j=0; j<num_symbols; j++) {
        for (i=0; i<num_channels; i++) {
            printf("     y(%2u,%2u) = %10.3e + j*%10.3e;\n", i+1, j+1, crealf(y[n]),      cimagf(y[n]));
            printf("y_test(%2u,%2u) = %10.3e + j*%10.3e;\n", i+1, j+1, crealf(y_test[n]), cimagf(y_test[n]));
            n++;
        }
    }
    // plot results
    printf("for i=1:4,\n");
    printf("    figure;\n");
    printf("    subplot(2,1,1); plot(1:8,real(y(i,:)),1:8,real(y_test(i,:)));\n");
    printf("    subplot(2,1,2); plot(1:8,imag(y(i,:)),1:8,imag(y_test(i,:)));\n");
    printf("end;\n");
#endif

    for (i=0; i<32; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(y_test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(y_test[i]), tol);
    }

    firpfbch_destroy(c);
}


#endif // __LIQUID_FIRPFBCH_AUTOTEST_H__


#ifndef __LIQUID_OFDMOQAM_AUTOTEST_H__
#define __LIQUID_OFDMOQAM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

#define OFDMOQAM_FILENAME "ofdmoqam.m"

// 
// AUTOTEST: test sub-band energy
//
void autotest_ofdmoqam_synthesis()
{
    unsigned int num_channels=4;
    unsigned int num_symbols=16;
    unsigned int m=2;
    float tol=0.05f;    // high tolerance due to prototyping filter mismatch

    unsigned int i, j;
    float complex x[64] = {
      1.00000000+  1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      1.00000000+  1.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I
    };

    float complex y_test[64] = {
     -0.02054200+  0.00000000*_Complex_I,   0.00126040+  0.00000000*_Complex_I, 
      0.03643500+ -0.02054200*_Complex_I,  -0.00253100+  0.00126040*_Complex_I, 
     -0.06595200+  0.03643500*_Complex_I,   0.00125830+ -0.00253100*_Complex_I, 
      0.39171000+ -0.10704000*_Complex_I,   0.99750000+  0.00377910*_Complex_I, 
      1.33650000+  0.46458000*_Complex_I,   0.99371000+  0.99244000*_Complex_I, 
      0.03643500+  1.20460000*_Complex_I,  -0.99498000+  0.99623000*_Complex_I, 
     -1.42300000+  0.81985000*_Complex_I,  -0.99624000+  1.00000000*_Complex_I, 
     -0.00000000+  1.25000000*_Complex_I,   0.99624000+  0.99118000*_Complex_I, 
      1.42300000+  0.07286900*_Complex_I,   0.99624000+ -0.99372000*_Complex_I, 
      0.00000000+ -1.42300000*_Complex_I,  -0.99624000+ -0.99624000*_Complex_I, 
     -1.46400000+ -0.00000000*_Complex_I,  -0.99372000+  0.99624000*_Complex_I, 
      0.07286900+  1.46400000*_Complex_I,   0.99118000+  0.99372000*_Complex_I, 
      1.29100000+ -0.07286900*_Complex_I,   0.99876000+ -0.99118000*_Complex_I, 
      0.78342000+ -1.29100000*_Complex_I,   0.99876000+ -0.99876000*_Complex_I, 
      1.25000000+ -0.78342000*_Complex_I,   0.99118000+ -0.99876000*_Complex_I, 
      0.07286900+ -1.29100000*_Complex_I,  -0.99372000+ -0.98866000*_Complex_I, 
     -1.44350000+  0.00000000*_Complex_I,  -0.99498000+  0.98866000*_Complex_I, 
      0.03643500+  1.27050000*_Complex_I,   0.99371000+  1.00000000*_Complex_I, 
      1.35700000+  0.81985000*_Complex_I,   0.99750000+  0.99623000*_Complex_I, 
      0.39171000+  1.18400000*_Complex_I,   0.00125830+  0.99244000*_Complex_I, 
     -0.08649500+  0.46458000*_Complex_I,  -0.00253100+  0.00377910*_Complex_I, 
      0.03643500+ -0.08649500*_Complex_I,   0.00126040+ -0.00253100*_Complex_I, 
      0.00000000+  0.03643500*_Complex_I,   0.00000000+  0.00126040*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I, 
      0.00000000+  0.00000000*_Complex_I,   0.00000000+  0.00000000*_Complex_I
    };

    // derived values

    // 
    ofdmoqam c = ofdmoqam_create(num_channels, m, OFDMOQAM_SYNTHESIZER);

    float complex y[64];
    for (i=0; i<64; i++)
        y[i] = 333.3f;

    // compute output
    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        ofdmoqam_execute(c, &x[n], &y[n]);
        n += num_channels;
    }

    // compare output
    for (i=0; i<64; i++) {
        CONTEND_DELTA( crealf(y[i]), crealf(y_test[i]), tol );
        CONTEND_DELTA( cimagf(y[i]), cimagf(y_test[i]), tol );

        float e = cabsf(y[i] - y_test[i]);
        printf("%3u : %8.4f + j%8.4f       ::       %8.4f + j%8.4f (e = %8.4f)\n",
            i,
            crealf(y[i]), cimagf(y[i]),
            crealf(y_test[i]), cimagf(y_test[i]),
            e);
    }

    // destroy objects
    ofdmoqam_destroy(c);
}

#endif // __LIQUID_OFDMOQAM_AUTOTEST_H__


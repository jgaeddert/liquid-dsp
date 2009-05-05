#ifndef __LIQUID_FIRPFBCH_AUTOTEST_H__
#define __LIQUID_FIRPFBCH_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: test sub-band energy
//
void autotest_firpfbch_analysis_x()
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
    float beta=0.99f;               // excess bandwidth factor
    float tol=0.05f;                // error tolerance

    unsigned int i;

    float complex sym[32] = {
      1.00000000+  1.00000000*_Complex_I,  -1.00000000+  1.00000000*_Complex_I, 
     -1.00000000+ -1.00000000*_Complex_I,   1.00000000+  1.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,  -1.00000000+ -1.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,   1.00000000+ -1.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,   1.00000000+ -1.00000000*_Complex_I, 
      1.00000000+  1.00000000*_Complex_I,   1.00000000+ -1.00000000*_Complex_I, 
     -1.00000000+ -1.00000000*_Complex_I,   1.00000000+  1.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,  -1.00000000+  1.00000000*_Complex_I, 
      1.00000000+  1.00000000*_Complex_I,   1.00000000+  1.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,   1.00000000+ -1.00000000*_Complex_I, 
     -1.00000000+ -1.00000000*_Complex_I,  -1.00000000+ -1.00000000*_Complex_I, 
      1.00000000+  1.00000000*_Complex_I,  -1.00000000+ -1.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,  -1.00000000+ -1.00000000*_Complex_I, 
      1.00000000+ -1.00000000*_Complex_I,   1.00000000+  1.00000000*_Complex_I, 
     -1.00000000+  1.00000000*_Complex_I,  -1.00000000+  1.00000000*_Complex_I, 
     -1.00000000+ -1.00000000*_Complex_I,  -1.00000000+  1.00000000*_Complex_I

    };

    float complex y_test[32] = {
      0.00000000+ -0.04108500*_Complex_I,   0.00252080+ -0.00000000*_Complex_I, 
     -0.00000000+ -0.07286900*_Complex_I,  -0.00506200+ -0.01012400*_Complex_I, 
      0.00000000+ -0.13190000*_Complex_I,   0.00251650+ -0.00000000*_Complex_I, 
     -0.00000000+ -0.78342000*_Complex_I,   2.00500000+  3.98980000*_Complex_I, 
     -0.08217000+  2.75510000*_Complex_I,   1.99490000+ -0.00252080*_Complex_I, 
     -0.00000000+  0.07286900*_Complex_I,  -1.99490000+  4.01510000*_Complex_I, 
     -0.30489000+ -2.58210000*_Complex_I,  -2.00500000+ -0.00503730*_Complex_I, 
     -0.07286900+  1.56680000*_Complex_I,  -0.00251650+ -1.97470000*_Complex_I, 
      5.21390000+ -2.58210000*_Complex_I,   0.00506200+ -2.00000000*_Complex_I, 
     -0.92916000+  0.14574000*_Complex_I,  -0.01264500+ -4.00000000*_Complex_I, 
     -3.01890000+  2.58210000*_Complex_I,  -0.00252080+  0.00254120*_Complex_I, 
     -1.63970000+  0.14574000*_Complex_I,   4.00500000+ -0.00753340*_Complex_I, 
      2.84590000+  2.58210000*_Complex_I,  -0.00251650+ -0.00755810*_Complex_I, 
      0.00000000+  1.56680000*_Complex_I,  -1.98480000+ -2.01010000*_Complex_I, 
     -2.80480000+ -2.58210000*_Complex_I,  -1.99490000+ -2.00250000*_Complex_I, 
      1.63970000+ -0.00000000*_Complex_I,  -2.00000000+  1.98990000*_Complex_I
    };

    firpfbch c = firpfbch_create(num_channels,
                                 m,
                                 beta,
                                 FIRPFBCH_ROOTNYQUIST,
                                 FIRPFBCH_SYNTHESIZER);

    float complex y[32];
    for (i=0; i<32; i++)
        y[i] = 0.0f;

    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        firpfbch_execute(c, &sym[n], &y[n]);
        n += num_channels;
    }

    for (i=0; i<32; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(y_test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(y_test[i]), tol);

        //float e = cabsf(y[i] - y_test[i]);
        //printf("  %3u : %8.4f\n", i, e);
    }

    firpfbch_destroy(c);
}


#endif // __LIQUID_FIRPFBCH_AUTOTEST_H__


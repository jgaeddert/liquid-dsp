#ifndef __FIR_FILTER_CRCF_AUTOTEST_H__
#define __FIR_FILTER_CRCF_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

void autotest_fir_filter_crcf_noise_01()
{
    // Initialize variables
    float h[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    // noise signal
    float complex x[32] = {
     -8.5985e-01+  1.9859e+00*_Complex_I,  -1.0981e+00+  7.3435e-01*_Complex_I, 
      1.9738e-02+  1.6488e-01*_Complex_I,   4.0861e-01+  9.8558e-01*_Complex_I, 
      8.8505e-01+  3.8529e-01*_Complex_I,  -7.3709e-01+ -1.2608e+00*_Complex_I, 
     -7.6302e-01+  1.4282e-01*_Complex_I,  -1.5708e+00+ -1.0774e+00*_Complex_I, 
     -4.7569e-02+ -1.0912e+00*_Complex_I,   1.3833e+00+  3.0085e-01*_Complex_I, 
      1.2652e+00+  1.7522e+00*_Complex_I,  -1.9119e-01+ -6.9241e-02*_Complex_I, 
      1.5155e+00+  1.2681e+00*_Complex_I,   1.1473e+00+ -1.1756e-02*_Complex_I, 
      1.0033e+00+ -1.2385e+00*_Complex_I,  -9.7532e-01+  4.4694e-02*_Complex_I, 
      1.4272e+00+  1.3811e+00*_Complex_I,  -5.2102e-02+ -1.4823e+00*_Complex_I, 
      6.5172e-01+  4.2771e-01*_Complex_I,  -2.9732e-01+  3.8132e-01*_Complex_I, 
      1.4363e-01+ -2.3562e-01*_Complex_I,  -4.1195e-01+  7.1421e-01*_Complex_I, 
     -1.5922e+00+  4.1696e-01*_Complex_I,   6.3603e-01+  1.4861e+00*_Complex_I, 
      1.1724e+00+ -1.8459e+00*_Complex_I,   1.7073e+00+ -1.6641e-01*_Complex_I, 
      2.8442e-01+ -6.1188e-01*_Complex_I,  -8.4867e-02+  1.0387e+00*_Complex_I, 
     -5.5189e-02+ -3.7222e-01*_Complex_I,   1.1913e+00+ -4.8040e-01*_Complex_I, 
     -1.0573e+00+ -2.1809e-01*_Complex_I,  -1.8163e-01+ -8.4464e-01*_Complex_I
    };
    
    // test output
    float complex test[32] = {
     -8.5985e-02+  1.9859e-01*_Complex_I,  -2.8178e-01+  4.7062e-01*_Complex_I, 
     -4.7561e-01+  7.5914e-01*_Complex_I,  -6.2857e-01+  1.1462e+00*_Complex_I, 
     -6.9303e-01+  1.5718e+00*_Complex_I,  -8.3119e-01+  1.8713e+00*_Complex_I, 
     -1.0457e+00+  2.1851e+00*_Complex_I,  -1.4172e+00+  2.3912e+00*_Complex_I, 
     -1.7935e+00+  2.4882e+00*_Complex_I,  -2.0315e+00+  2.6152e+00*_Complex_I, 
     -1.1971e+00+  7.3292e-01*_Complex_I,  -3.3769e-02+  2.1875e-02*_Complex_I, 
      1.6129e-01+ -9.3836e-03*_Complex_I,   4.1346e-02+ -9.6107e-01*_Complex_I, 
     -5.4322e-01+ -1.4748e+00*_Complex_I,   4.7054e-01+ -2.1203e-01*_Complex_I, 
      1.7292e+00+ -2.2896e-01*_Complex_I,   3.9476e+00+  9.3379e-01*_Complex_I, 
      4.7126e+00+  2.2623e+00*_Complex_I,   3.8787e+00+  2.2068e+00*_Complex_I, 
      3.0508e+00+  5.0119e-01*_Complex_I,   3.6572e+00+  6.9532e-01*_Complex_I, 
      2.2462e+00+ -5.3304e-01*_Complex_I,   1.1521e+00+ -3.3171e-01*_Complex_I, 
      2.1908e-01+  1.0356e+00*_Complex_I,   1.5329e+00+  1.0986e+00*_Complex_I, 
      3.2989e-01+ -3.7405e-01*_Complex_I,   6.0291e-01+  1.2688e+00*_Complex_I, 
      1.0141e-01+  9.2163e-01*_Complex_I,   6.9782e-01+  5.3468e-01*_Complex_I, 
      7.3318e-01+  7.6641e-01*_Complex_I,   1.3472e+00+ -1.0757e-01*_Complex_I
    };

    float complex y;    // ouput


    // Load filter coefficients externally
    fir_filter_crcf f = fir_filter_crcf_create(h, 10);

    unsigned int i;
    // 
    for (i=0; i<32; i++) {
        fir_filter_crcf_push(f, x[i]);
        fir_filter_crcf_execute(f, &y);
        CONTEND_DELTA( crealf(test[i]), crealf(y), 0.001 );
        CONTEND_DELTA( cimagf(test[i]), cimagf(y), 0.001 );
    }

    fir_filter_crcf_destroy(f);
}


#endif // __FIR_FILTER_CRCF_AUTOTEST_H__


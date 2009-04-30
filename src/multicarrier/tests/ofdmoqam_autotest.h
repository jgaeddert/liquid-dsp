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
    float tol=0.05f;

    unsigned int i, j;
    unsigned int msg[64] = {
       0,   0,   3,   1,
       1,   2,   1,   1,
       3,   1,   0,   0,
       1,   3,   2,   2,
       3,   2,   0,   3,
       3,   1,   1,   0,
       1,   0,   3,   1,
       0,   0,   3,   2,

       0,   0,   0,   0,
       0,   0,   0,   0,
       0,   0,   0,   0,
       0,   0,   0,   0,
       0,   0,   0,   0,
       0,   0,   0,   0,
       0,   0,   0,   0,
       0,   0,   0,   0
    };

    float complex sym[64];
    for (i=0; i<64; i++)
        sym[i] = cexpf(_Complex_I*M_PI*((float)msg[i] + 0.5f));

    float complex y_test[64] = {
     -0.0073+ -0.0073*_Complex_I,   0.0000+ -0.0000*_Complex_I, 
      0.0129+ -0.0129*_Complex_I,  -0.0000+ -0.0000*_Complex_I, 
     -0.0233+ -0.0306*_Complex_I,   0.0004+ -0.0018*_Complex_I, 
      0.1312+ -0.1586*_Complex_I,   0.0009+ -0.0000*_Complex_I, 
      0.4596+  0.4548*_Complex_I,   0.0009+  0.7071*_Complex_I, 
     -0.0177+ -0.1948*_Complex_I,  -0.3535+ -0.0000*_Complex_I, 
     -0.6545+  0.0975*_Complex_I,   0.3535+  0.0036*_Complex_I, 
      0.4492+  0.2978*_Complex_I,  -0.0009+  0.0000*_Complex_I, 
      0.3517+  0.6384*_Complex_I,  -0.0009+ -0.7080*_Complex_I, 
     -0.0306+ -0.0177*_Complex_I,  -0.0004+ -0.0000*_Complex_I, 
     -0.5232+ -0.4436*_Complex_I,   0.0004+  0.7071*_Complex_I, 
      0.0129+  0.2770*_Complex_I,  -0.0009+  0.0000*_Complex_I, 
      0.4798+ -0.4565*_Complex_I,   0.0004+ -0.7044*_Complex_I, 
      0.1385+  0.0000*_Complex_I,   0.3544+ -0.0000*_Complex_I, 
     -0.0378+  0.4492*_Complex_I,   0.3522+ -0.7098*_Complex_I, 
      0.0258+ -0.2899*_Complex_I,  -0.3535+  0.0000*_Complex_I, 
     -0.0306+  0.4186*_Complex_I,  -0.3553+  0.7026*_Complex_I, 
      0.1514+ -0.1643*_Complex_I,   0.3531+  0.0000*_Complex_I, 
      0.4492+ -0.0306*_Complex_I,  -0.3527+  0.7080*_Complex_I, 
      0.1514+ -0.0129*_Complex_I,   0.0004+ -0.0000*_Complex_I, 
     -0.0306+  0.0000*_Complex_I,   0.0009+  0.0027*_Complex_I, 
      0.0129+ -0.0000*_Complex_I,   0.0004+ -0.0000*_Complex_I, 
     -0.0000+ -0.0000*_Complex_I,   0.0000+  0.0009*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I
    };

    // derived values

    // 
    ofdmoqam c = ofdmoqam_create(num_channels, m, OFDMOQAM_SYNTHESIZER);

    FILE*fid = fopen(OFDMOQAM_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OFDMOQAM_FILENAME);

    float complex y[64];
    for (i=0; i<64; i++)
        y[i] = 0.0f;

    // compute output
    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        ofdmoqam_execute(c, &sym[n], &y[n]);
        n += num_channels;
    }

    fprintf(fid,"y = zeros(1,%u);\n", 64);
    for (i=0; i<64; i++)
        fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // compare output
    for (i=0; i<64; i++) {
        //CONTEND_DELTA( crealf(y[i]), crealf(y_test[i]), tol );
        //CONTEND_DELTA( cimagf(y[i]), cimagf(y_test[i]), tol );

        float e = cabsf(y[i] - y_test[i]);
        printf("%3u : %8.4f + j%8.4f       ::       %8.4f + j%8.4f (e = %8.4f)\n",
            i,
            crealf(y[i]), cimagf(y[i]),
            crealf(y_test[i]), cimagf(y_test[i]),
            e);
    }

    fclose(fid);

    // destroy objects
    ofdmoqam_destroy(c);
}

#endif // __LIQUID_OFDMOQAM_AUTOTEST_H__


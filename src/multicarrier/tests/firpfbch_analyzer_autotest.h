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

#ifndef __LIQUID_FIRPFBCH_ANALYZER_AUTOTEST_H__
#define __LIQUID_FIRPFBCH_ANALYZER_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: validate analysis correctness
//
void autotest_firpfbch_analysis() {
    // options
    unsigned int num_channels=4;    // number of channels
    unsigned int m=3;               // filter delay
    float slsl=-60;                 // sidelobe suppression level
    unsigned int num_symbols=16;    // number of baseband symbols
    float tol=1e-3f;                // error tolerance

    unsigned int num_frames = num_symbols+4*m;
    unsigned int i, j;

    // create channelizer object (analyzer)
    firpfbch ca = firpfbch_create(num_channels,
                                  m,
                                  slsl,
                                  0,
                                  FIRPFBCH_NYQUIST,
                                  FIRPFBCH_ANALYZER);

    // generate data buffers
    float complex  y[num_channels]; // input
    float complex z0[num_channels]; // conventional output
    float complex z1[num_channels]; // filterbank channelizer output

    // retrieve filter taps from channelizer object
    unsigned int h_len = 2*m*num_channels;
    float h[h_len];
    firpfbch_get_filter_taps(ca,h);

    // objects to run conventional channelizer
    decim_crcf decim[num_channels];
    nco ncox[num_channels];
    for (i=0; i<num_channels; i++) {
        decim[i] = decim_crcf_create(num_channels, h, h_len);
        ncox[i] = nco_create(LIQUID_VCO);
        nco_set_frequency(ncox[i], -2.0f*M_PI*(float)(i)/(float)(num_channels));
    }

    // analyze time series
    float complex z0a[num_channels];
    for (i=0; i<num_frames; i++) {
        // generate random samples
        for (j=0; j<num_channels; j++)
            y[j] = randnf() + randnf()*_Complex_I;

        // 
        // execute conventional analyzer
        //
        for (j=0; j<num_channels; j++)
            z0[j] = 0.0f;

        for (j=0; j<num_channels; j++) {
            // down-convert
            nco_mix_block_down(ncox[j],y,z0a,num_channels);

            // run decimator
            decim_crcf_execute(decim[j],z0a,&z0[j],0);
        }

        // 
        // execute analysis filter bank
        //
        firpfbch_execute(ca, y, z1);

        // 
        // validate outputs
        //
        for (j=0; j<num_channels; j++) {
            CONTEND_DELTA(crealf(z0[j]), crealf(z1[j]), tol);
            CONTEND_DELTA(cimagf(z0[j]), cimagf(z1[j]), tol);
        }
    }

    // clean up allocated objects
    for (i=0; i<num_channels; i++) {
        decim_crcf_destroy(decim[i]);
        nco_destroy(ncox[i]);
    }
    firpfbch_destroy(ca);

    printf("done.\n");
}


//
// AUTOTEST: validate analysis correctness
//
void xautotest_firpfbch_analysis_noise() {
    unsigned int num_channels = 4;  // number of channels
    unsigned int m=2;               // filter delay
    float beta=-40.0f;              // excess bandwidth factor
    unsigned int num_symbols = 8;   // number of symbols per channel
    float tol=0.05f;                // error tolerance

    unsigned int i;

    float complex x[32] = {
        -5.937e-02+ 8.925e-02*_Complex_I, -1.288e-01+ 2.123e-02*_Complex_I, 
        -1.655e-02+ 1.321e-02*_Complex_I, -1.451e-01+ 3.243e-02*_Complex_I, 
         1.801e-01+-1.064e-02*_Complex_I,  3.832e-02+ 1.462e-01*_Complex_I, 
         3.041e-02+-1.010e-01*_Complex_I, -1.765e-01+ 6.912e-02*_Complex_I, 
         2.375e-01+ 4.779e-02*_Complex_I, -3.149e-02+-5.068e-02*_Complex_I, 
         1.492e-01+-5.717e-02*_Complex_I,  1.615e-03+-3.996e-02*_Complex_I, 
         4.525e-02+ 2.823e-01*_Complex_I, -3.987e-02+ 3.753e-03*_Complex_I, 
         7.667e-02+-1.671e-01*_Complex_I,  2.917e-02+-4.288e-02*_Complex_I, 
         1.128e-02+ 1.703e-01*_Complex_I, -5.305e-02+ 1.134e-01*_Complex_I, 
        -2.110e-01+ 1.215e-01*_Complex_I,  8.129e-02+ 5.713e-03*_Complex_I, 
        -4.876e-02+ 8.990e-04*_Complex_I, -5.468e-02+-9.310e-02*_Complex_I, 
        -1.016e-02+-1.011e-01*_Complex_I, -7.739e-02+ 2.040e-01*_Complex_I, 
         2.625e-02+-1.820e-01*_Complex_I,  1.761e-02+-5.792e-03*_Complex_I, 
         5.142e-02+-2.764e-02*_Complex_I,  8.508e-02+-1.741e-01*_Complex_I, 
        -5.252e-02+ 2.439e-02*_Complex_I,  1.067e-01+ 2.441e-01*_Complex_I, 
        -4.582e-03+ 1.308e-01*_Complex_I,  5.419e-03+-6.557e-02*_Complex_I
        };

    float complex y_test[32] = {
         0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+ 0.000e+00*_Complex_I, 
        -0.000e+00+ 0.000e+00*_Complex_I,  0.000e+00+-0.000e+00*_Complex_I, 
         2.124e-02+-4.859e-03*_Complex_I, -2.506e-03+-6.411e-03*_Complex_I, 
        -1.805e-02+ 2.319e-03*_Complex_I, -6.752e-04+ 8.951e-03*_Complex_I, 
        -2.188e-01+ 1.152e-01*_Complex_I, -4.877e-02+ 1.605e-01*_Complex_I, 
         7.475e-02+ 9.823e-02*_Complex_I, -4.468e-02+-1.690e-02*_Complex_I, 
         6.507e-03+ 1.306e-01*_Complex_I,  2.785e-01+-1.418e-01*_Complex_I, 
         3.413e-01+-2.441e-01*_Complex_I,  9.413e-02+ 2.127e-01*_Complex_I, 
         1.875e-01+ 9.516e-03*_Complex_I,  7.891e-02+-6.548e-03*_Complex_I, 
         4.872e-01+-7.034e-02*_Complex_I,  1.964e-01+ 2.585e-01*_Complex_I, 
         1.805e-01+ 6.749e-02*_Complex_I, -7.878e-02+ 4.793e-01*_Complex_I, 
         2.104e-01+ 2.295e-01*_Complex_I, -1.311e-01+ 3.530e-01*_Complex_I, 
        -8.165e-02+ 2.312e-01*_Complex_I,  2.573e-01+ 2.808e-01*_Complex_I, 
        -8.047e-02+ 8.628e-02*_Complex_I, -5.003e-02+ 8.308e-02*_Complex_I, 
        -2.071e-01+ 4.612e-02*_Complex_I, -2.758e-02+ 7.776e-02*_Complex_I, 
        -1.751e-01+ 1.707e-02*_Complex_I,  2.147e-01+-1.373e-01*_Complex_I
        };


    // create channelizer
    firpfbch c = firpfbch_create(num_channels,
                                 m,
                                 beta,
                                 0.0f,
                                 FIRPFBCH_NYQUIST,
                                 FIRPFBCH_ANALYZER);

    float complex y[32];

    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        firpfbch_execute(c, &x[n], &y[n]);
        n += num_channels;
    }

//#if 0
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
//#endif

    for (i=0; i<32; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(y_test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(y_test[i]), tol);
    }

    firpfbch_destroy(c);
}

// 
// AUTOTEST: test sub-band energy
//
void xautotest_firpfbch_analysis_energy()
{
    unsigned int num_channels=8;
    unsigned int m=2;
    float slsl=-60.0f;
    float tol=0.05f;

    float f;
    nco nco_synth = nco_create(LIQUID_VCO);

    firpfbch c = firpfbch_create(num_channels, m, slsl, 0.0f, FIRPFBCH_NYQUIST,FIRPFBCH_ANALYZER);

    unsigned int i, j, k;
    float complex x[num_channels], y[num_channels];

    for (i=0; i<num_channels; i++) {
        // channel center frequency
        f = 2*M_PI * i / (float)num_channels;
        nco_set_frequency(nco_synth, f);

        // generate tone centered on sub-channel (need to generate
        // enough frames to flush buffers of other channels)
        for (j=0; j<4*m; j++) {
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


#endif // __LIQUID_FIRPFBCH_ANALYZER_AUTOTEST_H__


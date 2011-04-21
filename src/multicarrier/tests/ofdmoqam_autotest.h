/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

#include "autotest/autotest.h"
#include "liquid.h"

#define OFDMOQAM_FILENAME "ofdmoqam.m"

// 
// AUTOTEST : perfect reconstruction properties
//
void autotest_ofdmoqam_reconstruction()
{
    // options
    unsigned int num_channels=16;   // must be even number
    unsigned int num_symbols=8;     // num symbols
    unsigned int m=4;               // filter symbol delay
    float beta = 0.80f;             // excess bandwidth factor
    float dt   = 0.0f;              // timing offset (fractional sample) 
    modulation_scheme ms = LIQUID_MODEM_QAM; // modulation scheme
    unsigned int bps = 2;           // modulation depth (bits/symbol)
    float tol = 4e-3f;              // error tolerance

    // derived values
    unsigned int num_frames = num_symbols + 2*m + 1;        // number of frames (compensate for filter delay)
    unsigned int num_samples = num_channels * num_frames;   // number of samples

    // create synthesizer/analyzer objects
    ofdmoqam cs = ofdmoqam_create(num_channels, m, beta, dt, LIQUID_SYNTHESIZER,0);
    ofdmoqam ca = ofdmoqam_create(num_channels, m, beta, dt, LIQUID_ANALYZER,   0);

    // modem
    modem mod = modem_create(ms,bps);

    unsigned int i, j;
    unsigned int s[num_symbols][num_channels];
    float complex X[num_frames][num_channels]; // channelized symbols
    float complex y[num_samples];               // synthesized time-domain samples
    float complex Y[num_frames][num_channels]; // received symbols
    
    // generate random data symbols
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<num_channels; j++) {
            s[i][j] = modem_gen_rand_sym(mod);
        }
    }

    // run modem (generate complex samples)
    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            if (i<num_symbols) {
                modem_modulate(mod,s[i][j],&X[i][j]);
            } else {
                X[i][j] = 0.0f;
            }
        }
    }

        // execute synthesyzer
    for (i=0; i<num_frames; i++)
        ofdmoqam_execute(cs, &X[i][0], &y[i*num_channels]);

    // execute analyzer
    for (i=0; i<num_frames; i++)
        ofdmoqam_execute(ca, &y[i*num_channels], &Y[i][0]);

    // destroy objects
    ofdmoqam_destroy(cs);
    ofdmoqam_destroy(ca);
    modem_destroy(mod);

    // run tests
    float rmse = 0.0f;
    float emax = 0.0f;
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<num_channels; j++) {
            // check reconstructed symbols, compensating for delay (2*m+1)
            CONTEND_DELTA( crealf(Y[i+2*m][j]), crealf(X[i][j]), tol );
            CONTEND_DELTA( cimagf(Y[i+2*m][j]), cimagf(X[i][j]), tol );

            // update error
            float e = cabsf(Y[i+2*m][j] - X[i][j]);
            rmse += e*e;
            if ( (i==0 && j==0) || (e > emax) )
                emax = e;
        }
    }
    rmse = sqrtf(rmse/(float)(num_symbols*num_channels));

    if (liquid_autotest_verbose) {
        printf("reconstruction error:\n");
        printf("  rmse : %12.4e\n", rmse);
        printf("  emax : %12.4e\n", emax);
    }
}


// 
// AUTOTEST : output synthesis signal level
//
void autotest_ofdmoqam_synthesis_level()
{
    // options
    unsigned int num_channels=64;   // must be even number
    unsigned int num_symbols=30;    // num symbols
    unsigned int m=4;               // filter symbol delay
    float beta = 0.80f;             // excess bandwidth factor
    float dt   = 0.0f;              // timing offset (fractional sample) 
    modulation_scheme ms = LIQUID_MODEM_QAM; // modulation scheme
    unsigned int bps = 2;           // modulation depth (bits/symbol)
    float tol = 1e-3f;              // error tolerance

    // derived values
    unsigned int num_frames = num_symbols + 2*m;            // number of frames (compensate for filter delay)
    unsigned int num_samples = num_channels * num_frames;   // number of samples

    // create synthesizer/analyzer objects
    ofdmoqam cs = ofdmoqam_create(num_channels, m, beta, dt, LIQUID_SYNTHESIZER,0);

    // modem
    modem mod = modem_create(ms,bps);

    unsigned int i, j;
    float complex X[num_frames][num_channels]; // channelized symbols
    float complex y[num_samples];               // synthesized time-domain samples
    
    unsigned int s;
    msequence mseq = msequence_create_default(6);

    // generate frame data
    for (i=0; i<num_frames; i++) {
        for (j=0; j<num_channels; j++) {
            s = msequence_generate_symbol(mseq, bps);
            modem_modulate(mod,s,&X[i][j]);
        }
    }

    msequence_destroy(mseq);

    // execute synthesyzer
    for (i=0; i<num_frames; i++)
        ofdmoqam_execute(cs, &X[i][0], &y[i*num_channels]);

#if 0
    // output debugging file
    FILE * fid = fopen("ofdmoqam_synthesis_level.m", "w");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    fprintf(fid,"t = 0:(%u-1);\n", num_samples);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y), t,imag(y));\n");
    fclose(fid);
    printf("results written to 'ofdmoqam_synthesis_level.m'\n");
#endif

    // destroy objects
    ofdmoqam_destroy(cs);
    modem_destroy(mod);

    // compute signal level on resulting time series
    float e2 = 0.0f;
    unsigned int n=0;
    // remove effects of filter delay
    for (i=m*num_channels; i<num_samples; i++) {
        e2 += crealf( y[i]*conjf(y[i]) );
        n++;
    }
    float rms = sqrtf( e2/n );
    if (liquid_autotest_verbose)
        printf("  ofdm/oqam rms level : %12.8f (expected 1)\n", rms);
    CONTEND_DELTA( rms, 1.0f, tol );

}


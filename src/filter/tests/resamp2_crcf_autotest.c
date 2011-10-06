/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

// 
// AUTOTEST : test half-band filterbank (analyzer)
//
void autotest_resamp2_analysis()
{
    unsigned int m=5;       // filter semi-length (actual length: 4*m+1)
    unsigned int n=37;      // number of input samples
    float As=60.0f;         // stop-band attenuation [dB]
    float f0 =  0.0739f;    // low frequency signal
    float f1 = -0.1387f;    // high frequency signal (+pi)
    float tol = 1e-3f;      // error tolerance

    unsigned int i;

    // allocate memory for data arrays
    float complex x[2*n+2*m+1]; // input signal (with delay)
    float complex y0[n];        // low-pass output
    float complex y1[n];        // high-pass output

    // generate the baseband signal
    for (i=0; i<2*n+2*m+1; i++)
        x[i] = i < 2*n ? cexpf(_Complex_I*f0*i) + cexpf(_Complex_I*(M_PI+f1)*i) : 0.0f;

    // create/print the half-band resampler, with a specified
    // stopband attenuation level
    resamp2_crcf q = resamp2_crcf_create(m,0,As);

    // run half-band decimation
    float complex y_hat[2];
    for (i=0; i<n; i++) {
        resamp2_crcf_analyzer_execute(q, &x[2*i], y_hat);
        y0[i] = y_hat[0];
        y1[i] = y_hat[1];
    }

    // clean up allocated objects
    resamp2_crcf_destroy(q);

    // validate output
    for (i=m; i<n-m; i++) {
        CONTEND_DELTA( crealf(y0[i+m]), cosf(2*f0*(i+0.5f)), tol );
        CONTEND_DELTA( cimagf(y0[i+m]), sinf(2*f0*(i+0.5f)), tol );

        CONTEND_DELTA( crealf(y1[i+m]), cosf(2*f1*(i+0.5f)), tol );
        CONTEND_DELTA( cimagf(y1[i+m]), sinf(2*f1*(i+0.5f)), tol );
    }

#if 0
    // debugging
    FILE * fid = fopen("resamp2_test.m", "w");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    for (i=0; i<2*n; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<n; i++) {
        fprintf(fid,"y0(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y0[i]), cimagf(y0[i]));
        fprintf(fid,"y1(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y1[i]), cimagf(y1[i]));
    }

    // save expected values
    for (i=0; i<n; i++) {
        fprintf(fid,"z0(%3u) = %12.4e + j*%12.4e;\n", i+1, cosf(i*2*f0), sinf(i*2*f0));
        fprintf(fid,"z1(%3u) = %12.4e + j*%12.4e;\n", i+1, cosf(i*2*f1), sinf(i*2*f1));
    }

    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(length(y0)-1);\n");
    //fprintf(fid,"plot(t,real(z0),t-m+0.5,real(y0));\n");
    fprintf(fid,"plot(t,real(z1),t-m+0.5,real(y1));\n");
    fclose(fid);
    printf("results written to '%s'\n","resamp2_test.m");
#endif
}

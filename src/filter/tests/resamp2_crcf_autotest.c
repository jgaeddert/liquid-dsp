/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

// 
// AUTOTEST : test half-band filterbank (synthesizer)
//
void autotest_resamp2_synthesis()
{
    unsigned int m=5;       // filter semi-length (actual length: 4*m+1)
    unsigned int n=37;      // number of input samples
    float As=60.0f;         // stop-band attenuation [dB]
    float f0 =  0.0739f;    // low frequency signal
    float f1 = -0.1387f;    // high frequency signal (+pi)
    float tol = 3e-3f;      // error tolerance

    unsigned int i;

    // allocate memory for data arrays
    float complex x0[n+2*m+1];  // input signal (with delay)
    float complex x1[n+2*m+1];  // input signal (with delay)
    float complex y[2*n];       // synthesized output

    // generate the baseband signals
    for (i=0; i<n+2*m+1; i++) {
        x0[i] = i < 2*n ? cexpf(_Complex_I*f0*i) : 0.0f;
        x1[i] = i < 2*n ? cexpf(_Complex_I*f1*i) : 0.0f;
    }

    // create/print the half-band resampler, with a specified
    // stopband attenuation level
    resamp2_crcf q = resamp2_crcf_create(m,0,As);

    // run synthesis
    float complex x_hat[2];
    for (i=0; i<n; i++) {
        x_hat[0] = x0[i];
        x_hat[1] = x1[i];
        resamp2_crcf_synthesizer_execute(q, x_hat, &y[2*i]);
    }

    // clean up allocated objects
    resamp2_crcf_destroy(q);

    // validate output
    for (i=m; i<n-2*m; i++) {
        CONTEND_DELTA( crealf(y[i+2*m]), cosf(0.5f*f0*i) + cosf((M_PI+0.5f*f1)*i), tol );
        CONTEND_DELTA( cimagf(y[i+2*m]), sinf(0.5f*f0*i) + sinf((M_PI+0.5f*f1)*i), tol );
    }

#if 0
    // debugging
    FILE * fid = fopen("resamp2_test.m", "w");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    for (i=0; i<n+2*m+1; i++) {
        fprintf(fid,"x0(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x0[i]), cimagf(x0[i]));
        fprintf(fid,"x1(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x1[i]), cimagf(x1[i]));
    }

    for (i=0; i<2*n; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // save expected values
    for (i=0; i<2*n; i++) {
        fprintf(fid,"z(%3u) = %12.4e + j*%12.4e;\n", i+1, cosf(i*0.5f*f0) + cosf(i*(M_PI+0.5f*f1)),
                                                          sinf(i*0.5f*f0) + sinf(i*(M_PI+0.5f*f1)));
    }

    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(length(y)-1);\n");
    fprintf(fid,"plot(t,real(z),t-2*m,real(y));\n");
    fclose(fid);
    printf("results written to '%s'\n","resamp2_test.m");
#endif
}

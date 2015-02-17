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
// AUTOTEST : test arbitrary resampler
//
void autotest_resamp_crcf()
{
    // options
    unsigned int m = 13;        // filter semi-length (filter delay)
    float r=1.27115323f;        // resampling rate (output/input)
    float bw=0.45f;             // resampling filter bandwidth
    float As=60.0f;             // resampling filter stop-band attenuation [dB]
    unsigned int npfb=64;       // number of filters in bank (timing resolution)
    unsigned int n=400;         // number of input samples
    float fx=0.254230646f;      // complex input sinusoid frequency (0.2*r)

    unsigned int i;

    // number of input samples (zero-padded)
    unsigned int nx = n + m;

    // output buffer with extra padding for good measure
    unsigned int y_len = (unsigned int) ceilf(1.1 * nx * r) + 4;

    // arrays
    float complex x[nx];
    float complex y[y_len];

    // create resampler
    resamp_crcf q = resamp_crcf_create(r,m,bw,As,npfb);

    // generate input signal
    float wsum = 0.0f;
    for (i=0; i<nx; i++) {
        // compute window
        float w = i < n ? kaiser(i, n, 10.0f, 0.0f) : 0.0f;

        // apply window to complex sinusoid
        x[i] = cexpf(_Complex_I*2*M_PI*fx*i) * w;

        // accumulate window
        wsum += w;
    }

    // resample
    unsigned int ny=0;
    unsigned int nw;
    for (i=0; i<nx; i++) {
        // execute resampler, storing in output buffer
        resamp_crcf_execute(q, x[i], &y[ny], &nw);

        // increment output size
        ny += nw;
    }

    // clean up allocated objects
    resamp_crcf_destroy(q);

    // 
    // analyze resulting signal
    //

    // check that the actual resampling rate is close to the target
    float r_actual = (float)ny / (float)nx;
    float fy = fx / r;      // expected output frequency

    // run FFT and ensure that carrier has moved and that image
    // frequencies and distortion have been adequately suppressed
    unsigned int nfft = 1 << liquid_nextpow2(ny);
    float complex yfft[nfft];   // fft input
    float complex Yfft[nfft];   // fft output
    for (i=0; i<nfft; i++)
        yfft[i] = i < ny ? y[i] : 0.0f;
    fft_run(nfft, yfft, Yfft, LIQUID_FFT_FORWARD, 0);
    fft_shift(Yfft, nfft);  // run FFT shift

    // find peak frequency
    float Ypeak = 0.0f;
    float fpeak = 0.0f;
    float max_sidelobe = -1e9f;     // maximum side-lobe [dB]
    float main_lobe_width = 0.07f;  // TODO: figure this out from Kaiser's equations
    for (i=0; i<nfft; i++) {
        // normalized output frequency
        float f = (float)i/(float)nfft - 0.5f;

        // scale FFT output appropriately
        Yfft[i] /= (r * wsum);
        float Ymag = 20*log10f( cabsf(Yfft[i]) );

        // find frequency location of maximum magnitude
        if (Ymag > Ypeak || i==0) {
            Ypeak = Ymag;
            fpeak = f;
        }

        // find peak side-lobe value, ignoring frequencies
        // within a certain range of signal frequency
        if ( fabsf(f-fy) > main_lobe_width )
            max_sidelobe = Ymag > max_sidelobe ? Ymag : max_sidelobe;
    }

    if (liquid_autotest_verbose) {
        // print results
        printf("  desired resampling rate   :   %12.8f\n", r);
        printf("  measured resampling rate  :   %12.8f    (%u/%u)\n", r_actual, ny, nx);
        printf("  peak spectrum             :   %12.8f dB (expected 0.0 dB)\n", Ypeak);
        printf("  peak frequency            :   %12.8f    (expected %-12.8f)\n", fpeak, fy);
        printf("  max sidelobe              :   %12.8f dB (expected at least %.2f dB)\n", max_sidelobe, -As);
    }
    CONTEND_DELTA(     r_actual, r,    0.01f ); // check actual output sample rate
    CONTEND_DELTA(     Ypeak,    0.0f, 0.25f ); // peak should be about 0 dB
    CONTEND_DELTA(     fpeak,    fy,   0.01f ); // peak frequency should be nearly 0.2
    CONTEND_LESS_THAN( max_sidelobe, -As );     // maximum side-lobe should be sufficiently low

#if 0
    // export results for debugging
    char filename[] = "resamp_crcf_autotest.m";
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n",filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"r    = %12.8f;\n", r);
    fprintf(fid,"nx   = %u;\n", nx);
    fprintf(fid,"ny   = %u;\n", ny);
    fprintf(fid,"nfft = %u;\n", nfft);

    fprintf(fid,"Y = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"Y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(Yfft[i]), cimagf(Yfft[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot frequency-domain result\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,20*log10(abs(Y)),'Color',[0.25 0.5 0.0],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -120 20]);\n");

    fclose(fid);
    printf("results written to %s\n",filename);
#endif
}

/*
 * Copyright (c) 2007 - 2019 Joseph Gaeddert
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

// convenience methods
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

// 
// AUTOTEST : test rational-rate resampler
//
void test_harness_rresamp_crcf(unsigned int _P,
                               unsigned int _Q,
                               unsigned int _m,
                               float        _bw,
                               float        _As)
{
    // target about 1500 max samples
    unsigned int  n  = 1500 / max(_P,_Q);
    unsigned int  ny = n * _P;  // number of output samples
    unsigned int  nx = n * _Q;  // number of input samples

    // buffers
    float complex x[nx];        // input sample buffer
    float complex y[ny];        // output sample buffer

    // create resampler
    rresamp_crcf q = rresamp_crcf_create_kaiser(_P, _Q, _m, _bw, _As);
    float r = rresamp_crcf_get_rate(q);

    // generate input signal (windowed sinusoid)
    unsigned int i;
    float        wsum = 0.0f;
    unsigned int wlen = (n - _m)*_Q;
    float        fx = 0.25f*(r > 1.0f ? 1.0f : r);   // input tone frequency
    for (i=0; i<nx; i++) {
        // compute window
        float w = i < wlen ? kaiser(i, wlen, 10.0f, 0.0f) : 0.0f;

        // apply window to complex sinusoid
        x[i] = cexpf(_Complex_I*2*M_PI*fx*i) * w;

        // accumulate window
        wsum += w;
    }

    // resample input in blocks
    for (i=0; i<n; i++)
        rresamp_crcf_execute(q, &x[i*_Q], &y[i*_P]);

    // clean up allocated objects
    rresamp_crcf_destroy(q);

    // 
    // analyze resulting signal
    //

    // check that the actual resampling rate is close to the target
    float fy = fx / r;      // expected output frequency

    // run FFT and ensure that carrier has moved and that image
    // frequencies and distortion have been adequately suppressed
    unsigned int nfft = 8192;   // about 1500 max samples
    float complex yfft[nfft];   // fft input
    float complex Yfft[nfft];   // fft output
    for (i=0; i<nfft; i++)
        yfft[i] = i < ny ? y[i]/(wsum*sqrt(r)) : 0.0f;
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
        printf("test_harness_rresamp_crcf(P=%u, Q=%u, m=%u, A=%.3f)\n", _P, _Q, _m, _As);
        printf("  desired resampling rate   :   %12.8f = %u / %u\n", r, _P, _Q);
        printf("  frequency (input)         :   %12.8f / Fs\n", fx);
        printf("  peak spectrum             :   %12.8f dB (expected 0.0 dB)\n", Ypeak);
        printf("  peak frequency            :   %12.8f    (expected %-12.8f)\n", fpeak, fy);
        printf("  max sidelobe              :   %12.8f dB (expected at least %.2f dB)\n", max_sidelobe, -_As);
    }
    CONTEND_DELTA(     Ypeak,    0.0f, 0.25f ); // peak should be about 0 dB
    CONTEND_DELTA(     fpeak,    fy,   0.01f ); // peak frequency should be nearly 0.2
    CONTEND_LESS_THAN( max_sidelobe, -_As );    // maximum side-lobe should be sufficiently low

#if 0
    // export results for debugging
    char filename[256] = "";
    sprintf(filename,"rresamp_crcf_autotest_P%u_Q%u.m", _P, _Q);
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n",filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"r    = %12.8f;\n", r);
    fprintf(fid,"fx   = %.4e;\n", fx);
    fprintf(fid,"nx   = %u;\n", nx);
    fprintf(fid,"ny   = %u;\n", ny);
    fprintf(fid,"wsum = %.4e;\n", wsum);
    fprintf(fid,"nfft = %u;\n", nfft);

    fprintf(fid,"x = zeros(1,nx);\n");
    for (i=0; i<nx; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    fprintf(fid,"y = zeros(1,ny);\n");
    for (i=0; i<ny; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot frequency-domain result\n");
    fprintf(fid,"X = 20*log10(abs(fftshift(fft(x / (wsum        ), nfft))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y / (wsum*sqrt(r)), nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure('color','white','position',[100 100 800 800]);\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"    plot(f,  X,'Color',[0.5 0.5 0.5],'LineWidth',2);\n");
    fprintf(fid,"    plot(f*r,Y,'Color',[0.0 0.3 0.5],'LineWidth',2);\n");
    fprintf(fid,"    plot(fx, 0, 'or');\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  fmax = max(0.5,0.5*r);\n");
    fprintf(fid,"  axis([-fmax fmax -140 20]);\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  hold on;\n");
    fprintf(fid,"    plot(f,  X,'Color',[0.5 0.5 0.5],'LineWidth',2);\n");
    fprintf(fid,"    plot(f*r,Y,'Color',[0.0 0.3 0.5],'LineWidth',2);\n");
    fprintf(fid,"    plot(fx, 0, 'or');\n");
    fprintf(fid,"  hold off;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  axis([0.98*fx 1.02*fx -3 0.5]);\n");
    fprintf(fid,"  legend('original','resampled','location','northeast');\n");

    fclose(fid);
    printf("results written to %s\n",filename);
#endif
}

// actual tests
void autotest_rresamp_crcf_P1_Q5() { test_harness_rresamp_crcf( 1, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P2_Q5() { test_harness_rresamp_crcf( 2, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P3_Q5() { test_harness_rresamp_crcf( 3, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P6_Q5() { test_harness_rresamp_crcf( 6, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P8_Q5() { test_harness_rresamp_crcf( 8, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P9_Q5() { test_harness_rresamp_crcf( 9, 5, 15, 0.4f, 60.0f); }


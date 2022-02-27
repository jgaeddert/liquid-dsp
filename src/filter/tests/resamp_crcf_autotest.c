/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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
// AUTOTEST : test arbitrary resampler by observing spectral content
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
        float w = i < n ? liquid_kaiser(i, n, 10.0f) : 0.0f;

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

// test arbitrary resampler output length calculation
void testbench_resamp_crcf_num_output(float _rate, unsigned int _npfb)
{
    if (liquid_autotest_verbose)
        printf("testing resamp_crcf_get_num_output() with r=%g, npfb=%u\n", _rate, _npfb);

    // create object
    float fc = 0.4f;
    float As = 60.0f;
    unsigned int m = 20;
    resamp_crcf q = resamp_crcf_create(_rate, m, fc, As, _npfb);

    // sizes to test in sequence
    unsigned int sizes[10] = {1, 2, 3, 20, 7, 64, 4, 4, 4, 27};

    // allocate buffers (over-provision output to help avoid segmentation faults on error)
    unsigned int max_input = 64;
    unsigned int max_output = 16 + (unsigned int)(4.0f * max_input * _rate);
    printf("max_input : %u, max_output : %u\n", max_input, max_output);
    float complex buf_0[max_input];
    float complex buf_1[max_output];
    unsigned int i;
    for (i=0; i<max_input; i++)
        buf_0[i] = 0.0f;

    // run numerous blocks
    unsigned int b;
    for (b=0; b<8; b++) {
        for (i=0; i<10; i++) {
            unsigned int num_input  = sizes[i];
            unsigned int num_output = resamp_crcf_get_num_output(q, num_input);
            unsigned int num_written;
            resamp_crcf_execute_block(q, buf_0, num_input, buf_1, &num_written);
            if (liquid_autotest_verbose) {
                printf(" b[%2u][%2u], num_input:%5u, num_output:%5u, num_written:%5u\n",
                        b, i, num_input, num_output, num_written);
            }
            CONTEND_EQUALITY(num_output, num_written)
        }
    }

    // destroy object
    resamp_crcf_destroy(q);
}

void autotest_resamp_crcf_num_output_0(){ testbench_resamp_crcf_num_output(1.00f,     64); }
void autotest_resamp_crcf_num_output_1(){ testbench_resamp_crcf_num_output(1.00f,    256); }
void autotest_resamp_crcf_num_output_2(){ testbench_resamp_crcf_num_output(0.50f,    256); }
void autotest_resamp_crcf_num_output_3(){ testbench_resamp_crcf_num_output(sqrtf( 2),256); }
void autotest_resamp_crcf_num_output_4(){ testbench_resamp_crcf_num_output(sqrtf(17), 16); }
void autotest_resamp_crcf_num_output_5(){ testbench_resamp_crcf_num_output(1.0f/M_PI, 64); }
void autotest_resamp_crcf_num_output_6(){ testbench_resamp_crcf_num_output(expf(5.0f),64); }
void autotest_resamp_crcf_num_output_7(){ testbench_resamp_crcf_num_output(expf(-5.f),64); }


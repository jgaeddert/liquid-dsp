/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
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

// test spectral waterfall (spwaterfall) objects

#include <stdlib.h>
#include <string.h>
#include "autotest/autotest.h"
#include "liquid.internal.h"

int testbench_spwaterfallcf_compare(const void * _v0, const void * _v1)
    { return *(float*)_v0 > *(float*)_v1 ? 1 : -1; }

void testbench_spwaterfallcf_noise(unsigned int _nfft,
                                   unsigned int _window_len,
                                   unsigned int _delay,
                                   unsigned int _time,
                                   float        _noise_floor)
{
    unsigned int num_samples = 10*_nfft*_time;  // number of samples to generate
    float        nstd        = powf(10.0f,_noise_floor/20.0f); // noise std. dev.
    float        tol         = 4.0f; // error tolerance [dB] TODO: drop tolerance to 0.5 dB
    int _wtype = LIQUID_WINDOW_HAMMING;

    // create spectral periodogram
    spwaterfallcf q = spwaterfallcf_create(_nfft, _wtype, _window_len, _delay, _time);

    unsigned int i;
    for (i=0; i<num_samples; i++)
        spwaterfallcf_push(q, nstd*( randnf() + _Complex_I*randnf() ) * M_SQRT1_2);

    // verify number of samples processed
    CONTEND_EQUALITY(spwaterfallcf_get_num_samples_total(q), num_samples);

    // compute power spectral density output
    const float * psd = spwaterfallcf_get_psd(q);
    unsigned int time = spwaterfallcf_get_num_time(q);

#if 0
    // export results for testing
    FILE * fid = fopen("spwaterfallcf_testbench.m", "w");
    fprintf(fid,"clear all; close all; psd=zeros(%u,%u);\n", _nfft, time);
    for (i=0; i<_nfft*time; i++)
        fprintf(fid,"psd(%u) = %g;\n", i+1, psd[i]);
    fclose(fid);
#endif
    // compute median value; should be close to noise floor
    float * v = (float*) malloc(_nfft*time*sizeof(float));
    memmove(v, psd, _nfft*time*sizeof(float));
    qsort(v, _nfft*time, sizeof(float), &testbench_spwaterfallcf_compare);
    float median = v[_nfft*time/2];
    if (liquid_autotest_verbose) {
        printf("  spwaterfallcf_test(noise): nfft:%4u, wtype:%s, n0:%6.1f, est:%6.1f, tol:%5.2f\n",
                _nfft, liquid_window_str[_wtype][1], _noise_floor, median, tol);
    }
    CONTEND_DELTA(median, _noise_floor, tol)

    // destroy objects and free memory
    free(v);
    spwaterfallcf_destroy(q);
}

// test different transform sizes
void autotest_spwaterfallcf_noise_440()  { testbench_spwaterfallcf_noise( 440, 320, 10, 240, -80.0); }
void autotest_spwaterfallcf_noise_1024() { testbench_spwaterfallcf_noise( 680, 320, 10, 640, -80.0); }
void autotest_spwaterfallcf_noise_1200() { testbench_spwaterfallcf_noise(1200, 320, 10, 800, -80.0); }


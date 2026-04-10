/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.h"

// Use Parks-McClellan FIR filter design method to find transition
// band which gives time-series response as close to ideal as possible
// (center coefficient 1/2, and all other even coefficients 0).
//
//  H(f/Fs)
//   ^             ~ft
//   |           |<--->|
//   |. . . . . . . . . . . . . . . . +Ap/2
//   |/\/\/\/\/\/|
//   |. . . . . . .\. . . . . . . . . -Ap/2
//   |           |  \.
//   |           |   \.
//   |           |  : \. . . . . . . -As
//   |           |  :  \/^\ /^\ /^\ /|
//   |           |  :  |   |   |   | |
//   0           :  :  :            1/2   ---> f/Fs
//               :  :  f1 = 1/4 + ft/2
//               : 1/4
//              f0 = 1/4 - gamma*ft/2

// structured data type
struct firdespm_halfband_s
{
    // top-level filter design parameters
    unsigned int    m;          // filter semi-length
    unsigned int    h_len;      // filter length, 4*m+1
    float           ft;         // desired transition band
    float *         h;          // resulting filter coefficients

    // utility calculation
    unsigned int    nfft;       // transform size for analysis
    liquid_float_complex * buf_time;   // time buffer
    liquid_float_complex * buf_freq;   // frequency buffer
    fftplan         fft;        // transform object
};

// pointer to struct
typedef struct firdespm_halfband_s * firdespm_halfband;

// create object
firdespm_halfband firdespm_halfband_create(unsigned int _m, float _ft)
{
    // create and initialize object
    firdespm_halfband q = (firdespm_halfband)malloc(sizeof(struct firdespm_halfband_s));
    q->m     = _m;
    q->h_len = 4*_m+1;
    q->ft    = _ft;
    q->h     = (float*)malloc(q->h_len*sizeof(float));

    // initialize values for utility calculation
    q->nfft = 1200;
    while (q->nfft < 120*q->m)
        q->nfft <<= 1;
    q->buf_time = (liquid_float_complex*) fft_malloc(q->nfft*sizeof(liquid_float_complex));
    q->buf_freq = (liquid_float_complex*) fft_malloc(q->nfft*sizeof(liquid_float_complex));
    q->fft      = fft_create_plan(q->nfft, q->buf_time, q->buf_freq, LIQUID_FFT_FORWARD, 0);

    // ensure entire input buffer is empty
    memset(q->buf_time, 0x00, q->nfft * sizeof(liquid_float_complex));

    //
    return q;
}

// destroy object, freeing all internal memory
int firdespm_halfband_destroy(firdespm_halfband _q)
{
    free(_q->h);
    fft_destroy_plan(_q->fft);
    fft_free(_q->buf_time);
    fft_free(_q->buf_freq);
    free(_q);
    return LIQUID_OK;
}

// design filter with particular cutoff frequencies
int firdespm_halfband_design(firdespm_halfband _q, float _f0, float _f1)
{
    // design filter
    float bands[4]   = {0.00f, _f0, _f1, 0.50f};
    float des[2]     = {1.0f, 0.0f};
    float weights[2] = {1.0f, 1.0f}; // best with {1, 1}
    liquid_firdespm_wtype wtype[2] = {
        LIQUID_FIRDESPM_FLATWEIGHT, LIQUID_FIRDESPM_EXPWEIGHT,};
    return firdespm_run(_q->h_len, 2, bands, des, weights, wtype,
        LIQUID_FIRDESPM_BANDPASS, _q->h);
}

// callback function to design and evaluate filter based on the expectation
// that even-indexed coefficients (besides center coefficient) should be 0
float firdespm_halfband_utility(float _gamma, void * _userdata)
{
    // type-cast input structure as pointer
    firdespm_halfband q = (firdespm_halfband)_userdata;

    // design filter
    float f0 = 0.25f - 0.5f*q->ft*_gamma;
    float f1 = 0.25f + 0.5f*q->ft;
    firdespm_halfband_design(q, f0, f1);

    // compute utility: deviation from zero for even coefficients
    float u = 0.0f;
    unsigned int i;
    for (i=0; i<q->m; i++)
        u += q->h[2*i] * q->h[2*i];

    // force zeros for even coefficients
    for (i=0; i<q->m; i++) {
        q->h[           2*i] = 0;
        q->h[q->h_len-2*i-1] = 0;
    }

    // force center coefficient to be exactly 1/2
    q->h[q->h_len/2] = 0.5;
    return 10*log10f(u);
}

// function to evaluate the frequency response of a filter already designed
float firdespm_halfband_evaluate_stopband(firdespm_halfband _q)
{
    // copy coefficients to input buffer
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->buf_time[i] = i < _q->h_len ? _q->h[i] : 0.0f;

    // compute transform
    fft_execute(_q->fft);

    // compute metric: maximum value in stop-band
    unsigned int n = (unsigned int)(_q->nfft * (0.25f - 0.5f*_q->ft));
    float u = 0.0f;
    for (i=0; i<n; i++) {
        unsigned int idx = _q->nfft/2 - i;
        float u_test = cabsf(_q->buf_freq[idx]);
        //printf(" %3u : %12.8f : %12.3f\n", i, (float)(idx) / (float)(_q->nfft), 20*log10f(u_test));
        if (i==0 || u_test > u)
            u = u_test;
    }

    // return utility in dB
    return 20*log10f(u);
}

// perform search to find optimal coefficients given transition band
int firdespm_halfband_optimize_ft(firdespm_halfband _q, float * _h)
{
    // create optimizer and run search
    qs1dsearch optim = qs1dsearch_create(firdespm_halfband_utility, _q, LIQUID_OPTIM_MINIMIZE);
    qs1dsearch_init_bounds(optim, 1.0f, 0.9f);

    // run optimizer, allowing for early stopping
    unsigned int i;
    float u_prime = 0; // previous utility
    unsigned int persistence = 8, count = 0;
    for (i=0; i<32 && count < persistence; i++) {
        qs1dsearch_step (optim);
        //qs1dsearch_print(optim);

        // early stopping; break if utility hasn't improved in 'persistence' steps
        float u = qs1dsearch_get_opt_u(optim);
        if (i > 0)
            count = (u >= u_prime) ? count+1 : 0;

        u_prime = u;
    }

    // copy optimal coefficients
    memmove(_h, _q->h, _q->h_len*sizeof(float));

    return qs1dsearch_destroy(optim);
}

// perform search to find optimal coefficients given transition band
int liquid_firdespm_halfband_ft(unsigned int _m, float _ft, float * _h)
{
    // create and initialize object
    firdespm_halfband q = firdespm_halfband_create(_m, _ft);

    // optimize for transition band
    firdespm_halfband_optimize_ft(q, _h);

    // destroy objects
    return firdespm_halfband_destroy(q);
}

// perform search to find optimal coefficients given stop-band suppression
// NOTE: this is accurate but slow as it must iterate over the search for
//       a halfband filter design with a specific transition band
int liquid_firdespm_halfband_as(unsigned int _m, float _as, float * _h)
{
    // estimate transition band given other parameters
    float ft = estimate_req_filter_df(_as, 4*_m+1);

    // create and initialize object
    firdespm_halfband q = firdespm_halfband_create(_m, ft);

    // find boundaries and run bisection search
    float ft_0 = -1, ft_1 = -1;
    float as_0 = -1, as_1 = -1;

    // run in loop until desired stop-band attenuation is achieved
    unsigned int i;
    float alpha = 0.9f;
    for (i=0; i<10; i++)
    {
        // optimize for transition band
        firdespm_halfband_optimize_ft(q, _h);

        // evaluate stop-band as "positive attenuation"
        float as = -firdespm_halfband_evaluate_stopband(q);
        //printf("%3u: ft:{%12.8f,%12.8f}, as:{%12.8f,%12.8f} -> ft = %12.8f, as = %12.8f\n",
        //    i, ft_0, ft_1, as_0, as_1, ft, as);

        // early-exit criteria
        if (as > _as && (as - _as) < 0.05)
            break;
        // check bounds
        if (as > _as) { // ft is too large
            ft_1 = ft;
            as_1 = as;
        } else { // ft is too small
            ft_0 = ft;
            as_0 = as;
        }

        // check if either upper or lower bound is open
        if (ft_0 < 0)
            ft = alpha*ft; // nudge ft closer to 0
        else if (ft_1 < 0)
            ft = 1 - alpha*(1-ft); // nudge ft closer to 1 without exceeding
        else {
            // linear interpolation
            float del = (_as - as_0)/(as_1 - as_0);
            ft = ft_0 + del*(ft_1 - ft_0);
        }

        // set transition bandwidth within object
        q->ft = ft;
    }

    // destroy objects
    return firdespm_halfband_destroy(q);
}


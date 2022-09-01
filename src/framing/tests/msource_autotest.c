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
#include "liquid.internal.h"

// user-defined callback; generate tones
int callback_msourcecf_autotest(void *          _userdata,
                                float complex * _v,
                                unsigned int    _n)
{
    unsigned int * counter = (unsigned int*)_userdata;
    unsigned int i;
    for (i=0; i<_n; i++) {
        _v[i] = *counter==0 ? 1 : 0;
        *counter = (*counter+1) % 8;
    }
    return 0;
}

//void testbench_firdes_prototype(const char * _type,
//                                unsigned int _k,
//                                unsigned int _m,
//                                float        _beta,
//                                float        _as)
void autotest_msourecf_psd()
{
    // msource parameters
    int          ms     = LIQUID_MODEM_QPSK;    // linear modulation scheme
    unsigned int m      =    12;                // modulation filter semi-length
    float        beta   = 0.30f;                // modulation filter excess bandwidth factor
    float        bt     = 0.35f;                // GMSK filter bandwidth-time factor

    // spectral periodogram options
    unsigned int nfft        =   2400;  // spectral periodogram FFT size
    unsigned int num_samples = 192000;  // number of samples

    // create spectral periodogram
    spgramcf periodogram = spgramcf_create_default(nfft);

    unsigned int buf_len = 1024;
    float complex buf[buf_len];

    // create multi-signal source generator
    msourcecf gen = msourcecf_create_default();

    // add signals     (gen,  fc,   bw,    gain, {options})
    msourcecf_add_noise(gen,  0.0f, 1.00f, -40);               // wide-band noise
    msourcecf_add_noise(gen,  0.0f, 0.20f,   0);               // narrow-band noise
    msourcecf_add_tone (gen, -0.4f, 0.00f,  20);               // tone
    msourcecf_add_modem(gen,  0.2f, 0.10f,   0, ms, m, beta);  // modulated data (linear)
    msourcecf_add_gmsk (gen, -0.2f, 0.05f,   0, m, bt);        // modulated data (GMSK)
    unsigned int counter = 0;
    msourcecf_add_user (gen,  0.4f, 0.15f, -10, (void*)&counter, callback_msourcecf_autotest); // tones

    // print source generator object
    msourcecf_print(gen);

    unsigned int total_samples = 0;
    while (total_samples < num_samples) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf, buf_len);

        // push resulting sample through periodogram
        spgramcf_write(periodogram, buf, buf_len);

        // accumulated samples
        total_samples += buf_len;
    }
    printf("total samples: %u\n", total_samples);

    // compute power spectral density output
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    // destroy objects
    msourcecf_destroy(gen);
    spgramcf_destroy(periodogram);

    // verify interpolated spectrum
    autotest_psd_s regions[] = {
      // noise floor between signals
      {.fmin=-0.500,.fmax=-0.410, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=-0.390,.fmax=-0.260, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=-0.140,.fmax=-0.110, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.110,.fmax=+0.130, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.270,.fmax=+0.320, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      // space between tones
      {.fmin=+0.328,.fmax=+0.338, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.348,.fmax=+0.358, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.368,.fmax=+0.378, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.386,.fmax=+0.396, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.405,.fmax=+0.415, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.424,.fmax=+0.434, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      {.fmin=+0.442,.fmax=+0.452, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      // end
      {.fmin=+0.461,.fmax=+0.500, .pmin=-43.0, .pmax=-37.0, .test_lo=1, .test_hi=1},
      // signals
      {.fmin=-0.401,.fmax=-0.399, .pmin=+10.0, .pmax=+22.0, .test_lo=1, .test_hi=1},
      {.fmin=-0.220,.fmax=-0.180, .pmin=-18.0, .pmax= +6.5, .test_lo=1, .test_hi=1},
      {.fmin=-0.095,.fmax=+0.095, .pmin= -5.0, .pmax= +2.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.160,.fmax=+0.240, .pmin= -5.0, .pmax= +2.0, .test_lo=1, .test_hi=1},
      // tones
      {.fmin= 0.3245,.fmax=+0.3255, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.3432,.fmax=+0.3442, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.3620,.fmax=+0.3630, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.3810,.fmax=+0.3820, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.3995,.fmax=+0.4005, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.4182,.fmax=+0.4192, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.4370,.fmax=+0.4380, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.4555,.fmax=+0.4565, .pmin=-20.0, .pmax= +0.0, .test_lo=1, .test_hi=1},
    };
    char filename[256];
    sprintf(filename,"autotest/logs/msource_autotest.m");
    liquid_autotest_validate_spectrum(psd, nfft, regions, 5+7+1+4+8,
        liquid_autotest_verbose ? filename : NULL);
}



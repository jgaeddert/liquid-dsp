/*
 * Copyright (c) 2007 - 2024 Joseph Gaeddert
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

#include <assert.h>
#include "autotest/autotest.h"
#include "liquid.h"

void autotest_firpfbchr_crcf()
{
    // options
    unsigned int M = 16;            // number of channels
    unsigned int P =  6;            // output decimation rate
    unsigned int m =  5;            // filter semi-length (symbols)
    unsigned int num_blocks=1<<16;  // number of symbols
    float As = 60.0f;               // filter stop-band attenuation
    
    unsigned int i;
    unsigned int channel_id = 3;

    // create filterbank objects from prototype
    firpfbchr_crcf qa = firpfbchr_crcf_create_kaiser(M, P, m, As);
    firpfbchr_crcf_print(qa);

    // create multi-signal source generator
    msourcecf gen = msourcecf_create_default();

    // add signals          (gen,  fc,    bw,    gain, {options})
    msourcecf_add_noise(gen,  0.00f, 1.0f, -60);   // wide-band noise
    msourcecf_add_noise(gen, -0.30f, 0.1f, -20);   // narrow-band noise
    msourcecf_add_tone (gen,  0.08f, 0.0f,   0);   // tone
    // modulated data
    msourcecf_add_modem(gen,
       (float)channel_id/(float)M, // center frequency
       0.080f,                     // bandwidth (symbol rate)
       -20,                        // gain
       LIQUID_MODEM_QPSK,          // modulation scheme
       12,                         // filter semi-length
       0.3f);                      // modem parameters

    // create spectral periodogoram
    unsigned int nfft = 2400;
    spgramcf     p0   = spgramcf_create_default(nfft);
    spgramcf     p1   = spgramcf_create_default(nfft);

    // run channelizer
    float complex buf_0[P];
    float complex buf_1[M];
    for (i=0; i<num_blocks; i++) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf_0, P);

        // run analysis filterbank
        firpfbchr_crcf_push   (qa, buf_0);
        firpfbchr_crcf_execute(qa, buf_1);

        // push results through periodograms
        spgramcf_write(p0, buf_0, P);
        spgramcf_push (p1, buf_1[channel_id]);
    }
    spgramcf_print(p0);
    spgramcf_print(p1);

    // compute power spectral density output
    float psd_0[nfft];
    float psd_1[nfft];
    spgramcf_get_psd(p0, psd_0);
    spgramcf_get_psd(p1, psd_1);

    // destroy objects
    firpfbchr_crcf_destroy(qa);
    msourcecf_destroy(gen);
    spgramcf_destroy(p0);
    spgramcf_destroy(p1);

    // verify result
    autotest_psd_s regions[] = {
        {.fmin=-0.5f, .fmax=-0.3f, .pmin=-65, .pmax=-55, .test_lo=1, .test_hi=1},
        {.fmin=-0.2f, .fmax=+0.2f, .pmin=-25, .pmax=-15, .test_lo=1, .test_hi=1},
        {.fmin= 0.3f, .fmax=+0.5f, .pmin=-65, .pmax=-55, .test_lo=1, .test_hi=1},
    };
    char filename[256];
    sprintf(filename,"autotest/logs/firpfbchr_crcf_ch0.m");
    liquid_autotest_validate_spectrum(psd_1, nfft, regions, 3,
        liquid_autotest_verbose ? filename : NULL);
}

void autotest_firpfbchr_crcf_config()
{
#if LIQUID_STRICT_EXIT
    AUTOTEST_WARN("skipping firpfbchr_crcf config test with strict exit enabled\n");
    return;
#endif
#if !LIQUID_SUPPRESS_ERROR_OUTPUT
    fprintf(stderr,"warning: ignore potential errors here; checking for invalid configurations\n");
#endif
    // design prototype filter
    unsigned int h_len = 2*64*12+1;
    float h[2*64*12+1];
    liquid_firdes_kaiser(h_len, 0.1f, 60.0f, 0.0f, h);

    // check invalid function calls
    CONTEND_ISNULL(firpfbchr_crcf_create( 0, 76, 12,    h)) // too few channels
    CONTEND_ISNULL(firpfbchr_crcf_create(64,  0, 12,    h)) // decimation rate too small
    CONTEND_ISNULL(firpfbchr_crcf_create(64, 76,  0,    h)) // filter delay too small
    CONTEND_ISNULL(firpfbchr_crcf_create(64, 76, 12, NULL)) // coefficients pointer set to NULL

    // kaiser
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser( 0, 76, 12, 60.0f)) // too few channels
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser(64,  0, 12, 60.0f)) // decimation rate too small
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser(64, 76,  0, 60.0f)) // filter delay too small
    CONTEND_ISNULL(firpfbchr_crcf_create_kaiser(64, 76, 12, -1.0f)) // stop-band suppression out of range

    //CONTEND_ISNULL(firpfbchr_crcf_copy(NULL))

    // create proper object and test configurations
    firpfbchr_crcf q = firpfbchr_crcf_create_kaiser(64, 76, 12, 60.0f);

    CONTEND_EQUALITY(LIQUID_OK, firpfbchr_crcf_print(q))
    CONTEND_EQUALITY(64, firpfbchr_crcf_get_num_channels(q))
    CONTEND_EQUALITY(76, firpfbchr_crcf_get_decim_rate(q))
    CONTEND_EQUALITY(12, firpfbchr_crcf_get_m(q))

    firpfbchr_crcf_destroy(q);
}


/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

// test spectral periodogram (spgram) objects

#include <stdlib.h>
#include "liquid.internal.h"
#include "liquid.autotest.h"

void testbench_spgramcf_noise(liquid_autotest __q__,
                              unsigned int _nfft,
                              unsigned int _wlen,
                              unsigned int _delay,
                              int          _wtype,
                              float        _noise_floor)
{
    unsigned int num_samples = 2000*_nfft;  // number of samples to generate
    float        nstd        = powf(10.0f,_noise_floor/20.0f); // noise std. dev.
    float        tol         = 0.5f; // error tolerance [dB]
    liquid_log_debug("spgramcf (noise): nfft=%4u, wtype=%15s, noise floor=%6.1f",
            _nfft, liquid_window_str[_wtype][0], _noise_floor);

    // create spectral periodogram
    spgramcf q = NULL;
    if (_wlen==0 || _delay==0 || _wtype==LIQUID_WINDOW_UNKNOWN)
        q = spgramcf_create_default(_nfft);
    else
        q = spgramcf_create(_nfft, _wtype, _wlen, _delay);

    unsigned int i;
    for (i=0; i<num_samples; i++)
        spgramcf_push(q, nstd*( randnf() + _Complex_I*randnf() ) * M_SQRT1_2);

    // verify number of samples processed
    LIQUID_CHECK(spgramcf_get_num_samples(q) ==        num_samples);
    LIQUID_CHECK(spgramcf_get_num_samples_total(q) ==  num_samples);

    // compute power spectral density output
    LIQUID_VLA(float, psd, _nfft);
    spgramcf_get_psd(q, psd);

    // verify result
    for (i=0; i<_nfft; i++)
        LIQUID_CHECK_DELTA(psd[i], _noise_floor, tol)

    // destroy objects
    spgramcf_destroy(q);
}

// test different transform sizes, default parameters
LIQUID_AUTOTEST(spgramcf_noise_440,"","",0.1)  { testbench_spgramcf_noise(__q__,  440, 0, 0, 0, -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_1024,"","",0.1) { testbench_spgramcf_noise(__q__, 1024, 0, 0, 0, -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_1200,"","",0.1) { testbench_spgramcf_noise(__q__, 1200, 0, 0, 0, -80.0); }

// test different transform sizes, specific parameters
LIQUID_AUTOTEST(spgramcf_noise_custom_0,"","",0.1) { testbench_spgramcf_noise(__q__, 400, 400, 100, LIQUID_WINDOW_HAMMING, -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_custom_1,"","",0.1) { testbench_spgramcf_noise(__q__, 512, 200, 120, LIQUID_WINDOW_HAMMING, -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_custom_2,"","",0.1) { testbench_spgramcf_noise(__q__, 640, 100,  10, LIQUID_WINDOW_HAMMING, -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_custom_3,"","",0.1) { testbench_spgramcf_noise(__q__, 960,  83,  17, LIQUID_WINDOW_HAMMING, -80.0); }

// test different window types
LIQUID_AUTOTEST(spgramcf_noise_hamming        ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_HAMMING,        -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_hann           ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_HANN,           -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_blackmanharris ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_BLACKMANHARRIS, -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_blackmanharris7,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_BLACKMANHARRIS7,-80.0); }
LIQUID_AUTOTEST(spgramcf_noise_kaiser         ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_KAISER,         -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_flattop        ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_FLATTOP,        -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_triangular     ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_TRIANGULAR,     -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_rcostaper      ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_RCOSTAPER,      -80.0); }
LIQUID_AUTOTEST(spgramcf_noise_kbd            ,"","",0.1) { testbench_spgramcf_noise(__q__, 800, 0, 0, LIQUID_WINDOW_KBD,            -80.0); }

void testbench_spgramcf_signal(liquid_autotest __q__,
        unsigned int _nfft, int _wtype, float _fc, float _SNRdB)
{
    float bw = 0.25f; // signal bandwidth (relative)
    unsigned int m = 25;
    float beta = 0.2f, n0 = -80.0f, tol = 0.5f;
    liquid_log_debug("spgramcf (signal): nfft=%4u, wtype=%15s, fc=%6.2f Fs, snr=%4.1f dB",
            _nfft, liquid_window_str[_wtype][0], _fc, _SNRdB);

    // create objects
    spgramcf     q     = spgramcf_create(_nfft, _wtype, _nfft/2, _nfft/4);
    symstreamrcf gen   = symstreamrcf_create_linear(LIQUID_FIRFILT_KAISER,bw,m,beta,LIQUID_MODEM_QPSK);
    nco_crcf     mixer = nco_crcf_create(LIQUID_VCO);

    // set parameters
    float nstd = powf(10.0f,n0/20.0f); // noise std. dev.
    symstreamrcf_set_gain(gen, powf(10.0f, (n0 + _SNRdB + 10*log10f(bw))/20.0f));
    nco_crcf_set_frequency(mixer, 2*M_PI*_fc);

    // generate samples and push through spgram object
    unsigned int i, buf_len = 256, num_samples = 0;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);
    while (num_samples < 2000*_nfft) {
        // generate block of samples
        symstreamrcf_write_samples(gen, buf, buf_len);

        // mix to desired frequency and add noise
        nco_crcf_mix_block_up(mixer, buf, buf, buf_len);
        for (i=0; i<buf_len; i++)
            buf[i] += nstd*(randnf()+_Complex_I*randnf())*M_SQRT1_2;

        // run samples through the spgram object
        spgramcf_write(q, buf, buf_len);
        num_samples += buf_len;
    }

    // verify result
    LIQUID_VLA(float, psd, _nfft);
    spgramcf_get_psd(q, psd);
    float sn  = 10*log10f(powf(10,(_SNRdB+n0)/10.0f) + powf(10.0f,n0/10.0f));// signal + noise
    autotest_psd_s regions[] = {
        {.fmin=-0.5f,       .fmax=_fc-0.6f*bw, .pmin=n0-tol, .pmax=n0+tol, .test_lo=1, .test_hi=1},
        {.fmin=_fc-0.4f*bw, .fmax=_fc+0.4f*bw, .pmin=sn-tol, .pmax=sn+tol, .test_lo=1, .test_hi=1},
        {.fmin=_fc+0.6f*bw, .fmax=+0.5f,       .pmin=n0-tol, .pmax=n0+tol, .test_lo=1, .test_hi=1},
    };
    LIQUID_VLA(char, filename, 256);
    sprintf(filename,"autotest/logs/spgramcf_signal_%s_n%u_f%c%.0f_s%c%.0f.m",
        liquid_window_str[_wtype][0], _nfft,
        _fc < 0 ? 'm' : 'p', fabsf(_fc*1000),
        _SNRdB < 0 ? 'm' : 'p', fabsf(_SNRdB*1000));
    liquid_autotest_validate_spectrum(__q__, psd, _nfft, regions, 3, filename);

    // destroy objects
    spgramcf_destroy(q);
    symstreamrcf_destroy(gen);
    nco_crcf_destroy(mixer);
}

LIQUID_AUTOTEST(spgramcf_signal_00,"","",0.1) { testbench_spgramcf_signal(__q__, 800,LIQUID_WINDOW_HAMMING, 0.0f,30.0f); }
LIQUID_AUTOTEST(spgramcf_signal_01,"","",0.1) { testbench_spgramcf_signal(__q__, 800,LIQUID_WINDOW_HAMMING, 0.2f,10.0f); }
LIQUID_AUTOTEST(spgramcf_signal_02,"","",0.1) { testbench_spgramcf_signal(__q__, 800,LIQUID_WINDOW_HANN,    0.2f,10.0f); }
LIQUID_AUTOTEST(spgramcf_signal_03,"","",0.1) { testbench_spgramcf_signal(__q__, 400,LIQUID_WINDOW_KAISER, -0.3f,40.0f); }
LIQUID_AUTOTEST(spgramcf_signal_04,"","",0.1) { testbench_spgramcf_signal(__q__, 640,LIQUID_WINDOW_HAMMING,-0.2f, 0.0f); }
LIQUID_AUTOTEST(spgramcf_signal_05,"","",0.1) { testbench_spgramcf_signal(__q__, 640,LIQUID_WINDOW_HAMMING, 0.1f,-3.0f); }

LIQUID_AUTOTEST(spgramcf_counters,"","",0.1)
{
    // create spectral periodogram with specific parameters
    unsigned int nfft=1200, wlen=400, delay=200;
    int wtype = LIQUID_WINDOW_HAMMING;
    spgramcf q = spgramcf_create(nfft, wtype, wlen, delay);

    // check parameters
    LIQUID_CHECK( spgramcf_get_nfft(q) ==        nfft );
    LIQUID_CHECK( spgramcf_get_window_len(q) ==  wlen );
    LIQUID_CHECK( spgramcf_get_delay(q) ==       delay);

    unsigned int block_len = 1117, num_blocks = 1123;
    unsigned int i, num_samples = block_len * num_blocks;
    unsigned int num_transforms = num_samples / delay;
    for (i=0; i<num_samples; i++)
        spgramcf_push(q, randnf() + _Complex_I*randnf());

    // verify number of samples and transforms processed
    LIQUID_CHECK(spgramcf_get_num_samples(q) ==           num_samples);
    LIQUID_CHECK(spgramcf_get_num_samples_total(q) ==     num_samples);
    LIQUID_CHECK(spgramcf_get_num_transforms(q) ==        num_transforms);
    LIQUID_CHECK(spgramcf_get_num_transforms_total(q) ==  num_transforms);

    // clear object and run in blocks
    spgramcf_clear(q);
    LIQUID_VLA(liquid_float_complex, block, block_len);
    for (i=0; i<block_len; i++)
        block[i] = randnf() + _Complex_I*randnf();
    for (i=0; i<num_blocks; i++)
        spgramcf_write(q, block, block_len);

    // re-verify number of samples and transforms processed
    LIQUID_CHECK(spgramcf_get_num_samples(q) ==           num_samples);
    LIQUID_CHECK(spgramcf_get_num_samples_total(q) ==     num_samples * 2);
    LIQUID_CHECK(spgramcf_get_num_transforms(q) ==        num_transforms);
    LIQUID_CHECK(spgramcf_get_num_transforms_total(q) ==  num_transforms * 2);

    // reset object and ensure counters are zero
    spgramcf_reset(q);
    LIQUID_CHECK(spgramcf_get_num_samples(q) ==           0);
    LIQUID_CHECK(spgramcf_get_num_samples_total(q) ==     0);
    LIQUID_CHECK(spgramcf_get_num_transforms(q) ==        0);
    LIQUID_CHECK(spgramcf_get_num_transforms_total(q) ==  0);

    // destroy object(s)
    spgramcf_destroy(q);
}

LIQUID_AUTOTEST(spgramcf_config,"","",0.1)
{
    _liquid_error_downgrade_enable();
    // check that object returns NULL for invalid configurations
    LIQUID_CHECK(NULL == spgramcf_create(  0, LIQUID_WINDOW_HAMMING,       200, 200)); // nfft too small
    LIQUID_CHECK(NULL == spgramcf_create(  1, LIQUID_WINDOW_HAMMING,       200, 200)); // nfft too small
    LIQUID_CHECK(NULL == spgramcf_create(  2, LIQUID_WINDOW_HAMMING,       200, 200)); // window length too large
    LIQUID_CHECK(NULL == spgramcf_create(400, LIQUID_WINDOW_HAMMING,         0, 200)); // window length too small
    LIQUID_CHECK(NULL == spgramcf_create(400, LIQUID_WINDOW_UNKNOWN,       200, 200)); // invalid window type
    LIQUID_CHECK(NULL == spgramcf_create(400, LIQUID_WINDOW_NUM_FUNCTIONS, 200, 200)); // invalid window type
    LIQUID_CHECK(NULL == spgramcf_create(400, LIQUID_WINDOW_KBD,           201, 200)); // KBD must be even
    LIQUID_CHECK(NULL == spgramcf_create(400, LIQUID_WINDOW_HAMMING,       200,   0)); // delay too small

    // check that object returns NULL for invalid configurations (default)
    LIQUID_CHECK(NULL == spgramcf_create_default(0)); // nfft too small
    LIQUID_CHECK(NULL == spgramcf_create_default(1)); // nfft too small

    // create proper object but test invalid internal configurations
    spgramcf q = spgramcf_create_default(540);

    // check setting bandwidth
    float alpha = 0.0123456f;
    LIQUID_CHECK        ( spgramcf_set_alpha(q, 0.1) == 0 ); // valid
    LIQUID_CHECK_DELTA  ( spgramcf_get_alpha(q), 0.1, 1e-6f);
    LIQUID_CHECK        ( spgramcf_set_alpha(q,-7.0) == -1 ); // invalid
    LIQUID_CHECK_DELTA  ( spgramcf_get_alpha(q), 0.1, 1e-6f);
    LIQUID_CHECK        ( spgramcf_set_alpha(q,alpha) ==  0); // valid
    LIQUID_CHECK_DELTA  ( spgramcf_get_alpha(q), alpha, 1e-6f);
    LIQUID_CHECK        ( spgramcf_get_alpha(q) == alpha);
    LIQUID_CHECK        ( spgramcf_print(q) == LIQUID_OK );

    LIQUID_CHECK(LIQUID_OK != spgramcf_set_rate(q, -10e6))

    spgramcf_destroy(q);
    _liquid_error_downgrade_disable();
}

LIQUID_AUTOTEST(spgramcf_standalone,"","",0.1)
{
    unsigned int nfft        = 1200;
    unsigned int num_samples = 20*nfft; // number of samples to generate
    float        n0          = -20.0f;  // noise floor
    float        tol         = 3.0f;    // tolerance [dB]
    float        nstd        = powf(10.0f,n0/20.0f); // noise std. dev.

    liquid_float_complex * buf = (liquid_float_complex*)malloc(num_samples*sizeof(liquid_float_complex));
    unsigned int i;
    for (i=0; i<num_samples; i++)
        buf[i] = 0.1f + nstd*(randnf()+_Complex_I*randnf())*M_SQRT1_2;

    LIQUID_VLA(float, psd, nfft);
    spgramcf_estimate_psd(nfft, buf, num_samples, psd);

    // check mask
    autotest_psd_s regions[] = {
        {.fmin=-0.500, .fmax=-0.050, .pmin=n0-tol, .pmax=n0+tol, .test_lo=1, .test_hi=1},
        {.fmin=-0.001, .fmax=+0.001, .pmin=   2.0, .pmax=   8.0, .test_lo=1, .test_hi=1},
        {.fmin=+0.050, .fmax=+0.500, .pmin=n0-tol, .pmax=n0+tol, .test_lo=1, .test_hi=1},
    };
    liquid_autotest_validate_spectrum(__q__, psd, nfft, regions, 3,
        "autotest/logs/spgramcf_standalone.m");

    // free memory
    free(buf);
}

// check spectral periodogram operation where the input size is much shorter
// than the transform size
LIQUID_AUTOTEST(spgramcf_short,"","",0.1)
{
    unsigned int nfft        = 1200;    // transform size
    unsigned int num_samples =  200;    // number of samples to generate
    float        noise_floor = -20.0f;
    float        nstd        = powf(10.0f,noise_floor/20.0f); // noise std. dev.

    liquid_float_complex * buf = (liquid_float_complex*)malloc(num_samples*sizeof(liquid_float_complex));
    unsigned int i;
    for (i=0; i<num_samples; i++)
        buf[i] = 1.0f + nstd*(randnf()+_Complex_I*randnf())*M_SQRT1_2;

    LIQUID_VLA(float, psd, nfft);
    spgramcf_estimate_psd(nfft, buf, num_samples, psd);

    // use a very loose upper mask as we have only computed a few hundred samples
    for (i=0; i<nfft; i++) {
        float f       = (float)i / (float)nfft - 0.5f;
        float mask_hi = fabsf(f) < 0.2f ? 15.0f - 30*fabsf(f)/0.2f : -15.0f;
        liquid_log_debug("%6u : f=%6.3f, %8.2f < %8.2f", i, f, psd[i], mask_hi);
        LIQUID_CHECK( psd[i] < mask_hi );
    }
    // consider lower mask only for DC term
    float mask_lo = 0.0f;
    unsigned int nfft_2 = nfft/2;
    liquid_log_debug("DC : f=%6.3f, %8.2f > %8.2f", 0.0f, psd[nfft_2], mask_lo);
    LIQUID_CHECK( psd[nfft_2] > mask_lo );

    // free memory
    free(buf);
}

// check copy method
LIQUID_AUTOTEST(spgramcf_copy,"","",0.1)
{
    unsigned int nfft        = 1200;    // transform size
    unsigned int num_samples = 9600;    // number of samples to generate
    float        nstd        =  0.1f;   // noise standard deviation

    // create object with some odd properties
    spgramcf q0 = spgramcf_create(nfft, LIQUID_WINDOW_KAISER, 960, 373);

    // generate a bunch of random noise samples
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        liquid_float_complex v = 0.1f + nstd * (randnf() + _Complex_I*randnf());
        spgramcf_push(q0, v);
    }

    // copy object and push same samples through both
    spgramcf q1 = spgramcf_copy(q0);
    for (i=0; i<num_samples; i++) {
        liquid_float_complex v = 0.1f + nstd * (randnf() + _Complex_I*randnf());
        spgramcf_push(q0, v);
        spgramcf_push(q1, v);
    }

    // get spectrum and compare outputs
    LIQUID_VLA(float, psd_0, nfft);
    LIQUID_VLA(float, psd_1, nfft);
    spgramcf_get_psd(q0, psd_0);
    spgramcf_get_psd(q1, psd_1);
    LIQUID_CHECK_ARRAY(psd_0, psd_1, nfft*sizeof(float));

    // check parameters
    LIQUID_CHECK(spgramcf_get_nfft                (q0) == spgramcf_get_nfft                (q1));
    LIQUID_CHECK(spgramcf_get_window_len          (q0) == spgramcf_get_window_len          (q1));
    LIQUID_CHECK(spgramcf_get_delay               (q0) == spgramcf_get_delay               (q1));
    LIQUID_CHECK(spgramcf_get_wtype               (q0) == spgramcf_get_wtype               (q1));
    LIQUID_CHECK(spgramcf_get_num_samples         (q0) == spgramcf_get_num_samples         (q1));
    LIQUID_CHECK(spgramcf_get_num_samples_total   (q0) == spgramcf_get_num_samples_total   (q1));
    LIQUID_CHECK(spgramcf_get_num_transforms      (q0) == spgramcf_get_num_transforms      (q1));
    LIQUID_CHECK(spgramcf_get_num_transforms_total(q0) == spgramcf_get_num_transforms_total(q1));

    // destroy objects
    spgramcf_destroy(q0);
    spgramcf_destroy(q1);
}

// check spectral periodogram behavior on null input (zero samples)
LIQUID_AUTOTEST(spgramcf_null,"","",0.1)
{
    unsigned int nfft = 1200;   // transform size
    LIQUID_VLA(float, psd, nfft);
    spgramcf_estimate_psd(nfft, NULL, 0, psd);

    // value should be exactly minimum
    float psd_val = 10*log10f(LIQUID_SPGRAM_PSD_MIN);
    unsigned int i;
    for (i=0; i<nfft; i++)
        LIQUID_CHECK(psd[i] == psd_val);
}

// test file export
LIQUID_AUTOTEST(spgram_gnuplot,"","",0.1)
{
    // create default object
    spgramcf q = spgramcf_create_default(540);
    unsigned int i;
    for (i=0; i<20000; i++)
        spgramcf_push(q, randnf() + _Complex_I*randnf());

    // export once before setting values
    LIQUID_CHECK(LIQUID_OK == spgramcf_export_gnuplot(q,"autotest/logs/spgram.gnu"))

    // set values and export again
    LIQUID_CHECK(LIQUID_OK == spgramcf_set_freq(q, 100e6))
    LIQUID_CHECK(LIQUID_OK == spgramcf_set_rate(q,  20e6))
    LIQUID_CHECK(LIQUID_OK == spgramcf_export_gnuplot(q,"autotest/logs/spgram.gnu"))

    spgramcf_destroy(q);
}


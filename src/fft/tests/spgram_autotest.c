/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

void testbench_spgramcf_noise(unsigned int _nfft,
                              float        _noise_floor)
{
    unsigned int num_samples = 2e6;  // number of samples to generate
    float        nstd        = powf(10.0f,_noise_floor/20.0f); // noise std. dev.
    float        tol         = 0.5f; // error tolerance [dB]

    // create spectral periodogram
    spgramcf q = spgramcf_create_default(_nfft);

    unsigned int i;
    for (i=0; i<num_samples; i++)
        spgramcf_push(q, nstd*( randnf() + _Complex_I*randnf() ) * M_SQRT1_2);

    // compute power spectral density output
    float psd[_nfft];
    spgramcf_get_psd(q, psd);

    // verify result
    for (i=0; i<_nfft; i++)
        CONTEND_DELTA(psd[i], _noise_floor, tol)

    // destroy objects
    spgramcf_destroy(q);
}

void autotest_spgramcf_noise_1024() { testbench_spgramcf_noise(1024, -80.0); }


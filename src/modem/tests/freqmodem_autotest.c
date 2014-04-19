/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "autotest/autotest.h"
#include "liquid.h"

#define DEBUG_FREQMODEM_AUTOTEST 0

// Help function to keep code base small
//  _kf     :   modulation factor
void freqmodem_test(float _kf)
{
    // options
    unsigned int num_samples = 1024;
    //float tol = 1e-2f;

    unsigned int i;

    // create mod/demod objects
    freqmod mod = freqmod_create(_kf);  // modulator
    freqdem dem = freqdem_create(_kf);  // demodulator

    // allocate arrays
    float         m[num_samples];       // message signal
    float complex r[num_samples];       // received signal (complex baseband)
    float         y[num_samples];       // demodulator output

    // generate message signal (single-frequency sine)
    for (i=0; i<num_samples; i++)
        m[i] = 0.7f*cosf(2*M_PI*0.013f*i + 0.0f);

    // modulate/demodulate signal
    for (i=0; i<num_samples; i++) {
        // modulate
        freqmod_modulate(mod, m[i], &r[i]);

        // demodulate
        freqdem_demodulate(dem, r[i], &y[i]);
    }

    // delete modem objects
    freqmod_destroy(mod);
    freqdem_destroy(dem);

    // TODO: run actual comparison: remove delay and compute difference
#if 0
    // compute power spectral densities and compare
    float complex mcf[num_samples];
    float complex ycf[num_samples];
    float complex M[num_samples];
    float complex Y[num_samples];
    for (i=0; i<num_samples; i++) {
        mcf[i] = m[i] * hamming(i,num_samples);
        ycf[i] = y[i] * hamming(i,num_samples);
    }
    fft_run(num_samples, mcf, M, LIQUID_FFT_FORWARD, 0);
    fft_run(num_samples, ycf, Y, LIQUID_FFT_FORWARD, 0);

    // run test: compare spectral magnitude
    for (i=0; i<num_samples; i++)
        CONTEND_DELTA( cabsf(Y[i]), cabsf(M[i]), tol );
#endif
}

// AUTOTESTS: generic PSK
void autotest_freqmodem_kf_0_02() { freqmodem_test(0.02f); }
void autotest_freqmodem_kf_0_04() { freqmodem_test(0.04f); }
void autotest_freqmodem_kf_0_08() { freqmodem_test(0.08f); }


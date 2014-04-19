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

// Help function to keep code base small
//  _kf     :   modulation factor
void freqmodem_test(float _kf)
{
    // options
    unsigned int num_samples = 1024;
    float tol = 5e-2f;

    unsigned int i;

    // create mod/demod objects
    freqmod mod = freqmod_create(_kf);  // modulator
    freqdem dem = freqdem_create(_kf);  // demodulator

    // allocate arrays
    float         m[num_samples];       // message signal
    float complex r[num_samples];       // received signal (complex baseband)
    float         y[num_samples];       // demodulator output

    // generate message signal (sum of sines)
    for (i=0; i<num_samples; i++) {
        m[i] = 0.3f*cosf(2*M_PI*0.013f*i + 0.0f) +
               0.2f*cosf(2*M_PI*0.021f*i + 0.4f) +
               0.4f*cosf(2*M_PI*0.037f*i + 1.7f);
    }

    // modulate signal
    freqmod_modulate_block(mod, m, num_samples, r);

    // demodulate signal
    freqdem_demodulate_block(dem, r, num_samples, y);

    // delete modem objects
    freqmod_destroy(mod);
    freqdem_destroy(dem);

    // compare demodulated signal to original, skipping first sample
    for (i=1; i<num_samples; i++)
        CONTEND_DELTA( y[i], m[i], tol );
}

// AUTOTESTS: generic PSK
void autotest_freqmodem_kf_0_02() { freqmodem_test(0.02f); }
void autotest_freqmodem_kf_0_04() { freqmodem_test(0.04f); }
void autotest_freqmodem_kf_0_08() { freqmodem_test(0.08f); }


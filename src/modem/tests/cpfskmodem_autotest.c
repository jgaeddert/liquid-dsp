/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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

// Help function to keep code base small
void cpfskmodem_test_mod_demod(unsigned int _bps,
                               float        _h,
                               unsigned int _k,
                               unsigned int _m,
                               float        _beta,
                               int          _filter_type)
{
    // create modulator/demodulator pair
    cpfskmod mod = cpfskmod_create(_bps, _h, _k, _m, _beta, _filter_type);
    cpfskdem dem = cpfskdem_create(_bps, _h, _k, _m, _beta, _filter_type);

    // derived values
    unsigned int delay = cpfskmod_get_delay(mod) + cpfskdem_get_delay(dem);
    //unsigned int  M = 1 << _m;      // constellation size
    
    unsigned int  num_symbols = 80 + delay; // number of symbols to test

    msequence ms = msequence_create_default(7);

    float complex buf[_k];      // sample buffer
    unsigned int  sym_in [num_symbols]; // symbol buffer
    unsigned int  sym_out[num_symbols]; // symbol buffer

    // modulate, demodulate, count errors
    unsigned int i;
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        sym_in[i] = msequence_generate_symbol(ms, _bps);

        // modulate
        cpfskmod_modulate(mod, sym_in[i], buf);

        // demodulate
        sym_out[i] = cpfskdem_demodulate(dem, buf);
    }

    // count errors
    for (i=0; i<num_symbols; i++) {
        if (i >= delay) {
            // print results
            if (liquid_autotest_verbose) {
                printf("  %3u : input = %2u, output = %2u %s\n",
                        i, sym_in[i-delay], sym_out[i],
                        (sym_in[i-delay] == sym_out[i]) ? "" : "*");
            }

            // check result
            CONTEND_EQUALITY(sym_in[i-delay], sym_out[i]);
        }
    }

    // clean it up
    msequence_destroy(ms);
    cpfskmod_destroy(mod);
    cpfskdem_destroy(dem);
}

//
// AUTOTESTS: check different modulation indices
//

// square pulse shape
void autotest_cpfskmodem_bps1_h0p5000_k4_m3_square()    { cpfskmodem_test_mod_demod( 1, 0.5000f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }
void autotest_cpfskmodem_bps1_h0p0250_k4_m3_square()    { cpfskmodem_test_mod_demod( 1, 0.2500f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }
void autotest_cpfskmodem_bps1_h0p1250_k4_m3_square()    { cpfskmodem_test_mod_demod( 1, 0.1250f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }
void autotest_cpfskmodem_bps1_h0p0625_k4_m3_square()    { cpfskmodem_test_mod_demod( 1, 0.0625f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }

// raised-cosine pulse shape (full)
void autotest_cpfskmodem_bps1_h0p5000_k4_m3_rcosfull()  { cpfskmodem_test_mod_demod( 1, 0.5000f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_FULL ); }
void autotest_cpfskmodem_bps1_h0p0250_k4_m3_rcosfull()  { cpfskmodem_test_mod_demod( 1, 0.2500f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_FULL ); }
void autotest_cpfskmodem_bps1_h0p1250_k4_m3_rcosfull()  { cpfskmodem_test_mod_demod( 1, 0.1250f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_FULL ); }
void autotest_cpfskmodem_bps1_h0p0625_k4_m3_rcosfull()  { cpfskmodem_test_mod_demod( 1, 0.0625f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_FULL ); }

// raised-cosine pulse shape (partial)
void autotest_cpfskmodem_bps1_h0p5000_k4_m3_rcospart()  { cpfskmodem_test_mod_demod( 1, 0.5000f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_PARTIAL ); }
void autotest_cpfskmodem_bps1_h0p0250_k4_m3_rcospart()  { cpfskmodem_test_mod_demod( 1, 0.2500f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_PARTIAL ); }
void autotest_cpfskmodem_bps1_h0p1250_k4_m3_rcospart()  { cpfskmodem_test_mod_demod( 1, 0.1250f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_PARTIAL ); }
void autotest_cpfskmodem_bps1_h0p0625_k4_m3_rcospart()  { cpfskmodem_test_mod_demod( 1, 0.0625f, 4, 3, 0.25f, LIQUID_CPFSK_RCOS_PARTIAL ); }

// Gauss minimum-shift keying
void autotest_cpfskmodem_bps1_h0p5000_k4_m3_gmsk()      { cpfskmodem_test_mod_demod( 1, 0.5000f, 4, 3, 0.25f, LIQUID_CPFSK_GMSK ); }
void autotest_cpfskmodem_bps1_h0p0250_k4_m3_gmsk()      { cpfskmodem_test_mod_demod( 1, 0.2500f, 4, 3, 0.25f, LIQUID_CPFSK_GMSK ); }
void autotest_cpfskmodem_bps1_h0p1250_k4_m3_gmsk()      { cpfskmodem_test_mod_demod( 1, 0.1250f, 4, 3, 0.25f, LIQUID_CPFSK_GMSK ); }
void autotest_cpfskmodem_bps1_h0p0625_k4_m3_gmsk()      { cpfskmodem_test_mod_demod( 1, 0.0625f, 4, 3, 0.25f, LIQUID_CPFSK_GMSK ); }

//
// AUTOTESTS: check different bits per symbol
//

// square pulse shape
void autotest_cpfskmodem_bps2_h0p0250_k4_m3_square()    { cpfskmodem_test_mod_demod( 2, 0.2500f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }
void autotest_cpfskmodem_bps3_h0p1250_k4_m3_square()    { cpfskmodem_test_mod_demod( 3, 0.1250f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }
void autotest_cpfskmodem_bps4_h0p0625_k4_m3_square()    { cpfskmodem_test_mod_demod( 4, 0.0625f, 4, 3, 0.25f, LIQUID_CPFSK_SQUARE ); }

// test spectral response
void autotest_cpfskmodem_spectrum()
{
    // create modulator
    unsigned int bps    = 1;
    float        h      = 0.5f;
    unsigned int k      = 4;
    unsigned int m      = 3;
    float        beta   = 0.35f;
    int          type   = LIQUID_CPFSK_RCOS_PARTIAL;
    cpfskmod mod = cpfskmod_create(bps, h, k, m, beta, type);

    // spectral periodogram options
    unsigned int nfft        =   2400;  // spectral periodogram FFT size
    unsigned int num_symbols = 192000;  // number of symbols to generate
    float complex buf[k];
    unsigned int i;

    // modulate many, many symbols
    for (i=0; i<(1U<<24U); i++)
        cpfskmod_modulate(mod, 0, buf);

    // modulate several symbols and run result through spectral estimate
    spgramcf periodogram = spgramcf_create_default(nfft);
    for (i=0; i<num_symbols; i++) {
        cpfskmod_modulate(mod, rand() & ((1<<bps)-1), buf);
        spgramcf_write(periodogram, buf, k);
    }

    // compute power spectral density output
    float psd[nfft];
    spgramcf_get_psd(periodogram, psd);

    // destroy objects
    cpfskmod_destroy(mod);
    spgramcf_destroy(periodogram);

    // verify spectrum
    autotest_psd_s regions[] = {
      {.fmin=-0.50, .fmax=-0.35, .pmin= 0.0, .pmax=-40.0, .test_lo=0, .test_hi=1},
      {.fmin=-0.35, .fmax=-0.20, .pmin= 0.0, .pmax=-20.0, .test_lo=0, .test_hi=1},
      {.fmin=-0.10, .fmax= 0.10, .pmin= 0.0, .pmax= 10.0, .test_lo=1, .test_hi=1},
      {.fmin= 0.20, .fmax= 0.35, .pmin= 0.0, .pmax=-20.0, .test_lo=0, .test_hi=1},
      {.fmin= 0.35, .fmax= 0.50, .pmin= 0.0, .pmax=-40.0, .test_lo=0, .test_hi=1},
    };
    char filename[256];
    //sprintf(filename,"autotest/logs/cpfskmodem_psd_b%u_h%.3u_k%u_m%u_b%.3u_t%u_autotest.m",
    //        bps, (int)(h*100), k, m, (int)(beta*100), type);
    sprintf(filename,"autotest/logs/cpfskmodem_psd_autotest.m");
    liquid_autotest_validate_spectrum(psd, nfft, regions, 5,
        liquid_autotest_verbose ? filename : NULL);
}


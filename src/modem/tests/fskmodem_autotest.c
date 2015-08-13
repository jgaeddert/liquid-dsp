/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
void fskmodem_test_mod_demod(unsigned int _m,
                             unsigned int _k,
                             float        _bandwidth)
{
    // create modulator/demodulator pair
    fskmod mod = fskmod_create(_m,_k,_bandwidth);
    fskdem dem = fskdem_create(_m,_k,_bandwidth);

    unsigned int M = 1 << _m;   // constellation size
    float complex buf[_k];      // transmit buffer
    
    // modulate, demodulate, count errors
    unsigned int i;
    for (i=0; i<M; i++) {
        // generate random symbol
        unsigned int sym_in = i;

        // modulate
        fskmod_modulate(mod, sym_in, buf);

        // demodulate
        unsigned int sym_out = fskdem_demodulate(dem, buf);

        // count errors
        CONTEND_EQUALITY(sym_in, sym_out);
    }

    // clean it up
    fskmod_destroy(mod);
    fskdem_destroy(dem);
}

// AUTOTESTS: basic properties: M=2^m, k = 2*M, bandwidth = 0.25
void autotest_fskmodem_norm_M2()    { fskmodem_test_mod_demod( 1,    4, 0.25f    ); }
void autotest_fskmodem_norm_M4()    { fskmodem_test_mod_demod( 2,    8, 0.25f    ); }
void autotest_fskmodem_norm_M8()    { fskmodem_test_mod_demod( 3,   16, 0.25f    ); }
void autotest_fskmodem_norm_M16()   { fskmodem_test_mod_demod( 4,   32, 0.25f    ); }
void autotest_fskmodem_norm_M32()   { fskmodem_test_mod_demod( 5,   64, 0.25f    ); }
void autotest_fskmodem_norm_M64()   { fskmodem_test_mod_demod( 6,  128, 0.25f    ); }
void autotest_fskmodem_norm_M128()  { fskmodem_test_mod_demod( 7,  256, 0.25f    ); }
void autotest_fskmodem_norm_M256()  { fskmodem_test_mod_demod( 8,  512, 0.25f    ); }
void autotest_fskmodem_norm_M512()  { fskmodem_test_mod_demod( 9, 1024, 0.25f    ); }
void autotest_fskmodem_norm_M1024() { fskmodem_test_mod_demod(10, 2048, 0.25f    ); }

// AUTOTESTS: obscure properties: M=2^m, k not relative to M, bandwidth basically irrational
void autotest_fskmodem_misc_M2()    { fskmodem_test_mod_demod( 1,    5, 0.3721451); }
void autotest_fskmodem_misc_M4()    { fskmodem_test_mod_demod( 2,   10, 0.3721451); }
void autotest_fskmodem_misc_M8()    { fskmodem_test_mod_demod( 3,   20, 0.3721451); }
void autotest_fskmodem_misc_M16()   { fskmodem_test_mod_demod( 4,   30, 0.3721451); }
void autotest_fskmodem_misc_M32()   { fskmodem_test_mod_demod( 5,   60, 0.3721451); }
void autotest_fskmodem_misc_M64()   { fskmodem_test_mod_demod( 6,  100, 0.3721451); }
void autotest_fskmodem_misc_M128()  { fskmodem_test_mod_demod( 7,  200, 0.3721451); }
void autotest_fskmodem_misc_M256()  { fskmodem_test_mod_demod( 8,  500, 0.3721451); }
void autotest_fskmodem_misc_M512()  { fskmodem_test_mod_demod( 9, 1000, 0.3721451); }
void autotest_fskmodem_misc_M1024() { fskmodem_test_mod_demod(10, 2000, 0.3721451); }


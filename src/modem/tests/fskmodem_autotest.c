/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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


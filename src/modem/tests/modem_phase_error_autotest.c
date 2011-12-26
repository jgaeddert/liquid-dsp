/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#include <stdio.h>

#include "autotest/autotest.h"
#include "liquid.h"

// Helper function to keep code base small
void modem_test_phase_error(modulation_scheme _ms)
{
    // generate mod/demod
    modem mod   = modem_create(_ms);
    modem demod = modem_create(_ms);

    // run the test
    unsigned int i, s, M = 1 << modem_get_bps(mod);
    float complex x;
    float complex x_hat;    // rotated symbol
    float phase_error;
    float phi = 0.01f;

    for (i=0; i<M; i++) {
        // reset modem objects
        modem_reset(mod);
        modem_reset(demod);

        // modulate symbol
        modem_modulate(mod, i, &x);

        // ignore rare condition where modulated symbol is (0,0)
        // (e.g. APSK-8)
        if (cabsf(x) < 1e-3f) continue;

        // add phase offsets
        x_hat = x * cexpf( phi*_Complex_I);

        // demod positive phase signal, and ensure demodulator
        // maps to appropriate symbol
        modem_demodulate(demod, x_hat, &s);
        if (s != i)
            AUTOTEST_WARN("modem_test_phase_error(), output symbol does not match");

        phase_error = modem_get_demodulator_phase_error(demod);
        CONTEND_EXPRESSION(phase_error > 0.0f);
    }

    // repeat with negative phase error
    for (i=0; i<M; i++) {
        // reset modem objects
        modem_reset(mod);
        modem_reset(demod);

        // modulate symbol
        modem_modulate(mod, i, &x);

        // ignore rare condition where modulated symbol is (0,0)
        // (e.g. APSK-8)
        if (cabsf(x) < 1e-3f) continue;

        // add phase offsets
        x_hat = x * cexpf(-phi*_Complex_I);

        // demod positive phase signal, and ensure demodulator
        // maps to appropriate symbol
        modem_demodulate(demod, x_hat, &s);
        if (s != i)
            AUTOTEST_WARN("modem_test_phase_error(), output symbol does not match");

        phase_error = modem_get_demodulator_phase_error(demod);
        CONTEND_EXPRESSION(phase_error < 0.0f);
    }

    // clean up allocated objects up
    modem_destroy(mod);
    modem_destroy(demod);
}

//
// AUTOTESTS: Specific modems
//
void autotest_phase_error_bpsk()  {   modem_test_phase_error(LIQUID_MODEM_BPSK);    }
void autotest_phase_error_qpsk()  {   modem_test_phase_error(LIQUID_MODEM_QPSK);    }
void autotest_phase_error_ook()   {   modem_test_phase_error(LIQUID_MODEM_OOK);     }
void autotest_phase_error_sqam32(){   modem_test_phase_error(LIQUID_MODEM_SQAM32);  }
void autotest_phase_error_sqam128(){  modem_test_phase_error(LIQUID_MODEM_SQAM128); }

//
// AUTOTESTS: generic ASK
//
void autotest_phase_error_ask2()  {   modem_test_phase_error(LIQUID_MODEM_ASK2);   }
void autotest_phase_error_ask4()  {   modem_test_phase_error(LIQUID_MODEM_ASK4);   }
void autotest_phase_error_ask8()  {   modem_test_phase_error(LIQUID_MODEM_ASK8);   }
void autotest_phase_error_ask16() {   modem_test_phase_error(LIQUID_MODEM_ASK16);  }

//
// AUTOTESTS: generic PSK
//
void autotest_phase_error_psk2()  {   modem_test_phase_error(LIQUID_MODEM_PSK2);   }
void autotest_phase_error_psk4()  {   modem_test_phase_error(LIQUID_MODEM_PSK4);   }
void autotest_phase_error_psk8()  {   modem_test_phase_error(LIQUID_MODEM_PSK8);   }
void autotest_phase_error_psk16() {   modem_test_phase_error(LIQUID_MODEM_PSK16);  }
void autotest_phase_error_psk32() {   modem_test_phase_error(LIQUID_MODEM_PSK32);  }
void autotest_phase_error_psk64() {   modem_test_phase_error(LIQUID_MODEM_PSK64);  }

//
// AUTOTESTS: generic differential PSK
//
void autotest_phase_error_dpsk2()  {  modem_test_phase_error(LIQUID_MODEM_DPSK2);  }
void autotest_phase_error_dpsk4()  {  modem_test_phase_error(LIQUID_MODEM_DPSK4);  }
void autotest_phase_error_dpsk8()  {  modem_test_phase_error(LIQUID_MODEM_DPSK8);  }
void autotest_phase_error_dpsk16() {  modem_test_phase_error(LIQUID_MODEM_DPSK16); }
void autotest_phase_error_dpsk32() {  modem_test_phase_error(LIQUID_MODEM_DPSK32); }
void autotest_phase_error_dpsk64() {  modem_test_phase_error(LIQUID_MODEM_DPSK64); }

//
// AUTOTESTS: generic QAM
//
void autotest_phase_error_qam4()   {  modem_test_phase_error(LIQUID_MODEM_QAM4);   }
void autotest_phase_error_qam8()   {  modem_test_phase_error(LIQUID_MODEM_QAM8);   }
void autotest_phase_error_qam16()  {  modem_test_phase_error(LIQUID_MODEM_QAM16);  }
void autotest_phase_error_qam32()  {  modem_test_phase_error(LIQUID_MODEM_QAM32);  }
void autotest_phase_error_qam64()  {  modem_test_phase_error(LIQUID_MODEM_QAM64);  }
void autotest_phase_error_qam128() {  modem_test_phase_error(LIQUID_MODEM_QAM128); }
void autotest_phase_error_qam256() {  modem_test_phase_error(LIQUID_MODEM_QAM256); }

//
// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
//
void autotest_phase_error_apsk4()  {  modem_test_phase_error(LIQUID_MODEM_APSK4);   }
void autotest_phase_error_apsk8()  {  modem_test_phase_error(LIQUID_MODEM_APSK8);   }
void autotest_phase_error_apsk16() {  modem_test_phase_error(LIQUID_MODEM_APSK16);  }
void autotest_phase_error_apsk32() {  modem_test_phase_error(LIQUID_MODEM_APSK32);  }
void autotest_phase_error_apsk64() {  modem_test_phase_error(LIQUID_MODEM_APSK64);  }
void autotest_phase_error_apsk128(){  modem_test_phase_error(LIQUID_MODEM_APSK128); }
void autotest_phase_error_apsk256(){  modem_test_phase_error(LIQUID_MODEM_APSK256); }

//
// AUTOTESTS: arbitrary modems
//
void autotest_phase_error_arbV29()    { modem_test_phase_error(LIQUID_MODEM_V29);       }
void autotest_phase_error_arb16opt()  { modem_test_phase_error(LIQUID_MODEM_ARB16OPT);  }
void autotest_phase_error_arb32opt()  { modem_test_phase_error(LIQUID_MODEM_ARB32OPT);  }
void autotest_phase_error_arb64opt()  { modem_test_phase_error(LIQUID_MODEM_ARB64OPT);  }
void autotest_phase_error_arb128opt() { modem_test_phase_error(LIQUID_MODEM_ARB128OPT); }
void autotest_phase_error_arb256opt() { modem_test_phase_error(LIQUID_MODEM_ARB256OPT); }
void autotest_phase_error_arb64vt()   { modem_test_phase_error(LIQUID_MODEM_ARB64VT);   }


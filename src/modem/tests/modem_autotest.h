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

#include "autotest/autotest.h"
#include "liquid.h"

// Help function to keep code base small
void modem_test_mod_demod(modulation_scheme _ms, unsigned int _bps)
{
    // generate mod/demod
    modem mod = modem_create(_ms, _bps);
    modem demod = modem_create(_ms, _bps);

    // run the test
    unsigned int i, s, M=1<<_bps;
    float complex x;
    float e = 0.0f;
    for (i=0; i<M; i++) {
        modem_modulate(mod, i, &x);
        modem_demodulate(demod, x, &s);
        CONTEND_EQUALITY(s, i);

        CONTEND_DELTA( modem_get_demodulator_phase_error(demod), 0.0f, 1e-3f);
        
        CONTEND_DELTA( modem_get_demodulator_evm(demod), 0.0f, 1e-3f);

        e += crealf(x*conjf(x));
    }
    e = sqrtf(e / (float)M);

    CONTEND_DELTA(e,1.0f,1e-3f);

    // clean it up
    modem_destroy(mod);
    modem_destroy(demod);
}

//
// AUTOTESTS: Specific modems
//
void autotest_mod_demod_bpsk()  {   modem_test_mod_demod(LIQUID_MODEM_BPSK, 1);  }
void autotest_mod_demod_qpsk()  {   modem_test_mod_demod(LIQUID_MODEM_QPSK, 2);  }
void autotest_mod_demod_ook()   {   modem_test_mod_demod(LIQUID_MODEM_OOK,  1);  }
void autotest_mod_demod_sqam32(){   modem_test_mod_demod(LIQUID_MODEM_SQAM32,5);  }
void autotest_mod_demod_sqam128(){  modem_test_mod_demod(LIQUID_MODEM_SQAM128,7);  }

//
// AUTOTESTS: generic ASK
//
void autotest_mod_demod_ask2()  {   modem_test_mod_demod(LIQUID_MODEM_ASK, 1);   }
void autotest_mod_demod_ask4()  {   modem_test_mod_demod(LIQUID_MODEM_ASK, 2);   }
void autotest_mod_demod_ask8()  {   modem_test_mod_demod(LIQUID_MODEM_ASK, 3);   }
void autotest_mod_demod_ask16() {   modem_test_mod_demod(LIQUID_MODEM_ASK, 4);   }

//
// AUTOTESTS: generic PSK
//
void autotest_mod_demod_psk2()  {   modem_test_mod_demod(LIQUID_MODEM_PSK, 1);   }
void autotest_mod_demod_psk4()  {   modem_test_mod_demod(LIQUID_MODEM_PSK, 2);   }
void autotest_mod_demod_psk8()  {   modem_test_mod_demod(LIQUID_MODEM_PSK, 3);   }
void autotest_mod_demod_psk16() {   modem_test_mod_demod(LIQUID_MODEM_PSK, 4);   }
void autotest_mod_demod_psk32() {   modem_test_mod_demod(LIQUID_MODEM_PSK, 5);   }
void autotest_mod_demod_psk64() {   modem_test_mod_demod(LIQUID_MODEM_PSK, 6);   }

//
// AUTOTESTS: generic differential PSK
//
void autotest_mod_demod_dpsk2()  {  modem_test_mod_demod(LIQUID_MODEM_DPSK, 1);  }
void autotest_mod_demod_dpsk4()  {  modem_test_mod_demod(LIQUID_MODEM_DPSK, 2);  }
void autotest_mod_demod_dpsk8()  {  modem_test_mod_demod(LIQUID_MODEM_DPSK, 3);  }
void autotest_mod_demod_dpsk16() {  modem_test_mod_demod(LIQUID_MODEM_DPSK, 4);  }
void autotest_mod_demod_dpsk32() {  modem_test_mod_demod(LIQUID_MODEM_DPSK, 5);  }
void autotest_mod_demod_dpsk64() {  modem_test_mod_demod(LIQUID_MODEM_DPSK, 6);  }

//
// AUTOTESTS: generic QAM
//
void autotest_mod_demod_qam4()   {  modem_test_mod_demod(LIQUID_MODEM_QAM, 2);   }
void autotest_mod_demod_qam8()   {  modem_test_mod_demod(LIQUID_MODEM_QAM, 3);   }
void autotest_mod_demod_qam16()  {  modem_test_mod_demod(LIQUID_MODEM_QAM, 4);   }
void autotest_mod_demod_qam32()  {  modem_test_mod_demod(LIQUID_MODEM_QAM, 5);   }
void autotest_mod_demod_qam64()  {  modem_test_mod_demod(LIQUID_MODEM_QAM, 6);   }
void autotest_mod_demod_qam128() {  modem_test_mod_demod(LIQUID_MODEM_QAM, 7);   }
void autotest_mod_demod_qam256() {  modem_test_mod_demod(LIQUID_MODEM_QAM, 8);   }

//
// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
//
void autotest_mod_demod_apsk4()  {  modem_test_mod_demod(LIQUID_MODEM_APSK,2);   }
void autotest_mod_demod_apsk8()  {  modem_test_mod_demod(LIQUID_MODEM_APSK,3);   }
void autotest_mod_demod_apsk16() {  modem_test_mod_demod(LIQUID_MODEM_APSK,4);   }
void autotest_mod_demod_apsk32() {  modem_test_mod_demod(LIQUID_MODEM_APSK,5);   }
void autotest_mod_demod_apsk64() {  modem_test_mod_demod(LIQUID_MODEM_APSK,6);   }
void autotest_mod_demod_apsk128(){  modem_test_mod_demod(LIQUID_MODEM_APSK,7);   }
void autotest_mod_demod_apsk256(){  modem_test_mod_demod(LIQUID_MODEM_APSK,8);   }

//
// AUTOTESTS: arbitrary modems
//
void autotest_mod_demod_arbV29()    { modem_test_mod_demod(LIQUID_MODEM_V29,4);         }
void autotest_mod_demod_arb16opt()  { modem_test_mod_demod(LIQUID_MODEM_ARB16OPT,4);    }
void autotest_mod_demod_arb32opt()  { modem_test_mod_demod(LIQUID_MODEM_ARB32OPT,5);    }
void autotest_mod_demod_arb64opt()  { modem_test_mod_demod(LIQUID_MODEM_ARB64OPT,6);    }
void autotest_mod_demod_arb128opt() { modem_test_mod_demod(LIQUID_MODEM_ARB128OPT,7);   }
void autotest_mod_demod_arb256opt() { modem_test_mod_demod(LIQUID_MODEM_ARB256OPT,8);   }
void autotest_mod_demod_arb64vt()   { modem_test_mod_demod(LIQUID_MODEM_ARB64VT,6);     }

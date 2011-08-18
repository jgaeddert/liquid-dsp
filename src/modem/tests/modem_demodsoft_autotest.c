/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

//
// soft demodulation tests
//

#include "autotest/autotest.h"
#include "liquid.h"

// Help function to keep code base small
void modem_test_demodsoft(modulation_scheme _ms, unsigned int _bps)
{
    // generate mod/demod
    modem mod = modem_create(_ms, _bps);
    modem demod = modem_create(_ms, _bps);

    // 
    unsigned int bps = modem_get_bps(demod);

    // run the test
    unsigned int i, s, M=1<<bps;
    unsigned int sym_soft;
    unsigned char soft_bits[bps];
    float complex x;
    
    for (i=0; i<M; i++) {
        // modulate symbol
        modem_modulate(mod, i, &x);

        // demodulate using soft-decision
        modem_demodulate_soft(demod, x, &s, soft_bits);

        // check hard-decision output
        CONTEND_EQUALITY(s, i);

        // check soft bits
        liquid_pack_soft_bits(soft_bits, bps, &sym_soft);
        CONTEND_EQUALITY(sym_soft, i);

        // check phase error, evm, etc.
        //CONTEND_DELTA( modem_get_demodulator_phase_error(demod), 0.0f, 1e-3f);
        //CONTEND_DELTA( modem_get_demodulator_evm(demod), 0.0f, 1e-3f);
    }
    // clean it up
    modem_destroy(mod);
    modem_destroy(demod);
}

//
// AUTOTESTS: Specific modems
//
void autotest_demodsoft_bpsk()  {   modem_test_demodsoft(LIQUID_MODEM_BPSK, 1);  }
void autotest_demodsoft_qpsk()  {   modem_test_demodsoft(LIQUID_MODEM_QPSK, 2);  }
void autotest_demodsoft_ook()   {   modem_test_demodsoft(LIQUID_MODEM_OOK,  1);  }
void autotest_demodsoft_sqam32(){   modem_test_demodsoft(LIQUID_MODEM_SQAM32,5);  }
void autotest_demodsoft_sqam128(){  modem_test_demodsoft(LIQUID_MODEM_SQAM128,7);  }

//
// AUTOTESTS: generic ASK
//
void autotest_demodsoft_ask2()  {   modem_test_demodsoft(LIQUID_MODEM_ASK, 1);   }
void autotest_demodsoft_ask4()  {   modem_test_demodsoft(LIQUID_MODEM_ASK, 2);   }
void autotest_demodsoft_ask8()  {   modem_test_demodsoft(LIQUID_MODEM_ASK, 3);   }
void autotest_demodsoft_ask16() {   modem_test_demodsoft(LIQUID_MODEM_ASK, 4);   }

//
// AUTOTESTS: generic PSK
//
void autotest_demodsoft_psk2()  {   modem_test_demodsoft(LIQUID_MODEM_PSK, 1);   }
void autotest_demodsoft_psk4()  {   modem_test_demodsoft(LIQUID_MODEM_PSK, 2);   }
void autotest_demodsoft_psk8()  {   modem_test_demodsoft(LIQUID_MODEM_PSK, 3);   }
void autotest_demodsoft_psk16() {   modem_test_demodsoft(LIQUID_MODEM_PSK, 4);   }
void autotest_demodsoft_psk32() {   modem_test_demodsoft(LIQUID_MODEM_PSK, 5);   }
void autotest_demodsoft_psk64() {   modem_test_demodsoft(LIQUID_MODEM_PSK, 6);   }

//
// AUTOTESTS: generic differential PSK
//
void autotest_demodsoft_dpsk2()  {  modem_test_demodsoft(LIQUID_MODEM_DPSK, 1);  }
void autotest_demodsoft_dpsk4()  {  modem_test_demodsoft(LIQUID_MODEM_DPSK, 2);  }
void autotest_demodsoft_dpsk8()  {  modem_test_demodsoft(LIQUID_MODEM_DPSK, 3);  }
void autotest_demodsoft_dpsk16() {  modem_test_demodsoft(LIQUID_MODEM_DPSK, 4);  }
void autotest_demodsoft_dpsk32() {  modem_test_demodsoft(LIQUID_MODEM_DPSK, 5);  }
void autotest_demodsoft_dpsk64() {  modem_test_demodsoft(LIQUID_MODEM_DPSK, 6);  }

//
// AUTOTESTS: generic QAM
//
void autotest_demodsoft_qam4()   {  modem_test_demodsoft(LIQUID_MODEM_QAM, 2);   }
void autotest_demodsoft_qam8()   {  modem_test_demodsoft(LIQUID_MODEM_QAM, 3);   }
void autotest_demodsoft_qam16()  {  modem_test_demodsoft(LIQUID_MODEM_QAM, 4);   }
void autotest_demodsoft_qam32()  {  modem_test_demodsoft(LIQUID_MODEM_QAM, 5);   }
void autotest_demodsoft_qam64()  {  modem_test_demodsoft(LIQUID_MODEM_QAM, 6);   }
void autotest_demodsoft_qam128() {  modem_test_demodsoft(LIQUID_MODEM_QAM, 7);   }
void autotest_demodsoft_qam256() {  modem_test_demodsoft(LIQUID_MODEM_QAM, 8);   }

//
// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
//
void autotest_demodsoft_apsk4()  {  modem_test_demodsoft(LIQUID_MODEM_APSK,2);   }
void autotest_demodsoft_apsk8()  {  modem_test_demodsoft(LIQUID_MODEM_APSK,3);   }
void autotest_demodsoft_apsk16() {  modem_test_demodsoft(LIQUID_MODEM_APSK,4);   }
void autotest_demodsoft_apsk32() {  modem_test_demodsoft(LIQUID_MODEM_APSK,5);   }
void autotest_demodsoft_apsk64() {  modem_test_demodsoft(LIQUID_MODEM_APSK,6);   }
void autotest_demodsoft_apsk128(){  modem_test_demodsoft(LIQUID_MODEM_APSK,7);   }
void autotest_demodsoft_apsk256(){  modem_test_demodsoft(LIQUID_MODEM_APSK,8);   }

//
// AUTOTESTS: arbitrary modems
//
void autotest_demodsoft_arbV29()    { modem_test_demodsoft(LIQUID_MODEM_V29,4);         }
void autotest_demodsoft_arb16opt()  { modem_test_demodsoft(LIQUID_MODEM_ARB16OPT,4);    }
void autotest_demodsoft_arb32opt()  { modem_test_demodsoft(LIQUID_MODEM_ARB32OPT,5);    }
void autotest_demodsoft_arb64vt()   { modem_test_demodsoft(LIQUID_MODEM_ARB64VT,6);     }

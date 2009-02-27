#ifndef __MODEM_AUTOTEST_H__
#define __MODEM_AUTOTEST_H__

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
    float phase_error, evm;
    for (i=0; i<M; i++) {
        modulate(mod, i, &x);
        demodulate(demod, x, &s);
        CONTEND_EQUALITY(s, i);

        get_demodulator_phase_error(demod, &phase_error);
        CONTEND_DELTA(phase_error, 0.0f, 1e-6f);
        get_demodulator_evm(demod, &evm);
        CONTEND_DELTA(evm, 0.0f, 1e-6f);
    }

    // clean it up
    modem_destroy(mod);
    modem_destroy(demod);
}

//
// AUTOTESTS: Specific PSK
//
void autotest_mod_demod_bpsk()  {   modem_test_mod_demod(MOD_BPSK, 1);  }
void autotest_mod_demod_qpsk()  {   modem_test_mod_demod(MOD_QPSK, 2);  }

//
// AUTOTESTS: generic ASK
//
void autotest_mod_demod_ask2()  {   modem_test_mod_demod(MOD_PSK, 1);   }
void autotest_mod_demod_ask4()  {   modem_test_mod_demod(MOD_PSK, 2);   }
void autotest_mod_demod_ask8()  {   modem_test_mod_demod(MOD_PSK, 3);   }
void autotest_mod_demod_ask16() {   modem_test_mod_demod(MOD_PSK, 4);   }

//
// AUTOTESTS: generic PSK
//
void autotest_mod_demod_psk2()  {   modem_test_mod_demod(MOD_PSK, 1);   }
void autotest_mod_demod_psk4()  {   modem_test_mod_demod(MOD_PSK, 2);   }
void autotest_mod_demod_psk8()  {   modem_test_mod_demod(MOD_PSK, 3);   }
void autotest_mod_demod_psk16() {   modem_test_mod_demod(MOD_PSK, 4);   }
void autotest_mod_demod_psk32() {   modem_test_mod_demod(MOD_PSK, 5);   }
void autotest_mod_demod_psk64() {   modem_test_mod_demod(MOD_PSK, 6);   }

//
// AUTOTESTS: generic differential PSK
//
void autotest_mod_demod_dpsk2()  {  modem_test_mod_demod(MOD_DPSK, 1);  }
void autotest_mod_demod_dpsk4()  {  modem_test_mod_demod(MOD_DPSK, 2);  }
void autotest_mod_demod_dpsk8()  {  modem_test_mod_demod(MOD_DPSK, 3);  }
void autotest_mod_demod_dpsk16() {  modem_test_mod_demod(MOD_DPSK, 4);  }
void autotest_mod_demod_dpsk32() {  modem_test_mod_demod(MOD_DPSK, 5);  }
void autotest_mod_demod_dpsk64() {  modem_test_mod_demod(MOD_DPSK, 6);  }

//
// AUTOTESTS: generic QAM
//
void autotest_mod_demod_qam4()   {  modem_test_mod_demod(MOD_QAM, 2);   }
void autotest_mod_demod_qam8()   {  modem_test_mod_demod(MOD_QAM, 3);   }
void autotest_mod_demod_qam16()  {  modem_test_mod_demod(MOD_QAM, 4);   }
void autotest_mod_demod_qam32()  {  modem_test_mod_demod(MOD_QAM, 5);   }
void autotest_mod_demod_qam64()  {  modem_test_mod_demod(MOD_QAM, 6);   }
void autotest_mod_demod_qam128() {  modem_test_mod_demod(MOD_QAM, 7);   }
void autotest_mod_demod_qam256() {  modem_test_mod_demod(MOD_QAM, 8);   }


#endif 


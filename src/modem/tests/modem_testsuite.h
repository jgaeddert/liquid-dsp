#ifndef __MODEMTEST_H__
#define __MODEMTEST_H__

#include "autotest.h"
#include "../src/modem.h"
#include "../src/modem_internal.h"

void test_modem_pam() {
    unsigned int bits_per_symbol;
    for (bits_per_symbol=1; bits_per_symbol<6; bits_per_symbol++) {
        modem mod = modem_create(MOD_PAM, bits_per_symbol);
        modem demod = modem_create(MOD_PAM, bits_per_symbol);

        unsigned int i, s;
        complex x;
        float phase_error, evm;
        for (i=0; i<mod->M; i++) {
            modulate(mod, i, &x);
            demodulate(demod, x, &s);
            TS_ASSERT_EQUALS(s, i);

            get_demodulator_phase_error(demod, &phase_error);
            TS_ASSERT_DELTA(phase_error, 0.0f, 1e-6f);
            get_demodulator_evm(demod, &evm);
            TS_ASSERT_DELTA(evm, 0.0f, 1e-6f);
        }

        free_modem(mod);
        free_modem(demod);
    }
}


#endif 


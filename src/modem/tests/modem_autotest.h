#ifndef __MODEM_OPTS_AUTOTEST_H__
#define __MODEM_OPTS_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/modem.h"
//#include "../src/modem_internal.h"

// Structure for holding modulation schemes
typedef struct {
    modulation_scheme ms;
    unsigned int bps;
} mod_set;

#define NUM_MODEMS 17
mod_set modem_test[NUM_MODEMS] = {
    {MOD_BPSK,1}, {MOD_QPSK,2},
    {MOD_PSK,1},  {MOD_PSK,2},  {MOD_PSK,3},  {MOD_PSK,4},  {MOD_PSK,5},
    {MOD_DPSK,1}, {MOD_DPSK,2}, {MOD_DPSK,3}, {MOD_DPSK,4}, {MOD_DPSK,5},
    {MOD_QAM,2},  {MOD_QAM,3},  {MOD_QAM,4},  {MOD_QAM,5},  {MOD_QAM,6}
};

// Help function to keep code base small
void modem_test_mod_demod(modem _mod, modem _demod)
{
    unsigned int i, s, M=1<<modem_get_bps(_mod);
    complex x;
    float phase_error, evm;
    for (i=0; i<M; i++) {
        modulate(_mod, i, &x);
        demodulate(_demod, x, &s);
        CONTEND_EQUALITY(s, i);

        get_demodulator_phase_error(_demod, &phase_error);
        CONTEND_DELTA(phase_error, 0.0f, 1e-6f);
        get_demodulator_evm(_demod, &evm);
        CONTEND_DELTA(evm, 0.0f, 1e-6f);
    }
}

//
// AUTOTEST: mod_demod, tests modulating and demodulating symbols
//
void autotest_mod_demod()
{
    unsigned int i;
    for (i=0; i<NUM_MODEMS; i++) {
        // print modem type (for debugging)
        printf("Testing %d-%s:\n", 1<<(modem_test[i].bps), modulation_scheme_str[modem_test[i].ms]);

        // generate mod/demod
        modem mod = modem_create(modem_test[i].ms, modem_test[i].bps);
        modem demod = modem_create(modem_test[i].ms, modem_test[i].bps);

        // execute test
        modem_test_mod_demod(mod, demod);

        // clean it up
        free_modem(mod);
        free_modem(demod);
    }

}

#endif 


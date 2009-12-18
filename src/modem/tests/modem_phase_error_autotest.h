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

#ifndef __MODEM_PHASE_ERROR_AUTOTEST_H__
#define __MODEM_PHASE_ERROR_AUTOTEST_H__

#include <stdio.h>

#include "autotest/autotest.h"
#include "liquid.h"

// Helper function to keep code base small
void modem_test_phase_error(modulation_scheme _ms, unsigned int _bps)
{
    // generate mod/demod
    modem mod = modem_create(_ms, _bps);
    modem demod = modem_create(_ms, _bps);

    // run the test
    unsigned int i, s, M=1<<_bps;
    float complex x;
    float complex x_phi_pos;    // positive phase error demod input
    float complex x_phi_neg;    // negative phase error demod input
    float phase_error;
    float phi = 0.01f;

    for (i=0; i<M; i++) {
        // modulate symbol
        modem_modulate(mod, i, &x);

        // ignore rare condition where modulated symbol is (0,0)
        // (e.g. APSK-8)
        if (cabsf(x) < 1e-3f) continue;

        // add phase offsets
        x_phi_pos = x * cexpf( phi*_Complex_I);
        x_phi_neg = x * cexpf(-phi*_Complex_I);

        // demod positive phase signal, and ensure demodulator
        // maps to appropriate symbol
        modem_demodulate(demod, x_phi_pos, &s);
        if (s != i) {
            fprintf(stderr,"warning: modem_test_phase_error(), output symbol does not match\n");
        }
        get_demodulator_phase_error(demod,&phase_error);
        CONTEND_EXPRESSION(phase_error > 0.0f);

        // demod negative phase signal, and ensure demodulator
        // maps to appropriate symbol
        modem_demodulate(demod, x_phi_neg, &s);
        if (s != i) {
            fprintf(stderr,"warning: modem_test_phase_error(), output symbol does not match\n");
        }
        get_demodulator_phase_error(demod,&phase_error);
        CONTEND_EXPRESSION(phase_error < 0.0f);
    }

    // clean up allocated objects up
    modem_destroy(mod);
    modem_destroy(demod);
}

//
// AUTOTESTS: Specific PSK
//
void autotest_phase_error_bpsk()    {   modem_test_phase_error(MOD_BPSK,1); }
void autotest_phase_error_qpsk()    {   modem_test_phase_error(MOD_QPSK,2); }

//
// AUTOTESTS: generic ASK
//
void autotest_phase_error_ask2()    {   modem_test_phase_error(MOD_PSK, 1); }
void autotest_phase_error_ask4()    {   modem_test_phase_error(MOD_PSK, 2); }
void autotest_phase_error_ask8()    {   modem_test_phase_error(MOD_PSK, 3); }
void autotest_phase_error_ask16()   {   modem_test_phase_error(MOD_PSK, 4); }

//
// AUTOTESTS: generic PSK
//
void autotest_phase_error_psk2()    {   modem_test_phase_error(MOD_PSK, 1); }
void autotest_phase_error_psk4()    {   modem_test_phase_error(MOD_PSK, 2); }
void autotest_phase_error_psk8()    {   modem_test_phase_error(MOD_PSK, 3); }
void autotest_phase_error_psk16()   {   modem_test_phase_error(MOD_PSK, 4); }
void autotest_phase_error_psk32()   {   modem_test_phase_error(MOD_PSK, 5); }
void autotest_phase_error_psk64()   {   modem_test_phase_error(MOD_PSK, 6); }

//
// AUTOTESTS: generic differential PSK
//
void autotest_phase_error_dpsk2()   {  modem_test_phase_error(MOD_DPSK, 1); }
void autotest_phase_error_dpsk4()   {  modem_test_phase_error(MOD_DPSK, 2); }
void autotest_phase_error_dpsk8()   {  modem_test_phase_error(MOD_DPSK, 3); }
void autotest_phase_error_dpsk16()  {  modem_test_phase_error(MOD_DPSK, 4); }
void autotest_phase_error_dpsk32()  {  modem_test_phase_error(MOD_DPSK, 5); }
void autotest_phase_error_dpsk64()  {  modem_test_phase_error(MOD_DPSK, 6); }

//
// AUTOTESTS: generic QAM
//
void autotest_phase_error_qam4()    {   modem_test_phase_error(MOD_QAM, 2); }
void autotest_phase_error_qam8()    {   modem_test_phase_error(MOD_QAM, 3); }
void autotest_phase_error_qam16()   {   modem_test_phase_error(MOD_QAM, 4); }
void autotest_phase_error_qam32()   {   modem_test_phase_error(MOD_QAM, 5); }
void autotest_phase_error_qam64()   {   modem_test_phase_error(MOD_QAM, 6); }
void autotest_phase_error_qam128()  {   modem_test_phase_error(MOD_QAM, 7); }
void autotest_phase_error_qam256()  {   modem_test_phase_error(MOD_QAM, 8); }

//
// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
//
void autotest_phase_error_apsk8()   {   modem_test_phase_error(MOD_APSK,3); }
void autotest_phase_error_apsk16()  {   modem_test_phase_error(MOD_APSK,4); }
void autotest_phase_error_apsk32()  {   modem_test_phase_error(MOD_APSK,5); }
void autotest_phase_error_apsk64()  {   modem_test_phase_error(MOD_APSK,6); }
void autotest_phase_error_apsk128() {   modem_test_phase_error(MOD_APSK,7); }


#endif // __MODEM_PHASE_ERROR_AUTOTEST_H__


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
void modem_test_phase_error(modulation_scheme _ms, unsigned int _bps)
{
    // generate mod/demod
    modem mod = modem_create(_ms, _bps);
    modem demod = modem_create(_ms, _bps);

    // run the test
    unsigned int i, s, M=1<<_bps;
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
// AUTOTESTS: Specific PSK
//
void autotest_phase_error_bpsk()    {   modem_test_phase_error(LIQUID_MODEM_BPSK,1); }
void autotest_phase_error_qpsk()    {   modem_test_phase_error(LIQUID_MODEM_QPSK,2); }
void autotest_phase_error_ook()     {   modem_test_phase_error(LIQUID_MODEM_OOK, 1); }
void autotest_phase_error_sqam32()  {   modem_test_phase_error(LIQUID_MODEM_SQAM32,5); }
void autotest_phase_error_sqam128() {   modem_test_phase_error(LIQUID_MODEM_SQAM128,7); }

//
// AUTOTESTS: generic ASK
//
void autotest_phase_error_ask2()    {   modem_test_phase_error(LIQUID_MODEM_ASK, 1); }
void autotest_phase_error_ask4()    {   modem_test_phase_error(LIQUID_MODEM_ASK, 2); }
void autotest_phase_error_ask8()    {   modem_test_phase_error(LIQUID_MODEM_ASK, 3); }
void autotest_phase_error_ask16()   {   modem_test_phase_error(LIQUID_MODEM_ASK, 4); }

//
// AUTOTESTS: generic PSK
//
void autotest_phase_error_psk2()    {   modem_test_phase_error(LIQUID_MODEM_PSK, 1); }
void autotest_phase_error_psk4()    {   modem_test_phase_error(LIQUID_MODEM_PSK, 2); }
void autotest_phase_error_psk8()    {   modem_test_phase_error(LIQUID_MODEM_PSK, 3); }
void autotest_phase_error_psk16()   {   modem_test_phase_error(LIQUID_MODEM_PSK, 4); }
void autotest_phase_error_psk32()   {   modem_test_phase_error(LIQUID_MODEM_PSK, 5); }
void autotest_phase_error_psk64()   {   modem_test_phase_error(LIQUID_MODEM_PSK, 6); }

//
// AUTOTESTS: generic differential PSK
//
void autotest_phase_error_dpsk2()   {  modem_test_phase_error(LIQUID_MODEM_DPSK, 1); }
void autotest_phase_error_dpsk4()   {  modem_test_phase_error(LIQUID_MODEM_DPSK, 2); }
void autotest_phase_error_dpsk8()   {  modem_test_phase_error(LIQUID_MODEM_DPSK, 3); }
void autotest_phase_error_dpsk16()  {  modem_test_phase_error(LIQUID_MODEM_DPSK, 4); }
void autotest_phase_error_dpsk32()  {  modem_test_phase_error(LIQUID_MODEM_DPSK, 5); }
void autotest_phase_error_dpsk64()  {  modem_test_phase_error(LIQUID_MODEM_DPSK, 6); }

//
// AUTOTESTS: generic QAM
//
void autotest_phase_error_qam4()    {   modem_test_phase_error(LIQUID_MODEM_QAM, 2); }
void autotest_phase_error_qam8()    {   modem_test_phase_error(LIQUID_MODEM_QAM, 3); }
void autotest_phase_error_qam16()   {   modem_test_phase_error(LIQUID_MODEM_QAM, 4); }
void autotest_phase_error_qam32()   {   modem_test_phase_error(LIQUID_MODEM_QAM, 5); }
void autotest_phase_error_qam64()   {   modem_test_phase_error(LIQUID_MODEM_QAM, 6); }
void autotest_phase_error_qam128()  {   modem_test_phase_error(LIQUID_MODEM_QAM, 7); }
void autotest_phase_error_qam256()  {   modem_test_phase_error(LIQUID_MODEM_QAM, 8); }

//
// AUTOTESTS: generic APSK (maps to specific APSK modems internally)
//
void autotest_phase_error_apsk4()   {   modem_test_phase_error(LIQUID_MODEM_APSK,2); }
void autotest_phase_error_apsk8()   {   modem_test_phase_error(LIQUID_MODEM_APSK,3); }
void autotest_phase_error_apsk16()  {   modem_test_phase_error(LIQUID_MODEM_APSK,4); }
void autotest_phase_error_apsk32()  {   modem_test_phase_error(LIQUID_MODEM_APSK,5); }
void autotest_phase_error_apsk64()  {   modem_test_phase_error(LIQUID_MODEM_APSK,6); }
void autotest_phase_error_apsk128() {   modem_test_phase_error(LIQUID_MODEM_APSK,7); }
void autotest_phase_error_apsk256() {   modem_test_phase_error(LIQUID_MODEM_APSK,8); }


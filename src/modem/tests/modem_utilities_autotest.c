/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.internal.h"

// test printing schemes
LIQUID_AUTOTEST(modemcf_print_schemes,"","",0.1)
{
    LIQUID_CHECK(liquid_print_modulation_schemes() ==  LIQUID_OK);
}

// test parsing string to modulation scheme
LIQUID_AUTOTEST(modemcf_str2mod,"","",0.1)
{
    // start with invalid case
    _liquid_error_downgrade_enable();
    LIQUID_CHECK(liquid_getopt_str2mod("invalid scheme") ==  LIQUID_MODEM_UNKNOWN);
    _liquid_error_downgrade_disable();

    // check normal cases
    LIQUID_CHECK(liquid_getopt_str2mod("psk2"     ) ==  LIQUID_MODEM_PSK2);
    LIQUID_CHECK(liquid_getopt_str2mod("psk4"     ) ==  LIQUID_MODEM_PSK4);
    LIQUID_CHECK(liquid_getopt_str2mod("psk8"     ) ==  LIQUID_MODEM_PSK8);
    LIQUID_CHECK(liquid_getopt_str2mod("psk16"    ) ==  LIQUID_MODEM_PSK16);
    LIQUID_CHECK(liquid_getopt_str2mod("psk32"    ) ==  LIQUID_MODEM_PSK32);
    LIQUID_CHECK(liquid_getopt_str2mod("psk64"    ) ==  LIQUID_MODEM_PSK64);
    LIQUID_CHECK(liquid_getopt_str2mod("psk128"   ) ==  LIQUID_MODEM_PSK128);
    LIQUID_CHECK(liquid_getopt_str2mod("psk256"   ) ==  LIQUID_MODEM_PSK256);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk2"    ) ==  LIQUID_MODEM_DPSK2);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk4"    ) ==  LIQUID_MODEM_DPSK4);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk8"    ) ==  LIQUID_MODEM_DPSK8);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk16"   ) ==  LIQUID_MODEM_DPSK16);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk32"   ) ==  LIQUID_MODEM_DPSK32);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk64"   ) ==  LIQUID_MODEM_DPSK64);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk128"  ) ==  LIQUID_MODEM_DPSK128);
    LIQUID_CHECK(liquid_getopt_str2mod("dpsk256"  ) ==  LIQUID_MODEM_DPSK256);
    LIQUID_CHECK(liquid_getopt_str2mod("ask2"     ) ==  LIQUID_MODEM_ASK2);
    LIQUID_CHECK(liquid_getopt_str2mod("ask4"     ) ==  LIQUID_MODEM_ASK4);
    LIQUID_CHECK(liquid_getopt_str2mod("ask8"     ) ==  LIQUID_MODEM_ASK8);
    LIQUID_CHECK(liquid_getopt_str2mod("ask16"    ) ==  LIQUID_MODEM_ASK16);
    LIQUID_CHECK(liquid_getopt_str2mod("ask32"    ) ==  LIQUID_MODEM_ASK32);
    LIQUID_CHECK(liquid_getopt_str2mod("ask64"    ) ==  LIQUID_MODEM_ASK64);
    LIQUID_CHECK(liquid_getopt_str2mod("ask128"   ) ==  LIQUID_MODEM_ASK128);
    LIQUID_CHECK(liquid_getopt_str2mod("ask256"   ) ==  LIQUID_MODEM_ASK256);
    LIQUID_CHECK(liquid_getopt_str2mod("qam4"     ) ==  LIQUID_MODEM_QAM4);
    LIQUID_CHECK(liquid_getopt_str2mod("qam8"     ) ==  LIQUID_MODEM_QAM8);
    LIQUID_CHECK(liquid_getopt_str2mod("qam16"    ) ==  LIQUID_MODEM_QAM16);
    LIQUID_CHECK(liquid_getopt_str2mod("qam32"    ) ==  LIQUID_MODEM_QAM32);
    LIQUID_CHECK(liquid_getopt_str2mod("qam64"    ) ==  LIQUID_MODEM_QAM64);
    LIQUID_CHECK(liquid_getopt_str2mod("qam128"   ) ==  LIQUID_MODEM_QAM128);
    LIQUID_CHECK(liquid_getopt_str2mod("qam256"   ) ==  LIQUID_MODEM_QAM256);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk4"    ) ==  LIQUID_MODEM_APSK4);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk8"    ) ==  LIQUID_MODEM_APSK8);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk16"   ) ==  LIQUID_MODEM_APSK16);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk32"   ) ==  LIQUID_MODEM_APSK32);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk64"   ) ==  LIQUID_MODEM_APSK64);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk128"  ) ==  LIQUID_MODEM_APSK128);
    LIQUID_CHECK(liquid_getopt_str2mod("apsk256"  ) ==  LIQUID_MODEM_APSK256);
    LIQUID_CHECK(liquid_getopt_str2mod("bpsk"     ) ==  LIQUID_MODEM_BPSK);
    LIQUID_CHECK(liquid_getopt_str2mod("qpsk"     ) ==  LIQUID_MODEM_QPSK);
    LIQUID_CHECK(liquid_getopt_str2mod("ook"      ) ==  LIQUID_MODEM_OOK);
    LIQUID_CHECK(liquid_getopt_str2mod("sqam32"   ) ==  LIQUID_MODEM_SQAM32);
    LIQUID_CHECK(liquid_getopt_str2mod("sqam128"  ) ==  LIQUID_MODEM_SQAM128);
    LIQUID_CHECK(liquid_getopt_str2mod("V29"      ) ==  LIQUID_MODEM_V29);
    LIQUID_CHECK(liquid_getopt_str2mod("arb16opt" ) ==  LIQUID_MODEM_ARB16OPT);
    LIQUID_CHECK(liquid_getopt_str2mod("arb32opt" ) ==  LIQUID_MODEM_ARB32OPT);
    LIQUID_CHECK(liquid_getopt_str2mod("arb64opt" ) ==  LIQUID_MODEM_ARB64OPT);
    LIQUID_CHECK(liquid_getopt_str2mod("arb128opt") ==  LIQUID_MODEM_ARB128OPT);
    LIQUID_CHECK(liquid_getopt_str2mod("arb256opt") ==  LIQUID_MODEM_ARB256OPT);
    LIQUID_CHECK(liquid_getopt_str2mod("arb64vt"  ) ==  LIQUID_MODEM_ARB64VT);
}

// test basic types
LIQUID_AUTOTEST(modemcf_types,"","",0.1)
{
    // Phase-shift keying (PSK)
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK2) ==      1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK4) ==      1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK8) ==      1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK16) ==     1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK32) ==     1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK64) ==     1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK128) ==    1);
    LIQUID_CHECK(liquid_modem_is_psk(LIQUID_MODEM_PSK256) ==    1);

    // Differential phase-shift keying (DPSK)
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK2) ==     1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK4) ==     1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK8) ==     1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK16) ==    1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK32) ==    1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK64) ==    1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK128) ==   1);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_DPSK256) ==   1);

    // amplitude-shift keying (ASK)
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK2) ==      1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK4) ==      1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK8) ==      1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK16) ==     1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK32) ==     1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK64) ==     1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK128) ==    1);
    LIQUID_CHECK(liquid_modem_is_ask(LIQUID_MODEM_ASK256) ==    1);

    // rectangular quadrature amplitude-shift keying (QAM)
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM4) ==      1);
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM8) ==      1);
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM16) ==     1);
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM32) ==     1);
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM64) ==     1);
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM128) ==    1);
    LIQUID_CHECK(liquid_modem_is_qam(LIQUID_MODEM_QAM256) ==    1);

    // amplitude phase-shift keying (APSK)
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK4) ==     1);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK8) ==     1);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK16) ==    1);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK32) ==    1);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK64) ==    1);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK128) ==   1);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_APSK256) ==   1);

#if 0
    // specific modems
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_BPSK) ==       1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_QPSK) ==       1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_OOK) ==        1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_SQAM32) ==     1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_SQAM128) ==    1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_V29) ==        1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_ARB16OPT) ==   1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_ARB32OPT) ==   1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_ARB64OPT) ==   1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_ARB128OPT) ==  1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_ARB256OPT) ==  1);
    LIQUID_CHECK(liquid_modem_is_specific(LIQUID_MODEM_ARB64VT) ==    1);
#endif

    // test some negative cases
    LIQUID_CHECK(liquid_modem_is_psk (LIQUID_MODEM_QPSK) ==  0);
    LIQUID_CHECK(liquid_modem_is_dpsk(LIQUID_MODEM_QPSK) ==  0);
    LIQUID_CHECK(liquid_modem_is_ask (LIQUID_MODEM_QPSK) ==  0);
    LIQUID_CHECK(liquid_modem_is_qam (LIQUID_MODEM_QPSK) ==  0);
    LIQUID_CHECK(liquid_modem_is_apsk(LIQUID_MODEM_QPSK) ==  0);
}


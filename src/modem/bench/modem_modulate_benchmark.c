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

#include "liquid.benchmark.h"
#include "liquid.internal.h"

// Helper function to keep code base small
float modemcf_modulate_bench(unsigned long int _num_iterations,
                             modulation_scheme _ms)
{
    // initialize modulator
    modemcf mod = modemcf_create(_ms);
    unsigned long int i;
    float complex x;
    unsigned int symbol_in = 0;
    // start trials (4 modulates per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        modemcf_modulate(mod, symbol_in, &x);
        modemcf_modulate(mod, symbol_in, &x);
        modemcf_modulate(mod, symbol_in, &x);
        modemcf_modulate(mod, symbol_in, &x);
    }
    float extime = liquid_toc(timer);
    modemcf_destroy(mod);
    return extime;
}

// specific modems
LIQUID_BENCHMARK(modem_modulate_bpsk,    "modemcf modulate, bpsk",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_BPSK); }
LIQUID_BENCHMARK(modem_modulate_qpsk,    "modemcf modulate, qpsk",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QPSK); }
LIQUID_BENCHMARK(modem_modulate_ook,     "modemcf modulate, ook",     "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_OOK); }
LIQUID_BENCHMARK(modem_modulate_sqam32,  "modemcf modulate, sqam32",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_SQAM32); }
LIQUID_BENCHMARK(modem_modulate_sqam128, "modemcf modulate, sqam128", "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_SQAM128); }

// ASK
LIQUID_BENCHMARK(modem_modulate_ask2,    "modemcf modulate, ask2",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ASK2); }
LIQUID_BENCHMARK(modem_modulate_ask4,    "modemcf modulate, ask4",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ASK4); }
LIQUID_BENCHMARK(modem_modulate_ask8,    "modemcf modulate, ask8",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ASK8); }
LIQUID_BENCHMARK(modem_modulate_ask16,   "modemcf modulate, ask16",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ASK16); }

// PSK
LIQUID_BENCHMARK(modem_modulate_psk2,    "modemcf modulate, psk2",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_PSK2); }
LIQUID_BENCHMARK(modem_modulate_psk4,    "modemcf modulate, psk4",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_PSK4); }
LIQUID_BENCHMARK(modem_modulate_psk8,    "modemcf modulate, psk8",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_PSK8); }
LIQUID_BENCHMARK(modem_modulate_psk16,   "modemcf modulate, psk16",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_PSK16); }
LIQUID_BENCHMARK(modem_modulate_psk32,   "modemcf modulate, psk32",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_PSK32); }
LIQUID_BENCHMARK(modem_modulate_psk64,   "modemcf modulate, psk64",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_PSK64); }

// Differential PSK
LIQUID_BENCHMARK(modem_modulate_dpsk2,   "modemcf modulate, dpsk2",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_DPSK2); }
LIQUID_BENCHMARK(modem_modulate_dpsk4,   "modemcf modulate, dpsk4",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_DPSK4); }
LIQUID_BENCHMARK(modem_modulate_dpsk8,   "modemcf modulate, dpsk8",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_DPSK8); }
LIQUID_BENCHMARK(modem_modulate_dpsk16,  "modemcf modulate, dpsk16",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_DPSK16); }
LIQUID_BENCHMARK(modem_modulate_dpsk32,  "modemcf modulate, dpsk32",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_DPSK32); }
LIQUID_BENCHMARK(modem_modulate_dpsk64,  "modemcf modulate, dpsk64",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_DPSK64); }

// QAM
LIQUID_BENCHMARK(modem_modulate_qam4,    "modemcf modulate, qam4",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM4); }
LIQUID_BENCHMARK(modem_modulate_qam8,    "modemcf modulate, qam8",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM8); }
LIQUID_BENCHMARK(modem_modulate_qam16,   "modemcf modulate, qam16",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM16); }
LIQUID_BENCHMARK(modem_modulate_qam32,   "modemcf modulate, qam32",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM32); }
LIQUID_BENCHMARK(modem_modulate_qam64,   "modemcf modulate, qam64",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM64); }
LIQUID_BENCHMARK(modem_modulate_qam128,  "modemcf modulate, qam128",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM128); }
LIQUID_BENCHMARK(modem_modulate_qam256,  "modemcf modulate, qam256",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_QAM256); }

// APSK
LIQUID_BENCHMARK(modem_modulate_apsk4,   "modemcf modulate, apsk4",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK4); }
LIQUID_BENCHMARK(modem_modulate_apsk8,   "modemcf modulate, apsk8",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK8); }
LIQUID_BENCHMARK(modem_modulate_apsk16,  "modemcf modulate, apsk16",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK16); }
LIQUID_BENCHMARK(modem_modulate_apsk32,  "modemcf modulate, apsk32",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK32); }
LIQUID_BENCHMARK(modem_modulate_apsk64,  "modemcf modulate, apsk64",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK64); }
LIQUID_BENCHMARK(modem_modulate_apsk128, "modemcf modulate, apsk128", "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK128); }
LIQUID_BENCHMARK(modem_modulate_apsk256, "modemcf modulate, apsk256", "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_APSK256); }

// ARB
LIQUID_BENCHMARK(modem_modulate_arbV29,    "modemcf modulate, arbV29",    "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_V29); }
LIQUID_BENCHMARK(modem_modulate_arb16opt,  "modemcf modulate, arb16opt",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ARB16OPT); }
LIQUID_BENCHMARK(modem_modulate_arb32opt,  "modemcf modulate, arb32opt",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ARB32OPT); }
LIQUID_BENCHMARK(modem_modulate_arb64opt,  "modemcf modulate, arb64opt",  "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ARB64OPT); }
LIQUID_BENCHMARK(modem_modulate_arb128opt, "modemcf modulate, arb128opt", "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ARB128OPT); }
LIQUID_BENCHMARK(modem_modulate_arb256opt, "modemcf modulate, arb256opt", "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ARB256OPT); }
LIQUID_BENCHMARK(modem_modulate_arb64vt,   "modemcf modulate, arb64vt",   "modem")
    { return modemcf_modulate_bench(num_iterations, LIQUID_MODEM_ARB64VT); }


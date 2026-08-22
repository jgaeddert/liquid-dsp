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
#include <math.h>
#include "liquid.internal.h"

// Helper function to keep code base small
float modemcf_demodulate_soft_bench(unsigned long int _num_iterations,
                                    modulation_scheme _ms)
{
    // initialize modulator
    modemcf demod = modemcf_create(_ms);
    unsigned int bps = modemcf_get_bps(demod);
    unsigned long int i;
    // generate input vector to demodulate (spiral)
    float complex x[20];
    for (i=0; i<20; i++)
        x[i] = 0.07 * i * cexpf(_Complex_I*2*M_PI*0.1*i);
    unsigned int symbol_out;
    unsigned char soft_bits[bps];
    // start trials (20 soft demodulates per iteration; round down)
    unsigned long int n = _num_iterations / 20;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        modemcf_demodulate_soft(demod, x[ 0], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 1], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 2], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 3], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 4], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 5], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 6], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 7], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 8], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[ 9], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[10], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[11], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[12], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[13], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[14], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[15], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[16], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[17], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[18], &symbol_out, soft_bits);
        modemcf_demodulate_soft(demod, x[19], &symbol_out, soft_bits);
    }
    float extime = liquid_toc(timer);
    modemcf_destroy(demod);
    return extime;
}

// specific modems
LIQUID_BENCHMARK(modem_demodsoft_bpsk,    "modemcf demodulate_soft, bpsk",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_BPSK); }
LIQUID_BENCHMARK(modem_demodsoft_qpsk,    "modemcf demodulate_soft, qpsk",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QPSK); }
LIQUID_BENCHMARK(modem_demodsoft_ook,     "modemcf demodulate_soft, ook",     "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_OOK); }
LIQUID_BENCHMARK(modem_demodsoft_sqam32,  "modemcf demodulate_soft, sqam32",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_SQAM32); }
LIQUID_BENCHMARK(modem_demodsoft_sqam128, "modemcf demodulate_soft, sqam128", "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_SQAM128); }

// ASK
LIQUID_BENCHMARK(modem_demodsoft_ask2,    "modemcf demodulate_soft, ask2",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ASK2); }
LIQUID_BENCHMARK(modem_demodsoft_ask4,    "modemcf demodulate_soft, ask4",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ASK4); }
LIQUID_BENCHMARK(modem_demodsoft_ask8,    "modemcf demodulate_soft, ask8",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ASK8); }
LIQUID_BENCHMARK(modem_demodsoft_ask16,   "modemcf demodulate_soft, ask16",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ASK16); }

// PSK
LIQUID_BENCHMARK(modem_demodsoft_psk2,    "modemcf demodulate_soft, psk2",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_PSK2); }
LIQUID_BENCHMARK(modem_demodsoft_psk4,    "modemcf demodulate_soft, psk4",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_PSK4); }
LIQUID_BENCHMARK(modem_demodsoft_psk8,    "modemcf demodulate_soft, psk8",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_PSK8); }
LIQUID_BENCHMARK(modem_demodsoft_psk16,   "modemcf demodulate_soft, psk16",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_PSK16); }
LIQUID_BENCHMARK(modem_demodsoft_psk32,   "modemcf demodulate_soft, psk32",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_PSK32); }
LIQUID_BENCHMARK(modem_demodsoft_psk64,   "modemcf demodulate_soft, psk64",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_PSK64); }

// Differential PSK
LIQUID_BENCHMARK(modem_demodsoft_dpsk2,   "modemcf demodulate_soft, dpsk2",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_DPSK2); }
LIQUID_BENCHMARK(modem_demodsoft_dpsk4,   "modemcf demodulate_soft, dpsk4",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_DPSK4); }
LIQUID_BENCHMARK(modem_demodsoft_dpsk8,   "modemcf demodulate_soft, dpsk8",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_DPSK8); }
LIQUID_BENCHMARK(modem_demodsoft_dpsk16,  "modemcf demodulate_soft, dpsk16",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_DPSK16); }
LIQUID_BENCHMARK(modem_demodsoft_dpsk32,  "modemcf demodulate_soft, dpsk32",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_DPSK32); }
LIQUID_BENCHMARK(modem_demodsoft_dpsk64,  "modemcf demodulate_soft, dpsk64",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_DPSK64); }

// QAM
LIQUID_BENCHMARK(modem_demodsoft_qam4,    "modemcf demodulate_soft, qam4",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM4); }
LIQUID_BENCHMARK(modem_demodsoft_qam8,    "modemcf demodulate_soft, qam8",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM8); }
LIQUID_BENCHMARK(modem_demodsoft_qam16,   "modemcf demodulate_soft, qam16",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM16); }
LIQUID_BENCHMARK(modem_demodsoft_qam32,   "modemcf demodulate_soft, qam32",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM32); }
LIQUID_BENCHMARK(modem_demodsoft_qam64,   "modemcf demodulate_soft, qam64",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM64); }
LIQUID_BENCHMARK(modem_demodsoft_qam128,  "modemcf demodulate_soft, qam128",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM128); }
LIQUID_BENCHMARK(modem_demodsoft_qam256,  "modemcf demodulate_soft, qam256",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_QAM256); }

// APSK
LIQUID_BENCHMARK(modem_demodsoft_apsk4,   "modemcf demodulate_soft, apsk4",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK4); }
LIQUID_BENCHMARK(modem_demodsoft_apsk8,   "modemcf demodulate_soft, apsk8",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK8); }
LIQUID_BENCHMARK(modem_demodsoft_apsk16,  "modemcf demodulate_soft, apsk16",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK16); }
LIQUID_BENCHMARK(modem_demodsoft_apsk32,  "modemcf demodulate_soft, apsk32",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK32); }
LIQUID_BENCHMARK(modem_demodsoft_apsk64,  "modemcf demodulate_soft, apsk64",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK64); }
LIQUID_BENCHMARK(modem_demodsoft_apsk128, "modemcf demodulate_soft, apsk128", "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK128); }
LIQUID_BENCHMARK(modem_demodsoft_apsk256, "modemcf demodulate_soft, apsk256", "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_APSK256); }

// ARB
LIQUID_BENCHMARK(modem_demodsoft_arbV29,    "modemcf demodulate_soft, arbV29",    "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_V29); }
LIQUID_BENCHMARK(modem_demodsoft_arb16opt,  "modemcf demodulate_soft, arb16opt",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ARB16OPT); }
LIQUID_BENCHMARK(modem_demodsoft_arb32opt,  "modemcf demodulate_soft, arb32opt",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ARB32OPT); }
LIQUID_BENCHMARK(modem_demodsoft_arb64opt,  "modemcf demodulate_soft, arb64opt",  "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ARB64OPT); }
LIQUID_BENCHMARK(modem_demodsoft_arb128opt, "modemcf demodulate_soft, arb128opt", "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ARB128OPT); }
LIQUID_BENCHMARK(modem_demodsoft_arb256opt, "modemcf demodulate_soft, arb256opt", "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ARB256OPT); }
LIQUID_BENCHMARK(modem_demodsoft_arb64vt,   "modemcf demodulate_soft, arb64vt",   "modem,soft")
    { return modemcf_demodulate_soft_bench(num_iterations, LIQUID_MODEM_ARB64VT); }


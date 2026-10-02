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
float modemcf_demodulate_bench(unsigned long int _num_iterations,
                               modulation_scheme _ms)
{
    // initialize modulator
    modemcf demod = modemcf_create(_ms);

    unsigned long int i;
    // generate input vector to demodulate (spiral)
    float complex x[20];
    for (i=0; i<20; i++)
        x[i] = 0.07 * i * cexpf(_Complex_I*2*M_PI*0.1*i);
    unsigned int symbol_out;
    // start trials (20 demodulates per iteration; round down)
    unsigned long int n = _num_iterations / 20;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        modemcf_demodulate(demod, x[ 0], &symbol_out);
        modemcf_demodulate(demod, x[ 1], &symbol_out);
        modemcf_demodulate(demod, x[ 2], &symbol_out);
        modemcf_demodulate(demod, x[ 3], &symbol_out);
        modemcf_demodulate(demod, x[ 4], &symbol_out);
        modemcf_demodulate(demod, x[ 5], &symbol_out);
        modemcf_demodulate(demod, x[ 6], &symbol_out);
        modemcf_demodulate(demod, x[ 7], &symbol_out);
        modemcf_demodulate(demod, x[ 8], &symbol_out);
        modemcf_demodulate(demod, x[ 9], &symbol_out);
        modemcf_demodulate(demod, x[10], &symbol_out);
        modemcf_demodulate(demod, x[11], &symbol_out);
        modemcf_demodulate(demod, x[12], &symbol_out);
        modemcf_demodulate(demod, x[13], &symbol_out);
        modemcf_demodulate(demod, x[14], &symbol_out);
        modemcf_demodulate(demod, x[15], &symbol_out);
        modemcf_demodulate(demod, x[16], &symbol_out);
        modemcf_demodulate(demod, x[17], &symbol_out);
        modemcf_demodulate(demod, x[18], &symbol_out);
        modemcf_demodulate(demod, x[19], &symbol_out);
    }
    float extime = liquid_toc(timer);
    modemcf_destroy(demod);
    return extime;
}

// specific modems
LIQUID_BENCHMARK(modem_demodulate_bpsk,    "modemcf demodulate, bpsk",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_BPSK); }
LIQUID_BENCHMARK(modem_demodulate_qpsk,    "modemcf demodulate, qpsk",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QPSK); }
LIQUID_BENCHMARK(modem_demodulate_ook,     "modemcf demodulate, ook",     "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_OOK); }
LIQUID_BENCHMARK(modem_demodulate_sqam32,  "modemcf demodulate, sqam32",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_SQAM32); }
LIQUID_BENCHMARK(modem_demodulate_sqam128, "modemcf demodulate, sqam128", "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_SQAM128); }

// ASK
LIQUID_BENCHMARK(modem_demodulate_ask2,    "modemcf demodulate, ask2",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ASK2); }
LIQUID_BENCHMARK(modem_demodulate_ask4,    "modemcf demodulate, ask4",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ASK4); }
LIQUID_BENCHMARK(modem_demodulate_ask8,    "modemcf demodulate, ask8",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ASK8); }
LIQUID_BENCHMARK(modem_demodulate_ask16,   "modemcf demodulate, ask16",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ASK16); }

// PSK
LIQUID_BENCHMARK(modem_demodulate_psk2,    "modemcf demodulate, psk2",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_PSK2); }
LIQUID_BENCHMARK(modem_demodulate_psk4,    "modemcf demodulate, psk4",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_PSK4); }
LIQUID_BENCHMARK(modem_demodulate_psk8,    "modemcf demodulate, psk8",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_PSK8); }
LIQUID_BENCHMARK(modem_demodulate_psk16,   "modemcf demodulate, psk16",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_PSK16); }
LIQUID_BENCHMARK(modem_demodulate_psk32,   "modemcf demodulate, psk32",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_PSK32); }
LIQUID_BENCHMARK(modem_demodulate_psk64,   "modemcf demodulate, psk64",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_PSK64); }

// Differential PSK
LIQUID_BENCHMARK(modem_demodulate_dpsk2,   "modemcf demodulate, dpsk2",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_DPSK2); }
LIQUID_BENCHMARK(modem_demodulate_dpsk4,   "modemcf demodulate, dpsk4",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_DPSK4); }
LIQUID_BENCHMARK(modem_demodulate_dpsk8,   "modemcf demodulate, dpsk8",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_DPSK8); }
LIQUID_BENCHMARK(modem_demodulate_dpsk16,  "modemcf demodulate, dpsk16",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_DPSK16); }
LIQUID_BENCHMARK(modem_demodulate_dpsk32,  "modemcf demodulate, dpsk32",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_DPSK32); }
LIQUID_BENCHMARK(modem_demodulate_dpsk64,  "modemcf demodulate, dpsk64",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_DPSK64); }

// QAM
LIQUID_BENCHMARK(modem_demodulate_qam4,    "modemcf demodulate, qam4",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM4); }
LIQUID_BENCHMARK(modem_demodulate_qam8,    "modemcf demodulate, qam8",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM8); }
LIQUID_BENCHMARK(modem_demodulate_qam16,   "modemcf demodulate, qam16",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM16); }
LIQUID_BENCHMARK(modem_demodulate_qam32,   "modemcf demodulate, qam32",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM32); }
LIQUID_BENCHMARK(modem_demodulate_qam64,   "modemcf demodulate, qam64",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM64); }
LIQUID_BENCHMARK(modem_demodulate_qam128,  "modemcf demodulate, qam128",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM128); }
LIQUID_BENCHMARK(modem_demodulate_qam256,  "modemcf demodulate, qam256",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_QAM256); }

// APSK
LIQUID_BENCHMARK(modem_demodulate_apsk4,   "modemcf demodulate, apsk4",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK4); }
LIQUID_BENCHMARK(modem_demodulate_apsk8,   "modemcf demodulate, apsk8",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK8); }
LIQUID_BENCHMARK(modem_demodulate_apsk16,  "modemcf demodulate, apsk16",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK16); }
LIQUID_BENCHMARK(modem_demodulate_apsk32,  "modemcf demodulate, apsk32",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK32); }
LIQUID_BENCHMARK(modem_demodulate_apsk64,  "modemcf demodulate, apsk64",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK64); }
LIQUID_BENCHMARK(modem_demodulate_apsk128, "modemcf demodulate, apsk128", "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK128); }
LIQUID_BENCHMARK(modem_demodulate_apsk256, "modemcf demodulate, apsk256", "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_APSK256); }

// ARB
LIQUID_BENCHMARK(modem_demodulate_arbV29,    "modemcf demodulate, arbV29",    "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_V29); }
LIQUID_BENCHMARK(modem_demodulate_arb16opt,  "modemcf demodulate, arb16opt",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ARB16OPT); }
LIQUID_BENCHMARK(modem_demodulate_arb32opt,  "modemcf demodulate, arb32opt",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ARB32OPT); }
LIQUID_BENCHMARK(modem_demodulate_arb64opt,  "modemcf demodulate, arb64opt",  "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ARB64OPT); }
LIQUID_BENCHMARK(modem_demodulate_arb128opt, "modemcf demodulate, arb128opt", "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ARB128OPT); }
LIQUID_BENCHMARK(modem_demodulate_arb256opt, "modemcf demodulate, arb256opt", "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ARB256OPT); }
LIQUID_BENCHMARK(modem_demodulate_arb64vt,   "modemcf demodulate, arb64vt",   "modem")
    { return modemcf_demodulate_bench(num_iterations, LIQUID_MODEM_ARB64VT); }


/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.h"

#define MODEM_DEMODSOFT_BENCH_API(MS)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ modem_demodulate_soft_bench(_start, _finish, _num_iterations, MS); }

// Helper function to keep code base small
void modem_demodulate_soft_bench(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 modulation_scheme _ms)
{
    // normalize number of iterations
    switch (_ms) {
    case LIQUID_MODEM_UNKNOWN:
        fprintf(stderr,"error: modem_modulate_bench(), unknown modem scheme\n");
        exit(1);
    case LIQUID_MODEM_BPSK:     *_num_iterations *= 2;      break;
    case LIQUID_MODEM_QPSK:     *_num_iterations *= 2;      break;
    case LIQUID_MODEM_OOK:      *_num_iterations *= 2;      break;
    case LIQUID_MODEM_SQAM32:   *_num_iterations /= 10;     break;
    case LIQUID_MODEM_SQAM128:  *_num_iterations /= 20;     break;
    case LIQUID_MODEM_V29:      *_num_iterations /= 16;     break;
    case LIQUID_MODEM_ARB16OPT: *_num_iterations /= 16;     break;
    case LIQUID_MODEM_ARB32OPT: *_num_iterations /= 32;     break;
    case LIQUID_MODEM_ARB64VT:  *_num_iterations /= 64;     break;
    default:;
        *_num_iterations /= 8;
    }

    if (*_num_iterations < 1) *_num_iterations = 1;


    // initialize modulator
    modem demod = modem_create(_ms);
    unsigned int bps = modem_get_bps(demod);

    unsigned long int i;

    // generate input vector to demodulate (spiral)
    float complex x[20];
    for (i=0; i<20; i++)
        x[i] = 0.07 * i * cexpf(_Complex_I*2*M_PI*0.1*i);

    unsigned int symbol_out;
    unsigned char soft_bits[bps];

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        modem_demodulate_soft(demod, x[ 0], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 1], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 2], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 3], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 4], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 5], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 6], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 7], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 8], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[ 9], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[10], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[11], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[12], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[13], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[14], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[15], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[16], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[17], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[18], &symbol_out, soft_bits);
        modem_demodulate_soft(demod, x[19], &symbol_out, soft_bits);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 20;

    modem_destroy(demod);
}

// specific modems
void benchmark_demodsoft_bpsk    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_BPSK)
void benchmark_demodsoft_qpsk    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QPSK)
void benchmark_demodsoft_ook     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_OOK)
void benchmark_demodsoft_sqam32  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_SQAM32)
void benchmark_demodsoft_sqam128 MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_SQAM128)

// ASK
void benchmark_demodsoft_ask2    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK2)
void benchmark_demodsoft_ask4    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK4)
void benchmark_demodsoft_ask8    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK8)
void benchmark_demodsoft_ask16   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK16)

// PSK
void benchmark_demodsoft_psk2    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK2)
void benchmark_demodsoft_psk4    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK4)
void benchmark_demodsoft_psk8    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK8)
void benchmark_demodsoft_psk16   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK16)
void benchmark_demodsoft_psk32   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK32)
void benchmark_demodsoft_psk64   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK64)

// Differential PSK
void benchmark_demodsoft_dpsk2   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK2)
void benchmark_demodsoft_dpsk4   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK4)
void benchmark_demodsoft_dpsk8   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK8)
void benchmark_demodsoft_dpsk16  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK16)
void benchmark_demodsoft_dpsk32  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK32)
void benchmark_demodsoft_dpsk64  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK64)

// QAM
void benchmark_demodsoft_qam4    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM4)
void benchmark_demodsoft_qam8    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM8)
void benchmark_demodsoft_qam16   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM16)
void benchmark_demodsoft_qam32   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM32)
void benchmark_demodsoft_qam64   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM64)
void benchmark_demodsoft_qam128  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM128)
void benchmark_demodsoft_qam256  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM256)

// APSK
void benchmark_demodsoft_apsk4   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK4)
void benchmark_demodsoft_apsk8   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK8)
void benchmark_demodsoft_apsk16  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK16)
void benchmark_demodsoft_apsk32  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK32)
void benchmark_demodsoft_apsk64  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK64)
void benchmark_demodsoft_apsk128 MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK128)
void benchmark_demodsoft_apsk256 MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK256)

// ARB
void benchmark_demodsoft_arbV29    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_V29)
void benchmark_demodsoft_arb16opt  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB16OPT)
void benchmark_demodsoft_arb32opt  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB32OPT)
void benchmark_demodsoft_arb64opt  MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB64OPT)
void benchmark_demodsoft_arb128opt MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB128OPT)
void benchmark_demodsoft_arb256opt MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB256OPT)
void benchmark_demodsoft_arb64vt   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB64VT)


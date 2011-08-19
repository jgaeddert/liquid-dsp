/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include "liquid.h"

#define MODEM_DEMODSOFT_BENCH_API(MS,BPS)   \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ modem_demodulate_soft_bench(_start, _finish, _num_iterations, MS, BPS); }

// Helper function to keep code base small
void modem_demodulate_soft_bench(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 modulation_scheme _ms,
                                 unsigned int _bps)
{
    // normalize number of iterations
    switch (_ms) {
    case LIQUID_MODEM_UNKNOWN:
        fprintf(stderr,"error: modem_modulate_bench(), unknown modem scheme\n");
        exit(1);
    case LIQUID_MODEM_PSK:      *_num_iterations /= 2;      break;
    case LIQUID_MODEM_DPSK:     *_num_iterations /= 2;      break;
    case LIQUID_MODEM_ASK:      *_num_iterations /= 2;      break;
    case LIQUID_MODEM_QAM:      *_num_iterations /= _bps-1; break;
    case LIQUID_MODEM_APSK:     *_num_iterations /= _bps+1; break;
    case LIQUID_MODEM_ARB:      *_num_iterations /= 10;     break;
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
    }
    if (*_num_iterations < 1) *_num_iterations = 1;


    // initialize modulator
    modem demod = modem_create(_ms, _bps);
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
void benchmark_demodsoft_bpsk       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_BPSK, 1)
void benchmark_demodsoft_qpsk       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QPSK, 2)
void benchmark_demodsoft_ook        MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_OOK,  1)
void benchmark_demodsoft_sqam32     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_SQAM32,5)
void benchmark_demodsoft_sqam128    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_SQAM128,7)

// ASK
void benchmark_demodsoft_ask2       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK, 1)
void benchmark_demodsoft_ask4       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK, 2)
void benchmark_demodsoft_ask8       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK, 3)
void benchmark_demodsoft_ask16      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ASK, 4)

// PSK
void benchmark_demodsoft_psk2       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK, 1)
void benchmark_demodsoft_psk4       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK, 2)
void benchmark_demodsoft_psk8       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK, 3)
void benchmark_demodsoft_psk16      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK, 4)
void benchmark_demodsoft_psk32      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK, 5)
void benchmark_demodsoft_psk64      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_PSK, 6)

// Differential PSK
void benchmark_demodsoft_dpsk2      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK, 1)
void benchmark_demodsoft_dpsk4      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK, 2)
void benchmark_demodsoft_dpsk8      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK, 3)
void benchmark_demodsoft_dpsk16     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK, 4)
void benchmark_demodsoft_dpsk32     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK, 5)
void benchmark_demodsoft_dpsk64     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_DPSK, 6)

// QAM
void benchmark_demodsoft_qam4       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 2)
void benchmark_demodsoft_qam8       MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 3)
void benchmark_demodsoft_qam16      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 4)
void benchmark_demodsoft_qam32      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 5)
void benchmark_demodsoft_qam64      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 6)
void benchmark_demodsoft_qam128     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 7)
void benchmark_demodsoft_qam256     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_QAM, 8)

// A-PSK
void benchmark_demodsoft_apsk4      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 2)
void benchmark_demodsoft_apsk8      MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 3)
void benchmark_demodsoft_apsk16     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 4)
void benchmark_demodsoft_apsk32     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 5)
void benchmark_demodsoft_apsk64     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 6)
void benchmark_demodsoft_apsk128    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 7)
void benchmark_demodsoft_apsk256    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_APSK, 8)

// ARB
void benchmark_demodsoft_arbV29     MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_V29,     4);
void benchmark_demodsoft_arb16opt   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB16OPT,4);
void benchmark_demodsoft_arb32opt   MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB32OPT,5);
void benchmark_demodsoft_arb64vt    MODEM_DEMODSOFT_BENCH_API(LIQUID_MODEM_ARB64VT, 6);


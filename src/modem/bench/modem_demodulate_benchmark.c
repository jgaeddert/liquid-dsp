/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#define MODEM_DEMODULATE_BENCH_API(MS)  \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ modem_demodulate_bench(_start, _finish, _num_iterations, MS); }

// Helper function to keep code base small
void modem_demodulate_bench(struct rusage *_start,
                            struct rusage *_finish,
                            unsigned long int *_num_iterations,
                            modulation_scheme _ms)
{
    // normalize number of iterations
    switch (_ms) {
    case LIQUID_MODEM_UNKNOWN:
        fprintf(stderr,"error: modem_demodulate_bench(), unknown modem scheme\n");
        exit(1);
    case LIQUID_MODEM_BPSK:     *_num_iterations *= 2;      break;
    case LIQUID_MODEM_QPSK:     *_num_iterations *= 2;      break;
    case LIQUID_MODEM_OOK:      *_num_iterations *= 2;      break;
    case LIQUID_MODEM_SQAM32:   *_num_iterations /= 10;     break;
    case LIQUID_MODEM_SQAM128:  *_num_iterations /= 20;     break;
    case LIQUID_MODEM_V29:      *_num_iterations /= 16;     break;
    case LIQUID_MODEM_ARB16OPT: *_num_iterations /= 8;      break;
    case LIQUID_MODEM_ARB32OPT: *_num_iterations /= 16;     break;
    case LIQUID_MODEM_ARB64OPT: *_num_iterations /= 32;     break;
    case LIQUID_MODEM_ARB128OPT: *_num_iterations /= 64;    break;
    case LIQUID_MODEM_ARB256OPT: *_num_iterations /= 128;   break;
    case LIQUID_MODEM_ARB64VT:  *_num_iterations /= 64;     break;
    default:;
        *_num_iterations /= 8;
    }

    if (*_num_iterations < 1) *_num_iterations = 1;


    // initialize modulator
    modem demod = modem_create(_ms);

    unsigned long int i;

    // generate input vector to demodulate (spiral)
    float complex x[20];
    for (i=0; i<20; i++)
        x[i] = 0.07 * i * cexpf(_Complex_I*2*M_PI*0.1*i);

    unsigned int symbol_out;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        modem_demodulate(demod, x[ 0], &symbol_out);
        modem_demodulate(demod, x[ 1], &symbol_out);
        modem_demodulate(demod, x[ 2], &symbol_out);
        modem_demodulate(demod, x[ 3], &symbol_out);
        modem_demodulate(demod, x[ 4], &symbol_out);
        modem_demodulate(demod, x[ 5], &symbol_out);
        modem_demodulate(demod, x[ 6], &symbol_out);
        modem_demodulate(demod, x[ 7], &symbol_out);
        modem_demodulate(demod, x[ 8], &symbol_out);
        modem_demodulate(demod, x[ 9], &symbol_out);
        modem_demodulate(demod, x[10], &symbol_out);
        modem_demodulate(demod, x[11], &symbol_out);
        modem_demodulate(demod, x[12], &symbol_out);
        modem_demodulate(demod, x[13], &symbol_out);
        modem_demodulate(demod, x[14], &symbol_out);
        modem_demodulate(demod, x[15], &symbol_out);
        modem_demodulate(demod, x[16], &symbol_out);
        modem_demodulate(demod, x[17], &symbol_out);
        modem_demodulate(demod, x[18], &symbol_out);
        modem_demodulate(demod, x[19], &symbol_out);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 20;

    modem_destroy(demod);
}

// specific modems
void benchmark_demodulate_bpsk    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_BPSK)
void benchmark_demodulate_qpsk    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QPSK)
void benchmark_demodulate_ook     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_OOK)
void benchmark_demodulate_sqam32  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_SQAM32)
void benchmark_demodulate_sqam128 MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_SQAM128)

// ASK
void benchmark_demodulate_ask2    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK2)
void benchmark_demodulate_ask4    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK4)
void benchmark_demodulate_ask8    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK8)
void benchmark_demodulate_ask16   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK16)

// PSK
void benchmark_demodulate_psk2    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK2)
void benchmark_demodulate_psk4    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK4)
void benchmark_demodulate_psk8    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK8)
void benchmark_demodulate_psk16   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK16)
void benchmark_demodulate_psk32   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK32)
void benchmark_demodulate_psk64   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK64)

// Differential PSK
void benchmark_demodulate_dpsk2   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK2)
void benchmark_demodulate_dpsk4   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK4)
void benchmark_demodulate_dpsk8   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK8)
void benchmark_demodulate_dpsk16  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK16)
void benchmark_demodulate_dpsk32  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK32)
void benchmark_demodulate_dpsk64  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK64)

// QAM
void benchmark_demodulate_qam4    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM4)
void benchmark_demodulate_qam8    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM8)
void benchmark_demodulate_qam16   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM16)
void benchmark_demodulate_qam32   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM32)
void benchmark_demodulate_qam64   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM64)
void benchmark_demodulate_qam128  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM128)
void benchmark_demodulate_qam256  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM256)

// APSK
void benchmark_demodulate_apsk4   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK4)
void benchmark_demodulate_apsk8   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK8)
void benchmark_demodulate_apsk16  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK16)
void benchmark_demodulate_apsk32  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK32)
void benchmark_demodulate_apsk64  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK64)
void benchmark_demodulate_apsk128 MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK128)
void benchmark_demodulate_apsk256 MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK256)

// ARB
void benchmark_demodulate_arbV29    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_V29)
void benchmark_demodulate_arb16opt  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ARB16OPT)
void benchmark_demodulate_arb32opt  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ARB32OPT)
void benchmark_demodulate_arb64opt  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ARB64OPT)
void benchmark_demodulate_arb128opt MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ARB128OPT)
void benchmark_demodulate_arb256opt MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ARB256OPT)
void benchmark_demodulate_arb64vt   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ARB64VT)


/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
#include <sys/resource.h>
#include "liquid.h"

#define MODEM_DEMODULATE_BENCH_API(MS,BPS)  \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ modem_demodulate_bench(_start, _finish, _num_iterations, MS, BPS); }

// Helper function to keep code base small
void modem_demodulate_bench(struct rusage *_start,
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
    case LIQUID_MODEM_PSK:      *_num_iterations *= 10;     break;
    case LIQUID_MODEM_DPSK:     *_num_iterations *= 10;     break;
    case LIQUID_MODEM_ASK:      *_num_iterations /= 1;      break;
    case LIQUID_MODEM_QAM:      *_num_iterations *= 2;      break;
    case LIQUID_MODEM_APSK:     *_num_iterations /= 2;      break;
    case LIQUID_MODEM_ARB:      *_num_iterations /= 1;      break;
    case LIQUID_MODEM_BPSK:     *_num_iterations /= 1;      break;
    case LIQUID_MODEM_QPSK:     *_num_iterations /= 1;      break;
    case LIQUID_MODEM_OOK:      *_num_iterations /= 1;      break;
    case LIQUID_MODEM_SQAM32:   *_num_iterations /= 4;      break;
    case LIQUID_MODEM_SQAM128:  *_num_iterations /= 8;      break;
    case LIQUID_MODEM_ARB16OPT: *_num_iterations /= 1;      break;
    case LIQUID_MODEM_ARB64VT:  *_num_iterations /= 1;      break;
    default:;
    }
    if (*_num_iterations < 1) *_num_iterations = 1;


    // initialize modulator
    modem demod = modem_create(_ms, _bps);

    float complex x = 1.0f;
    unsigned int symbol_out;
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        modem_demodulate(demod, x, &symbol_out);
        modem_demodulate(demod, x, &symbol_out);
        modem_demodulate(demod, x, &symbol_out);
        modem_demodulate(demod, x, &symbol_out);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    modem_destroy(demod);
}

// specific modems
void benchmark_demodulate_bpsk      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_BPSK, 1)
void benchmark_demodulate_qpsk      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QPSK, 2)
void benchmark_demodulate_ook       MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_OOK,  1)
void benchmark_demodulate_sqam32    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_SQAM32,5)
void benchmark_demodulate_sqam128   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_SQAM128,7)

// ASK
void benchmark_demodulate_ask2      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK, 1)
void benchmark_demodulate_ask4      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK, 2)
void benchmark_demodulate_ask8      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK, 3)
void benchmark_demodulate_ask16     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_ASK, 4)

// PSK
void benchmark_demodulate_psk2      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK, 1)
void benchmark_demodulate_psk4      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK, 2)
void benchmark_demodulate_psk8      MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK, 3)
void benchmark_demodulate_psk16     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK, 4)
void benchmark_demodulate_psk32     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK, 5)
void benchmark_demodulate_psk64     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_PSK, 6)

// Differential PSK
void benchmark_demodulate_dpsk2     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK, 1)
void benchmark_demodulate_dpsk4     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK, 2)
void benchmark_demodulate_dpsk8     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK, 3)
void benchmark_demodulate_dpsk16    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK, 4)
void benchmark_demodulate_dpsk32    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK, 5)
void benchmark_demodulate_dpsk64    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_DPSK, 6)

// QAM
void benchmark_demodulate_qam4     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 2)
void benchmark_demodulate_qam8     MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 3)
void benchmark_demodulate_qam16    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 4)
void benchmark_demodulate_qam32    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 5)
void benchmark_demodulate_qam64    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 6)
void benchmark_demodulate_qam128   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 7)
void benchmark_demodulate_qam256   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_QAM, 8)

// A-PSK
void benchmark_demodulate_apsk4    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 2)
void benchmark_demodulate_apsk8    MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 3)
void benchmark_demodulate_apsk16   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 4)
void benchmark_demodulate_apsk32   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 5)
void benchmark_demodulate_apsk64   MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 6)
void benchmark_demodulate_apsk128  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 7)
void benchmark_demodulate_apsk256  MODEM_DEMODULATE_BENCH_API(LIQUID_MODEM_APSK, 8)


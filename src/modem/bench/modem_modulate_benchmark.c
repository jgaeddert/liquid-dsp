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

#define MODEM_MODULATE_BENCH_API(MS,BPS)    \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ modem_modulate_bench(_start, _finish, _num_iterations, MS, BPS); }

// Helper function to keep code base small
void modem_modulate_bench(struct rusage *_start,
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
    case LIQUID_MODEM_PSK:      *_num_iterations *= 100;    break;
    case LIQUID_MODEM_DPSK:     *_num_iterations *= 16;     break;
    case LIQUID_MODEM_ASK:      *_num_iterations *= 64;     break;
    case LIQUID_MODEM_QAM:      *_num_iterations *= 32;     break;
    case LIQUID_MODEM_APSK:     *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB:      *_num_iterations *= 1;      break;
    case LIQUID_MODEM_BPSK:     *_num_iterations *= 64;     break;
    case LIQUID_MODEM_QPSK:     *_num_iterations *= 64;     break;
    case LIQUID_MODEM_OOK:      *_num_iterations *= 64;     break;
    case LIQUID_MODEM_SQAM32:   *_num_iterations *= 16;     break;
    case LIQUID_MODEM_SQAM128:  *_num_iterations *= 16;     break;
    case LIQUID_MODEM_V29:      *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB16OPT: *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB32OPT: *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB64OPT: *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB128OPT: *_num_iterations *= 100;   break;
    case LIQUID_MODEM_ARB256OPT: *_num_iterations *= 100;   break;
    case LIQUID_MODEM_ARB64VT:  *_num_iterations *= 100;    break;
    default:;
    }
    if (*_num_iterations < 1) *_num_iterations = 1;


    // initialize modulator
    modem mod = modem_create(_ms, _bps);

    float complex x;
    unsigned int symbol_in = 0;
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        modem_modulate(mod, symbol_in, &x);
        modem_modulate(mod, symbol_in, &x);
        modem_modulate(mod, symbol_in, &x);
        modem_modulate(mod, symbol_in, &x);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    modem_destroy(mod);
}

// specific modems
void benchmark_modulate_bpsk    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_BPSK, 1)
void benchmark_modulate_qpsk    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QPSK, 2)
void benchmark_modulate_ook     MODEM_MODULATE_BENCH_API(LIQUID_MODEM_OOK,  1)
void benchmark_modulate_sqam32  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_SQAM32,5)
void benchmark_modulate_sqam128 MODEM_MODULATE_BENCH_API(LIQUID_MODEM_SQAM128,7)

// ASK
void benchmark_modulate_ask2    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK, 1)
void benchmark_modulate_ask4    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK, 2)
void benchmark_modulate_ask8    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK, 3)
void benchmark_modulate_ask16   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK, 4)

// PSK
void benchmark_modulate_psk2    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK, 1)
void benchmark_modulate_psk4    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK, 2)
void benchmark_modulate_psk8    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK, 3)
void benchmark_modulate_psk16   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK, 4)
void benchmark_modulate_psk32   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK, 5)
void benchmark_modulate_psk64   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK, 6)

// Differential PSK
void benchmark_modulate_dpsk2   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK, 1)
void benchmark_modulate_dpsk4   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK, 2)
void benchmark_modulate_dpsk8   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK, 3)
void benchmark_modulate_dpsk16  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK, 4)
void benchmark_modulate_dpsk32  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK, 5)
void benchmark_modulate_dpsk64  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK, 6)

// QAM
void benchmark_modulate_qam4    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 2)
void benchmark_modulate_qam8    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 3)
void benchmark_modulate_qam16   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 4)
void benchmark_modulate_qam32   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 5)
void benchmark_modulate_qam64   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 6)
void benchmark_modulate_qam128  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 7)
void benchmark_modulate_qam256  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM, 8)

// APSK
void benchmark_modulate_apsk4   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  2)
void benchmark_modulate_apsk8   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  3)
void benchmark_modulate_apsk16  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  4)
void benchmark_modulate_apsk32  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  5)
void benchmark_modulate_apsk64  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  6)
void benchmark_modulate_apsk128 MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  7)
void benchmark_modulate_apsk256 MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK,  8)

// ARB
void benchmark_modulate_arbV29    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_V29,       4);
void benchmark_modulate_arb16opt  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB16OPT,  4);
void benchmark_modulate_arb32opt  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB32OPT,  5);
void benchmark_modulate_arb64opt  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB64OPT,  6);
void benchmark_modulate_arb128opt MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB128OPT, 7);
void benchmark_modulate_arb256opt MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB256OPT, 8);
void benchmark_modulate_arb64vt   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB64VT,   6);


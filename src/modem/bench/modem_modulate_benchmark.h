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

#ifndef __LIQUID_MODEM_BENCH_H__
#define __LIQUID_MODEM_BENCH_H__

#include <sys/resource.h>
#include "liquid.h"

#define MODEM_MODULATE_BENCH_API(MS,BPS) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ modem_modulate_bench(_start, _finish, _num_iterations, MS, BPS); }

// Helper function to keep code base small
void modem_modulate_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    modulation_scheme _ms,
    unsigned int _bps)
{
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
void benchmark_modulate_bpsk    MODEM_MODULATE_BENCH_API(MOD_BPSK, 1)
void benchmark_modulate_qpsk    MODEM_MODULATE_BENCH_API(MOD_QPSK, 2)

// ASK
void benchmark_modulate_ask2    MODEM_MODULATE_BENCH_API(MOD_ASK, 1)
void benchmark_modulate_ask4    MODEM_MODULATE_BENCH_API(MOD_ASK, 2)
void benchmark_modulate_ask8    MODEM_MODULATE_BENCH_API(MOD_ASK, 3)
void benchmark_modulate_ask16   MODEM_MODULATE_BENCH_API(MOD_ASK, 4)

// PSK
void benchmark_modulate_psk2    MODEM_MODULATE_BENCH_API(MOD_PSK, 1)
void benchmark_modulate_psk4    MODEM_MODULATE_BENCH_API(MOD_PSK, 2)
void benchmark_modulate_psk8    MODEM_MODULATE_BENCH_API(MOD_PSK, 3)
void benchmark_modulate_psk16   MODEM_MODULATE_BENCH_API(MOD_PSK, 4)
void benchmark_modulate_psk32   MODEM_MODULATE_BENCH_API(MOD_PSK, 5)
void benchmark_modulate_psk64   MODEM_MODULATE_BENCH_API(MOD_PSK, 6)

// Differential PSK
void benchmark_modulate_dpsk2   MODEM_MODULATE_BENCH_API(MOD_DPSK, 1)
void benchmark_modulate_dpsk4   MODEM_MODULATE_BENCH_API(MOD_DPSK, 2)
void benchmark_modulate_dpsk8   MODEM_MODULATE_BENCH_API(MOD_DPSK, 3)
void benchmark_modulate_dpsk16  MODEM_MODULATE_BENCH_API(MOD_DPSK, 4)
void benchmark_modulate_dpsk32  MODEM_MODULATE_BENCH_API(MOD_DPSK, 5)
void benchmark_modulate_dpsk64  MODEM_MODULATE_BENCH_API(MOD_DPSK, 6)

// QAM
void benchmark_modulate_qam4   MODEM_MODULATE_BENCH_API(MOD_QAM, 2)
void benchmark_modulate_qam8   MODEM_MODULATE_BENCH_API(MOD_QAM, 3)
void benchmark_modulate_qam16  MODEM_MODULATE_BENCH_API(MOD_QAM, 4)
void benchmark_modulate_qam32  MODEM_MODULATE_BENCH_API(MOD_QAM, 5)
void benchmark_modulate_qam64  MODEM_MODULATE_BENCH_API(MOD_QAM, 6)
void benchmark_modulate_qam128 MODEM_MODULATE_BENCH_API(MOD_QAM, 7)
void benchmark_modulate_qam256 MODEM_MODULATE_BENCH_API(MOD_QAM, 8)

// APSK
void benchmark_modulate_apsk32 MODEM_MODULATE_BENCH_API(MOD_APSK32, 5)

#endif // __LIQUID_MODEM_BENCH_H__


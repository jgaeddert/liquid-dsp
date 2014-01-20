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
#include <string.h>
#include <sys/resource.h>

#include "liquid.internal.h"

#define CRC_BENCH_API(CRC,N)                \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ crc_bench(_start, _finish, _num_iterations, CRC, N); }

// Helper function to keep code base small
void crc_bench(struct rusage *_start,
               struct rusage *_finish,
               unsigned long int *_num_iterations,
               crc_scheme _crc,
               unsigned int _n)
{
    // normalize number of iterations
    if (_crc != LIQUID_CRC_CHECKSUM)
        *_num_iterations /= 8;

    unsigned long int i;

    // create arrays
    unsigned char msg[_n];
    unsigned int key;

    // initialze message
    for (i=0; i<_n; i++)
        msg[i] = rand() & 0xff;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        key = crc_generate_key(_crc, msg, _n);
        key = crc_generate_key(_crc, msg, _n);
        key = crc_generate_key(_crc, msg, _n);
        key = crc_generate_key(_crc, msg, _n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

//
// BENCHMARKS
//
void benchmark_crc_checksum_n256    CRC_BENCH_API(LIQUID_CRC_CHECKSUM,  256)

void benchmark_crc_crc8_n256        CRC_BENCH_API(LIQUID_CRC_8,         256)
void benchmark_crc_crc16_n256       CRC_BENCH_API(LIQUID_CRC_16,        256)
void benchmark_crc_crc24_n256       CRC_BENCH_API(LIQUID_CRC_24,        256)
void benchmark_crc_crc32_n256       CRC_BENCH_API(LIQUID_CRC_32,        256)


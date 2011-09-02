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
#include <string.h>
#include <sys/resource.h>

#include "liquid.internal.h"

#define PACKETIZER_DECODE_BENCH_API(N, CRC, FEC0, FEC1) \
(   struct rusage *_start,                              \
    struct rusage *_finish,                             \
    unsigned long int *_num_iterations)                 \
{ packetizer_decode_bench(_start, _finish, _num_iterations, N, CRC, FEC0, FEC1); }

// Helper function to keep code base small
void packetizer_decode_bench(struct rusage * _start,
                             struct rusage * _finish,
                             unsigned long int *_num_iterations,
                             unsigned int _n,
                             crc_scheme _crc,
                             fec_scheme _fec0,
                             fec_scheme _fec1)
{
    // adjust number of iterations
    *_num_iterations /= 10;

    // create packet generator
    packetizer p = packetizer_create(_n, _crc, _fec0, _fec1);

    unsigned int k = packetizer_compute_enc_msg_len(_n,_crc,_fec0,_fec1);
    unsigned char msg_org[_n];
    unsigned char msg_rec[ k];
    unsigned char msg_dec[_n];

    // initialize data
    unsigned long int i;
    for (i=0; i<_n; i++) {
        msg_org[i] = rand() & 0xff;
        msg_dec[i] = 0x00;
    }

    // encode packet
    packetizer_encode(p, msg_org, msg_rec);

    // initialize original data message
    for (i=0; i<_n; i++)
        msg_org[i] = rand() % 256;

    int crc_pass;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // decode packet
        crc_pass = packetizer_decode(p, msg_rec, msg_rec);
        crc_pass = packetizer_decode(p, msg_rec, msg_rec);
        crc_pass = packetizer_decode(p, msg_rec, msg_rec);
        crc_pass = packetizer_decode(p, msg_rec, msg_rec);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up allocated objects
    packetizer_destroy(p);
}


//
// BENCHMARKS
//
void benchmark_packetizer_n64_0_0   PACKETIZER_DECODE_BENCH_API(64, LIQUID_CRC_32, LIQUID_FEC_NONE, LIQUID_FEC_NONE)


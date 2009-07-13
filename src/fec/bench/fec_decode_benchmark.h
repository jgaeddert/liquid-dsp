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

#ifndef __LIQUID_FEC_DECODE_BENCHMARK_H__
#define __LIQUID_FEC_DECODE_BENCHMARK_H__

#include <sys/resource.h>
#include <stdlib.h>

#include "liquid.h"

#define FEC_DECODE_BENCH_API(FS,N, OPT) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fec_decode_bench(_start, _finish, _num_iterations, FS, N, OPT); }

// Helper function to keep code base small
void fec_decode_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    fec_scheme _fs,
    unsigned int _n,
    void * _opts)
{
    // normalize number of iterations
    *_num_iterations /= _n;

    // generate fec object
    fec q = fec_create(_fs,_opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // decoded message
    unsigned char msg_dec[_n];      // decoded message

    // initialze message
    unsigned long int i;
    for (i=0; i<_n; i++)
        msg[i] = rand() & 0xff;

    // encode message
    fec_encode(q,_n,msg,msg_enc);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_decode(q,_n,msg_enc,msg_dec);
        fec_decode(q,_n,msg_enc,msg_dec);
        fec_decode(q,_n,msg_enc,msg_dec);
        fec_decode(q,_n,msg_enc,msg_dec);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up objects
    fec_destroy(q);
}

//
// BENCHMARKS
//
void benchmark_fec_dec_rep3_n4          FEC_DECODE_BENCH_API(FEC_REP3, 4,   NULL)
void benchmark_fec_dec_rep3_n16         FEC_DECODE_BENCH_API(FEC_REP3, 16,  NULL)
void benchmark_fec_dec_rep3_n64         FEC_DECODE_BENCH_API(FEC_REP3, 64,  NULL)
void benchmark_fec_dec_rep3_n256        FEC_DECODE_BENCH_API(FEC_REP3, 256, NULL)

void benchmark_fec_dec_hamming74_n4     FEC_DECODE_BENCH_API(FEC_HAMMING74, 4,   NULL)
void benchmark_fec_dec_hamming74_n16    FEC_DECODE_BENCH_API(FEC_HAMMING74, 16,  NULL)
void benchmark_fec_dec_hamming74_n64    FEC_DECODE_BENCH_API(FEC_HAMMING74, 64,  NULL)
void benchmark_fec_dec_hamming74_n256   FEC_DECODE_BENCH_API(FEC_HAMMING74, 256, NULL)

void benchmark_fec_dec_conv27_n4        FEC_DECODE_BENCH_API(FEC_CONV_V27,  4,      NULL)
void benchmark_fec_dec_conv27_n16       FEC_DECODE_BENCH_API(FEC_CONV_V27,  16,     NULL)
void benchmark_fec_dec_conv27_n64       FEC_DECODE_BENCH_API(FEC_CONV_V27,  64,     NULL)
void benchmark_fec_dec_conv27_n256      FEC_DECODE_BENCH_API(FEC_CONV_V27,  256,    NULL)

#endif // __LIQUID_FEC_DECODE_BENCHMARK_H__


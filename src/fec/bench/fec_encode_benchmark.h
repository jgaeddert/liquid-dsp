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

#ifndef __LIQUID_FEC_ENCODE_BENCHMARK_H__
#define __LIQUID_FEC_ENCODE_BENCHMARK_H__

#include <sys/resource.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define FEC_ENCODE_BENCH_API(FS,N, OPT) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fec_encode_bench(_start, _finish, _num_iterations, FS, N, OPT); }

// Helper function to keep code base small
void fec_encode_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    fec_scheme _fs,
    unsigned int _n,
    void * _opts)
{
#if HAVE_FEC_H == 0
    if ( _fs == FEC_CONV_V27    ||
         _fs == FEC_CONV_V29    ||
         _fs == FEC_CONV_V39    ||
         _fs == FEC_CONV_V615   ||
         _fs == FEC_CONV_V27P23 ||
         _fs == FEC_CONV_V27P34 ||
         _fs == FEC_CONV_V27P45 ||
         _fs == FEC_CONV_V27P56 ||
         _fs == FEC_CONV_V27P67 ||
         _fs == FEC_CONV_V27P78 ||
         _fs == FEC_CONV_V29P23 ||
         _fs == FEC_CONV_V29P34 ||
         _fs == FEC_CONV_V29P45 ||
         _fs == FEC_CONV_V29P56 ||
         _fs == FEC_CONV_V29P67 ||
         _fs == FEC_CONV_V29P78)
    {
        fprintf(stderr,"warning: convolutional codes unavailable (install libfec)\n");
        getrusage(RUSAGE_SELF, _start);
        memmove((void*)_finish,(void*)_start,sizeof(struct rusage));
        return;
    }
#endif

    // normalize number of iterations
    if (_fs > 3)
        *_num_iterations /= _n;

    // generate fec object
    fec q = fec_create(_fs,_opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // encoded message

    // initialze message
    unsigned long int i;
    for (i=0; i<_n; i++) {
        msg[i] = rand() & 0xff;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_encode(q,_n,msg,msg_enc);
        fec_encode(q,_n,msg,msg_enc);
        fec_encode(q,_n,msg,msg_enc);
        fec_encode(q,_n,msg,msg_enc);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up objects
    fec_destroy(q);
}

//
// BENCHMARKS
//
void benchmark_fec_enc_rep3_n64         FEC_ENCODE_BENCH_API(FEC_REP3,      64,  NULL)
void benchmark_fec_enc_hamming74_n64    FEC_ENCODE_BENCH_API(FEC_HAMMING74, 64,  NULL)

void benchmark_fec_enc_conv27_n64       FEC_ENCODE_BENCH_API(FEC_CONV_V27,  64,  NULL)
void benchmark_fec_enc_conv29_n64       FEC_ENCODE_BENCH_API(FEC_CONV_V29,  64,  NULL)
void benchmark_fec_enc_conv39_n64       FEC_ENCODE_BENCH_API(FEC_CONV_V39,  64,  NULL)
void benchmark_fec_enc_conv615_n64      FEC_ENCODE_BENCH_API(FEC_CONV_V615, 64,  NULL)

void benchmark_fec_enc_conv27p23_n64    FEC_ENCODE_BENCH_API(FEC_CONV_V27P23,64, NULL)
void benchmark_fec_enc_conv27p34_n64    FEC_ENCODE_BENCH_API(FEC_CONV_V27P34,64, NULL)
void benchmark_fec_enc_conv27p45_n64    FEC_ENCODE_BENCH_API(FEC_CONV_V27P45,64, NULL)

#endif // __LIQUID_FEC_ENCODE_BENCHMARK_H__


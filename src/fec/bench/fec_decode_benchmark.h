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

#ifndef __LIQUID_FEC_DECODE_BENCHMARK_H__
#define __LIQUID_FEC_DECODE_BENCHMARK_H__

#include <sys/resource.h>
#include <stdlib.h>

#include "liquid.internal.h"

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
         _fs == FEC_CONV_V29P78 ||
         _fs == FEC_RS_M8)
    {
        fprintf(stderr,"warning: convolutional, Reed-Solomon codes unavailable (install libfec)\n");
        getrusage(RUSAGE_SELF, _start);
        memmove((void*)_finish,(void*)_start,sizeof(struct rusage));
        return;
    }
#endif

    // normalize number of iterations
    if (_fs != FEC_NONE)
        *_num_iterations /= _n;

    switch (_fs) {
    case FEC_CONV_V27:  *_num_iterations /= 5;      break;
    case FEC_CONV_V29:  *_num_iterations /= 10;     break;
    case FEC_CONV_V39:  *_num_iterations /= 100;    break;
    case FEC_CONV_V615: *_num_iterations /= 200;    break;
    case FEC_CONV_V27P23:
    case FEC_CONV_V27P34:
    case FEC_CONV_V27P45:
    case FEC_CONV_V27P56:
    case FEC_CONV_V27P67:
    case FEC_CONV_V27P78:
        *_num_iterations /= 20;
        break;
    case FEC_CONV_V29P23:
    case FEC_CONV_V29P34:
    case FEC_CONV_V29P45:
    case FEC_CONV_V29P56:
    case FEC_CONV_V29P67:
    case FEC_CONV_V29P78:
        *_num_iterations /= 50;
        break;
    default:;
    }
    if (*_num_iterations < 1) *_num_iterations = 1;

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
void benchmark_fec_dec_none_n64         FEC_DECODE_BENCH_API(FEC_NONE,      64,  NULL)

void benchmark_fec_dec_rep3_n64         FEC_DECODE_BENCH_API(FEC_REP3,      64,  NULL)
void benchmark_fec_dec_rep5_n64         FEC_DECODE_BENCH_API(FEC_REP5,      64,  NULL)
void benchmark_fec_dec_hamming74_n64    FEC_DECODE_BENCH_API(FEC_HAMMING74, 64,  NULL)

void benchmark_fec_dec_conv27_n64       FEC_DECODE_BENCH_API(FEC_CONV_V27,  64,  NULL)
void benchmark_fec_dec_conv29_n64       FEC_DECODE_BENCH_API(FEC_CONV_V29,  64,  NULL)
void benchmark_fec_dec_conv39_n64       FEC_DECODE_BENCH_API(FEC_CONV_V39,  64,  NULL)
void benchmark_fec_dec_conv615_n64      FEC_DECODE_BENCH_API(FEC_CONV_V615, 64,  NULL)

void benchmark_fec_dec_conv27p23_n64    FEC_DECODE_BENCH_API(FEC_CONV_V27P23,64, NULL)
void benchmark_fec_dec_conv27p34_n64    FEC_DECODE_BENCH_API(FEC_CONV_V27P34,64, NULL)
void benchmark_fec_dec_conv27p45_n64    FEC_DECODE_BENCH_API(FEC_CONV_V27P45,64, NULL)
void benchmark_fec_dec_conv27p56_n64    FEC_DECODE_BENCH_API(FEC_CONV_V27P56,64, NULL)
void benchmark_fec_dec_conv27p67_n64    FEC_DECODE_BENCH_API(FEC_CONV_V27P67,64, NULL)
void benchmark_fec_dec_conv27p78_n64    FEC_DECODE_BENCH_API(FEC_CONV_V27P78,64, NULL)

void benchmark_fec_dec_conv29p23_n64    FEC_DECODE_BENCH_API(FEC_CONV_V29P23,64, NULL)
void benchmark_fec_dec_conv29p34_n64    FEC_DECODE_BENCH_API(FEC_CONV_V29P34,64, NULL)
void benchmark_fec_dec_conv29p45_n64    FEC_DECODE_BENCH_API(FEC_CONV_V29P45,64, NULL)
void benchmark_fec_dec_conv29p56_n64    FEC_DECODE_BENCH_API(FEC_CONV_V29P56,64, NULL)
void benchmark_fec_dec_conv29p67_n64    FEC_DECODE_BENCH_API(FEC_CONV_V29P67,64, NULL)
void benchmark_fec_dec_conv29p78_n64    FEC_DECODE_BENCH_API(FEC_CONV_V29P78,64, NULL)

void benchmark_fec_dec_rs8_n64          FEC_DECODE_BENCH_API(FEC_RS_M8,      64,  NULL)

#endif // __LIQUID_FEC_DECODE_BENCHMARK_H__


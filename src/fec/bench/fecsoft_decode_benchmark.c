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

//
// fecsoft_decode_benchmark.h
//
// Test soft decoding speeds of certain FEC codecs
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#include "liquid.internal.h"

#define FECSOFT_DECODE_BENCH_API(FS,N, OPT) \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ fecsoft_decode_bench(_start, _finish, _num_iterations, FS, N, OPT); }

// Helper function to keep code base small
void fecsoft_decode_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    fec_scheme _fs,
    unsigned int _n,
    void * _opts)
{
#if !LIBFEC_ENABLED
    if ( _fs == LIQUID_FEC_CONV_V27    ||
         _fs == LIQUID_FEC_CONV_V29    ||
         _fs == LIQUID_FEC_CONV_V39    ||
         _fs == LIQUID_FEC_CONV_V615   ||
         _fs == LIQUID_FEC_CONV_V27P23 ||
         _fs == LIQUID_FEC_CONV_V27P34 ||
         _fs == LIQUID_FEC_CONV_V27P45 ||
         _fs == LIQUID_FEC_CONV_V27P56 ||
         _fs == LIQUID_FEC_CONV_V27P67 ||
         _fs == LIQUID_FEC_CONV_V27P78 ||
         _fs == LIQUID_FEC_CONV_V29P23 ||
         _fs == LIQUID_FEC_CONV_V29P34 ||
         _fs == LIQUID_FEC_CONV_V29P45 ||
         _fs == LIQUID_FEC_CONV_V29P56 ||
         _fs == LIQUID_FEC_CONV_V29P67 ||
         _fs == LIQUID_FEC_CONV_V29P78 ||
         _fs == LIQUID_FEC_RS_M8)
    {
        fprintf(stderr,"warning: convolutional, Reed-Solomon codes unavailable (install libfec)\n");
        getrusage(RUSAGE_SELF, _start);
        memmove((void*)_finish,(void*)_start,sizeof(struct rusage));
        return;
    }
#endif

    // normalize number of iterations
    *_num_iterations /= _n;

    switch (_fs) {
    case LIQUID_FEC_NONE:          *_num_iterations *= 50;  break;
    case LIQUID_FEC_REP3:          *_num_iterations *= 20;  break;
    case LIQUID_FEC_REP5:          *_num_iterations *= 10;  break;
    case LIQUID_FEC_HAMMING74:     *_num_iterations *= 1;   break;
    case LIQUID_FEC_HAMMING84:     *_num_iterations *= 1;   break;
    case LIQUID_FEC_HAMMING128:    *_num_iterations *= 1;   break;
    case LIQUID_FEC_CONV_V27:      *_num_iterations /= 5;   break;
    case LIQUID_FEC_CONV_V29:      *_num_iterations /= 50;  break;
    case LIQUID_FEC_CONV_V39:      *_num_iterations /= 50;  break;
    case LIQUID_FEC_CONV_V615:     *_num_iterations /= 800; break;
    case LIQUID_FEC_CONV_V27P23:
    case LIQUID_FEC_CONV_V27P34:
    case LIQUID_FEC_CONV_V27P45:
    case LIQUID_FEC_CONV_V27P56:
    case LIQUID_FEC_CONV_V27P67:
    case LIQUID_FEC_CONV_V27P78:
        *_num_iterations /= 5;
        break;
    case LIQUID_FEC_CONV_V29P23:
    case LIQUID_FEC_CONV_V29P34:
    case LIQUID_FEC_CONV_V29P45:
    case LIQUID_FEC_CONV_V29P56:
    case LIQUID_FEC_CONV_V29P67:
    case LIQUID_FEC_CONV_V29P78:
        *_num_iterations /= 50;
        break;
    case LIQUID_FEC_RS_M8:
        *_num_iterations *= 1;
        break;
    default:;
    }
    if (*_num_iterations < 1) *_num_iterations = 1;

    // generate fec object
    fec q = fec_create(_fs,_opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // encoded message
    unsigned char msg_soft[8*n_enc];// encoded message (soft bits)
    unsigned char msg_dec[_n];      // decoded message

    // initialze message
    unsigned long int i;
    for (i=0; i<_n; i++)
        msg[i] = rand() & 0xff;

    // encode message
    fec_encode(q,_n,msg,msg_enc);

    // expand to soft bits
    for (i=0; i<n_enc; i++) {
        msg_soft[8*i+0] = (msg_enc[i] & 0x80) ? 255 : 0;
        msg_soft[8*i+1] = (msg_enc[i] & 0x40) ? 255 : 0;
        msg_soft[8*i+2] = (msg_enc[i] & 0x20) ? 255 : 0;
        msg_soft[8*i+3] = (msg_enc[i] & 0x10) ? 255 : 0;
        msg_soft[8*i+4] = (msg_enc[i] & 0x08) ? 255 : 0;
        msg_soft[8*i+5] = (msg_enc[i] & 0x04) ? 255 : 0;
        msg_soft[8*i+6] = (msg_enc[i] & 0x02) ? 255 : 0;
        msg_soft[8*i+7] = (msg_enc[i] & 0x01) ? 255 : 0;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fec_decode_soft(q, _n, msg_enc, msg_dec);
        fec_decode_soft(q, _n, msg_enc, msg_dec);
        fec_decode_soft(q, _n, msg_enc, msg_dec);
        fec_decode_soft(q, _n, msg_enc, msg_dec);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up objects
    fec_destroy(q);
}

//
// BENCHMARKS
//
void benchmark_fecsoft_dec_none_n64       FECSOFT_DECODE_BENCH_API(LIQUID_FEC_NONE,      64,  NULL)

void benchmark_fecsoft_dec_rep3_n64       FECSOFT_DECODE_BENCH_API(LIQUID_FEC_REP3,      64,  NULL)
void benchmark_fecsoft_dec_rep5_n64       FECSOFT_DECODE_BENCH_API(LIQUID_FEC_REP5,      64,  NULL)
void benchmark_fecsoft_dec_hamming74_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_HAMMING74, 64,  NULL)
void benchmark_fecsoft_dec_hamming84_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_HAMMING84, 64,  NULL)
void benchmark_fecsoft_dec_hamming128_n64 FECSOFT_DECODE_BENCH_API(LIQUID_FEC_HAMMING128,64,  NULL)

void benchmark_fecsoft_dec_conv27_n64     FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27,  64,  NULL)
void benchmark_fecsoft_dec_conv29_n64     FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29,  64,  NULL)
void benchmark_fecsoft_dec_conv39_n64     FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V39,  64,  NULL)
void benchmark_fecsoft_dec_conv615_n64    FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V615, 64,  NULL)

void benchmark_fecsoft_dec_conv27p23_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27P23,64, NULL)
void benchmark_fecsoft_dec_conv27p34_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27P34,64, NULL)
void benchmark_fecsoft_dec_conv27p45_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27P45,64, NULL)
void benchmark_fecsoft_dec_conv27p56_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27P56,64, NULL)
void benchmark_fecsoft_dec_conv27p67_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27P67,64, NULL)
void benchmark_fecsoft_dec_conv27p78_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V27P78,64, NULL)

void benchmark_fecsoft_dec_conv29p23_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29P23,64, NULL)
void benchmark_fecsoft_dec_conv29p34_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29P34,64, NULL)
void benchmark_fecsoft_dec_conv29p45_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29P45,64, NULL)
void benchmark_fecsoft_dec_conv29p56_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29P56,64, NULL)
void benchmark_fecsoft_dec_conv29p67_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29P67,64, NULL)
void benchmark_fecsoft_dec_conv29p78_n64  FECSOFT_DECODE_BENCH_API(LIQUID_FEC_CONV_V29P78,64, NULL)

void benchmark_fecsoft_dec_rs8_n64        FECSOFT_DECODE_BENCH_API(LIQUID_FEC_RS_M8,      64, NULL)


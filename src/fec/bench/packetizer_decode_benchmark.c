/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "liquid.benchmark.h"
#include "liquid.internal.h"

// Helper function to keep code base small
float packetizer_decode_bench(unsigned long int _num_iterations,
                              unsigned int      _n,
                              crc_scheme        _crc,
                              fec_scheme        _fec0,
                              fec_scheme        _fec1)
{
    unsigned int msg_dec_len = _n;
    unsigned int msg_enc_len = packetizer_compute_enc_msg_len(_n,_crc,_fec0,_fec1);
    unsigned char msg_rec[msg_enc_len];
    unsigned char msg_dec[msg_dec_len];
    // initialize data
    unsigned long int i;
    for (i=0; i<msg_enc_len; i++) msg_rec[i] = rand() & 0xff;
    for (i=0; i<msg_dec_len; i++) msg_dec[i] = 0x00;
    // create packet generator
    packetizer q = packetizer_create(msg_dec_len, _crc, _fec0, _fec1);
    int crc_pass = 0;
    // start trials (4 packet decodes per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // decode packet
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        // randomize input
        msg_rec[0] ^= crc_pass ? 1 : 0;
    }
    float extime = liquid_toc(timer);
    // clean up allocated objects
    packetizer_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(packetizer_n16,   "packetizer_decode, n=16",   "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 16,   LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }
LIQUID_BENCHMARK(packetizer_n32,   "packetizer_decode, n=32",   "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 32,   LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }
LIQUID_BENCHMARK(packetizer_n64,   "packetizer_decode, n=64",   "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 64,   LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }
LIQUID_BENCHMARK(packetizer_n128,  "packetizer_decode, n=128",  "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 128,  LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }
LIQUID_BENCHMARK(packetizer_n256,  "packetizer_decode, n=256",  "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 256,  LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }
LIQUID_BENCHMARK(packetizer_n512,  "packetizer_decode, n=512",  "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 512,  LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }
LIQUID_BENCHMARK(packetizer_n1024, "packetizer_decode, n=1024", "fec,packetizer")
    { return packetizer_decode_bench(num_iterations, 1024, LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE); }


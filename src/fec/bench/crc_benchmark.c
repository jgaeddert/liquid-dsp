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
float crc_bench(unsigned long int _num_iterations,
               crc_scheme      _crc,
               unsigned int      _n)
{
    unsigned long int i;

    // create arrays
    unsigned char msg[_n];
    unsigned int key = 0;

    // initialize message
    for (i=0; i<_n; i++)
        msg[i] = rand() & 0xff;

    // start trials (4 key generations per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        key ^= crc_generate_key(_crc, msg, _n);
        key ^= crc_generate_key(_crc, msg, _n);
        key ^= crc_generate_key(_crc, msg, _n);
        key ^= crc_generate_key(_crc, msg, _n);

        // randomize input
        msg[0] ^= key & 0xff;
    }
    float extime = liquid_toc(timer);
    return extime;
}

// validate error-detection
LIQUID_BENCHMARK(crc_checksum_n256,
    "crc_generate_key checksum, n=256",
    "fec,crc,checksum")
{ return crc_bench(num_iterations, LIQUID_CRC_CHECKSUM, 256); }

LIQUID_BENCHMARK(crc_crc8_n256,
    "crc_generate_key crc8, n=256",
    "fec,crc")
{ return crc_bench(num_iterations, LIQUID_CRC_8,         256); }

LIQUID_BENCHMARK(crc_crc16_n256,
    "crc_generate_key crc16, n=256",
    "fec,crc")
{ return crc_bench(num_iterations, LIQUID_CRC_16,        256); }

LIQUID_BENCHMARK(crc_crc24_n256,
    "crc_generate_key crc24, n=256",
    "fec,crc")
{ return crc_bench(num_iterations, LIQUID_CRC_24,        256); }

LIQUID_BENCHMARK(crc_crc32_n256,
    "crc_generate_key crc32, n=256",
    "fec,crc")
{ return crc_bench(num_iterations, LIQUID_CRC_32,        256); }


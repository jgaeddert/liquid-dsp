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
float fec_encode_bench(unsigned long int _num_iterations,
                       fec_scheme        _fs,
                       unsigned int      _n,
                       void *            _opts)
{
#if !LIBFEC_ENABLED
    switch (_fs) {
    case LIQUID_FEC_CONV_V27:
    case LIQUID_FEC_CONV_V29:
    case LIQUID_FEC_CONV_V39:
    case LIQUID_FEC_CONV_V615:
    case LIQUID_FEC_CONV_V27P23:
    case LIQUID_FEC_CONV_V27P34:
    case LIQUID_FEC_CONV_V27P45:
    case LIQUID_FEC_CONV_V27P56:
    case LIQUID_FEC_CONV_V27P67:
    case LIQUID_FEC_CONV_V27P78:
    case LIQUID_FEC_CONV_V29P23:
    case LIQUID_FEC_CONV_V29P34:
    case LIQUID_FEC_CONV_V29P45:
    case LIQUID_FEC_CONV_V29P56:
    case LIQUID_FEC_CONV_V29P67:
    case LIQUID_FEC_CONV_V29P78:
    case LIQUID_FEC_RS_M8:
        liquid_log_warn("convolutional, Reed-Solomon codes unavailable (install libfec)");
        return -1.0f;
    default:;
    }
#endif
    // generate fec object
    fec q = fec_create(_fs,_opts);
    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // encoded message
    // initialize message
    unsigned long int i;
    for (i=0; i<_n; i++) {
        msg[i] = rand() & 0xff;
    }
    // start trials (4 encodes per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        fec_encode(q,_n,msg,msg_enc);
        fec_encode(q,_n,msg,msg_enc);
        fec_encode(q,_n,msg,msg_enc);
        fec_encode(q,_n,msg,msg_enc);
    }
    float extime = liquid_toc(timer);
    // clean up objects
    fec_destroy(q);
    return extime;
}

// no forward error correction
LIQUID_BENCHMARK(fec_enc_none_n64,
    "fec_encode none, n=64",
    "fec,encode")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_NONE,      64,  NULL); }

// repeat codes
LIQUID_BENCHMARK(fec_enc_rep3_n64,
    "fec_encode rep3, n=64",
    "fec,encode,repeat")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_REP3,      64,  NULL); }

LIQUID_BENCHMARK(fec_enc_rep5_n64,
    "fec_encode rep5, n=64",
    "fec,encode,repeat")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_REP5,      64,  NULL); }

// Hamming block codes
LIQUID_BENCHMARK(fec_enc_hamming74_n64,
    "fec_encode hamming74, n=64",
    "fec,encode,hamming")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_HAMMING74, 64,  NULL); }

LIQUID_BENCHMARK(fec_enc_hamming84_n64,
    "fec_encode hamming84, n=64",
    "fec,encode,hamming")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_HAMMING84, 64,  NULL); }

LIQUID_BENCHMARK(fec_enc_hamming128_n64,
    "fec_encode hamming128, n=64",
    "fec,encode,hamming")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_HAMMING128,64,  NULL); }

// Golay block codes
LIQUID_BENCHMARK(fec_enc_golay2412_n64,
    "fec_encode golay2412, n=64",
    "fec,encode,golay")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_GOLAY2412, 64,  NULL); }

// SEC-DED block codecs
LIQUID_BENCHMARK(fec_enc_secded2216_n64,
    "fec_encode secded2216, n=64",
    "fec,encode,secded")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_SECDED2216,64,  NULL); }

LIQUID_BENCHMARK(fec_enc_secded3932_n64,
    "fec_encode secded3932, n=64",
    "fec,encode,secded")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_SECDED3932,64,  NULL); }

LIQUID_BENCHMARK(fec_enc_secded7264_n64,
    "fec_encode secded7264, n=64",
    "fec,encode,secded")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_SECDED7264,64,  NULL); }

// convolutional codes
LIQUID_BENCHMARK(fec_enc_conv27_n64,
    "fec_encode conv27, n=64",
    "fec,encode,convolutional")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V27,  64,  NULL); }

LIQUID_BENCHMARK(fec_enc_conv29_n64,
    "fec_encode conv29, n=64",
    "fec,encode,convolutional")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V29,  64,  NULL); }

LIQUID_BENCHMARK(fec_enc_conv39_n64,
    "fec_encode conv39, n=64",
    "fec,encode,convolutional")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V39,  64,  NULL); }

LIQUID_BENCHMARK(fec_enc_conv615_n64,
    "fec_encode conv615, n=64",
    "fec,encode,convolutional")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V615, 64,  NULL); }

// convolutional codes (punctured)
LIQUID_BENCHMARK(fec_enc_conv27p23_n64,
    "fec_encode conv27p23, n=64",
    "fec,encode,convolutional,punctured")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V27P23,64, NULL); }

LIQUID_BENCHMARK(fec_enc_conv27p34_n64,
    "fec_encode conv27p34, n=64",
    "fec,encode,convolutional,punctured")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V27P34,64, NULL); }

LIQUID_BENCHMARK(fec_enc_conv27p45_n64,
    "fec_encode conv27p45, n=64",
    "fec,encode,convolutional,punctured")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_CONV_V27P45,64, NULL); }

// Reed-Solomon block codes
LIQUID_BENCHMARK(fec_enc_rs8_n64,
    "fec_encode rs8, n=64",
    "fec,encode,reed-solomon")
{ return fec_encode_bench(num_iterations, LIQUID_FEC_RS_M8,     64,  NULL); }


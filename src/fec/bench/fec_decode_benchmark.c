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
float fec_decode_bench(unsigned long int _num_iterations,
                       fec_scheme        _fs,
                       unsigned int      _n,
                       void *            _opts)
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
        liquid_error(LIQUID_EUMODE,"convolutional, Reed-Solomon codes unavailable (install libfec)");
        return 0.0f;
    }
#endif
    // generate fec object
    fec q = fec_create(_fs,_opts);
    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // decoded message
    unsigned char msg_dec[_n];      // decoded message
    // initialize message
    unsigned long int i;
    for (i=0; i<_n; i++)
        msg[i] = rand() & 0xff;
    // encode message
    fec_encode(q,_n,msg,msg_enc);
    // start trials (4 decodes per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        fec_decode(q,_n,msg_enc,msg_dec);
        fec_decode(q,_n,msg_enc,msg_dec);
        fec_decode(q,_n,msg_enc,msg_dec);
        fec_decode(q,_n,msg_enc,msg_dec);
    }
    float extime = liquid_toc(timer);
    // clean up objects
    fec_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(fec_dec_none_n64,         "fec_decode none, n=64",         "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_NONE,      64,  NULL); }
LIQUID_BENCHMARK(fec_dec_rep3_n64,         "fec_decode rep3, n=64",         "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_REP3,      64,  NULL); }
LIQUID_BENCHMARK(fec_dec_rep5_n64,         "fec_decode rep5, n=64",         "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_REP5,      64,  NULL); }
LIQUID_BENCHMARK(fec_dec_hamming74_n64,    "fec_decode hamming74, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_HAMMING74, 64,  NULL); }
LIQUID_BENCHMARK(fec_dec_hamming84_n64,    "fec_decode hamming84, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_HAMMING84, 64,  NULL); }
LIQUID_BENCHMARK(fec_dec_hamming128_n64,   "fec_decode hamming128, n=64",   "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_HAMMING128,64,  NULL); }

// SEC-DED block codes
LIQUID_BENCHMARK(fec_dec_secded2216_n64,   "fec_decode secded2216, n=64",   "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_SECDED2216,64,  NULL); }
LIQUID_BENCHMARK(fec_dec_secded3932_n64,   "fec_decode secded3932, n=64",   "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_SECDED3932,64,  NULL); }
LIQUID_BENCHMARK(fec_dec_secded7264_n64,   "fec_decode secded7264, n=64",   "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_SECDED7264,64,  NULL); }

LIQUID_BENCHMARK(fec_dec_golay2412_n64,    "fec_decode golay2412, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_GOLAY2412, 64,  NULL); }

LIQUID_BENCHMARK(fec_dec_conv27_n64,       "fec_decode conv27, n=64",       "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27,  64,  NULL); }
LIQUID_BENCHMARK(fec_dec_conv29_n64,       "fec_decode conv29, n=64",       "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29,  64,  NULL); }
LIQUID_BENCHMARK(fec_dec_conv39_n64,       "fec_decode conv39, n=64",       "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V39,  64,  NULL); }
LIQUID_BENCHMARK(fec_dec_conv615_n64,      "fec_decode conv615, n=64",      "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V615, 64,  NULL); }

LIQUID_BENCHMARK(fec_dec_conv27p23_n64,    "fec_decode conv27p23, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27P23,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv27p34_n64,    "fec_decode conv27p34, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27P34,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv27p45_n64,    "fec_decode conv27p45, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27P45,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv27p56_n64,    "fec_decode conv27p56, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27P56,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv27p67_n64,    "fec_decode conv27p67, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27P67,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv27p78_n64,    "fec_decode conv27p78, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V27P78,64, NULL); }

LIQUID_BENCHMARK(fec_dec_conv29p23_n64,    "fec_decode conv29p23, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29P23,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv29p34_n64,    "fec_decode conv29p34, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29P34,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv29p45_n64,    "fec_decode conv29p45, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29P45,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv29p56_n64,    "fec_decode conv29p56, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29P56,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv29p67_n64,    "fec_decode conv29p67, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29P67,64, NULL); }
LIQUID_BENCHMARK(fec_dec_conv29p78_n64,    "fec_decode conv29p78, n=64",    "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_CONV_V29P78,64, NULL); }

LIQUID_BENCHMARK(fec_dec_rs8_n64,          "fec_decode rs8, n=64",          "fec,decode")
    { return fec_decode_bench(num_iterations, LIQUID_FEC_RS_M8,     64,  NULL); }


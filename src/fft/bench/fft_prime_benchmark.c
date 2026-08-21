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

// benchmark FFTs of prime length

#include "src/fft/bench/fft_runbench.h"

// prime numbers
LIQUID_BENCHMARK(fft_3  , "fft execute, nfft=3",   "fft,prime") {return fft_runbench(num_iterations, 3  , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_5  , "fft execute, nfft=5",   "fft,prime") {return fft_runbench(num_iterations, 5  , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_7  , "fft execute, nfft=7",   "fft,prime") {return fft_runbench(num_iterations, 7  , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_11 , "fft execute, nfft=11",  "fft,prime") {return fft_runbench(num_iterations, 11 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_13 , "fft execute, nfft=13",  "fft,prime") {return fft_runbench(num_iterations, 13 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_17 , "fft execute, nfft=17",  "fft,prime") {return fft_runbench(num_iterations, 17 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_19 , "fft execute, nfft=19",  "fft,prime") {return fft_runbench(num_iterations, 19 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_23 , "fft execute, nfft=23",  "fft,prime") {return fft_runbench(num_iterations, 23 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_29 , "fft execute, nfft=29",  "fft,prime") {return fft_runbench(num_iterations, 29 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_31 , "fft execute, nfft=31",  "fft,prime") {return fft_runbench(num_iterations, 31 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_37 , "fft execute, nfft=37",  "fft,prime") {return fft_runbench(num_iterations, 37 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_41 , "fft execute, nfft=41",  "fft,prime") {return fft_runbench(num_iterations, 41 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_43 , "fft execute, nfft=43",  "fft,prime") {return fft_runbench(num_iterations, 43 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_47 , "fft execute, nfft=47",  "fft,prime") {return fft_runbench(num_iterations, 47 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_53 , "fft execute, nfft=53",  "fft,prime") {return fft_runbench(num_iterations, 53 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_59 , "fft execute, nfft=59",  "fft,prime") {return fft_runbench(num_iterations, 59 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_61 , "fft execute, nfft=61",  "fft,prime") {return fft_runbench(num_iterations, 61 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_67 , "fft execute, nfft=67",  "fft,prime") {return fft_runbench(num_iterations, 67 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_71 , "fft execute, nfft=71",  "fft,prime") {return fft_runbench(num_iterations, 71 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_73 , "fft execute, nfft=73",  "fft,prime") {return fft_runbench(num_iterations, 73 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_79 , "fft execute, nfft=79",  "fft,prime") {return fft_runbench(num_iterations, 79 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_83 , "fft execute, nfft=83",  "fft,prime") {return fft_runbench(num_iterations, 83 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_89 , "fft execute, nfft=89",  "fft,prime") {return fft_runbench(num_iterations, 89 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_97 , "fft execute, nfft=97",  "fft,prime") {return fft_runbench(num_iterations, 97 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_101, "fft execute, nfft=101", "fft,prime") {return fft_runbench(num_iterations, 101, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_103, "fft execute, nfft=103", "fft,prime") {return fft_runbench(num_iterations, 103, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_107, "fft execute, nfft=107", "fft,prime") {return fft_runbench(num_iterations, 107, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_109, "fft execute, nfft=109", "fft,prime") {return fft_runbench(num_iterations, 109, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_113, "fft execute, nfft=113", "fft,prime") {return fft_runbench(num_iterations, 113, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_127, "fft execute, nfft=127", "fft,prime") {return fft_runbench(num_iterations, 127, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_131, "fft execute, nfft=131", "fft,prime") {return fft_runbench(num_iterations, 131, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_137, "fft execute, nfft=137", "fft,prime") {return fft_runbench(num_iterations, 137, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_139, "fft execute, nfft=139", "fft,prime") {return fft_runbench(num_iterations, 139, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_149, "fft execute, nfft=149", "fft,prime") {return fft_runbench(num_iterations, 149, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_151, "fft execute, nfft=151", "fft,prime") {return fft_runbench(num_iterations, 151, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_157, "fft execute, nfft=157", "fft,prime") {return fft_runbench(num_iterations, 157, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_163, "fft execute, nfft=163", "fft,prime") {return fft_runbench(num_iterations, 163, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_167, "fft execute, nfft=167", "fft,prime") {return fft_runbench(num_iterations, 167, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_173, "fft execute, nfft=173", "fft,prime") {return fft_runbench(num_iterations, 173, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_179, "fft execute, nfft=179", "fft,prime") {return fft_runbench(num_iterations, 179, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_181, "fft execute, nfft=181", "fft,prime") {return fft_runbench(num_iterations, 181, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_191, "fft execute, nfft=191", "fft,prime") {return fft_runbench(num_iterations, 191, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_193, "fft execute, nfft=193", "fft,prime") {return fft_runbench(num_iterations, 193, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_197, "fft execute, nfft=197", "fft,prime") {return fft_runbench(num_iterations, 197, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_199, "fft execute, nfft=199", "fft,prime") {return fft_runbench(num_iterations, 199, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_211, "fft execute, nfft=211", "fft,prime") {return fft_runbench(num_iterations, 211, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_223, "fft execute, nfft=223", "fft,prime") {return fft_runbench(num_iterations, 223, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_227, "fft execute, nfft=227", "fft,prime") {return fft_runbench(num_iterations, 227, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_229, "fft execute, nfft=229", "fft,prime") {return fft_runbench(num_iterations, 229, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_233, "fft execute, nfft=233", "fft,prime") {return fft_runbench(num_iterations, 233, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_239, "fft execute, nfft=239", "fft,prime") {return fft_runbench(num_iterations, 239, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_241, "fft execute, nfft=241", "fft,prime") {return fft_runbench(num_iterations, 241, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_251, "fft execute, nfft=251", "fft,prime") {return fft_runbench(num_iterations, 251, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_257, "fft execute, nfft=257", "fft,prime") {return fft_runbench(num_iterations, 257, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_263, "fft execute, nfft=263", "fft,prime") {return fft_runbench(num_iterations, 263, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_269, "fft execute, nfft=269", "fft,prime") {return fft_runbench(num_iterations, 269, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_271, "fft execute, nfft=271", "fft,prime") {return fft_runbench(num_iterations, 271, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_277, "fft execute, nfft=277", "fft,prime") {return fft_runbench(num_iterations, 277, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_281, "fft execute, nfft=281", "fft,prime") {return fft_runbench(num_iterations, 281, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_283, "fft execute, nfft=283", "fft,prime") {return fft_runbench(num_iterations, 283, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_293, "fft execute, nfft=293", "fft,prime") {return fft_runbench(num_iterations, 293, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_307, "fft execute, nfft=307", "fft,prime") {return fft_runbench(num_iterations, 307, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_311, "fft execute, nfft=311", "fft,prime") {return fft_runbench(num_iterations, 311, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_313, "fft execute, nfft=313", "fft,prime") {return fft_runbench(num_iterations, 313, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_317, "fft execute, nfft=317", "fft,prime") {return fft_runbench(num_iterations, 317, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_331, "fft execute, nfft=331", "fft,prime") {return fft_runbench(num_iterations, 331, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_337, "fft execute, nfft=337", "fft,prime") {return fft_runbench(num_iterations, 337, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_347, "fft execute, nfft=347", "fft,prime") {return fft_runbench(num_iterations, 347, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_349, "fft execute, nfft=349", "fft,prime") {return fft_runbench(num_iterations, 349, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_353, "fft execute, nfft=353", "fft,prime") {return fft_runbench(num_iterations, 353, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_359, "fft execute, nfft=359", "fft,prime") {return fft_runbench(num_iterations, 359, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_367, "fft execute, nfft=367", "fft,prime") {return fft_runbench(num_iterations, 367, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_373, "fft execute, nfft=373", "fft,prime") {return fft_runbench(num_iterations, 373, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_379, "fft execute, nfft=379", "fft,prime") {return fft_runbench(num_iterations, 379, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_383, "fft execute, nfft=383", "fft,prime") {return fft_runbench(num_iterations, 383, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_389, "fft execute, nfft=389", "fft,prime") {return fft_runbench(num_iterations, 389, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_397, "fft execute, nfft=397", "fft,prime") {return fft_runbench(num_iterations, 397, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_401, "fft execute, nfft=401", "fft,prime") {return fft_runbench(num_iterations, 401, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_409, "fft execute, nfft=409", "fft,prime") {return fft_runbench(num_iterations, 409, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_419, "fft execute, nfft=419", "fft,prime") {return fft_runbench(num_iterations, 419, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_421, "fft execute, nfft=421", "fft,prime") {return fft_runbench(num_iterations, 421, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_431, "fft execute, nfft=431", "fft,prime") {return fft_runbench(num_iterations, 431, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_433, "fft execute, nfft=433", "fft,prime") {return fft_runbench(num_iterations, 433, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_439, "fft execute, nfft=439", "fft,prime") {return fft_runbench(num_iterations, 439, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_443, "fft execute, nfft=443", "fft,prime") {return fft_runbench(num_iterations, 443, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_449, "fft execute, nfft=449", "fft,prime") {return fft_runbench(num_iterations, 449, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_457, "fft execute, nfft=457", "fft,prime") {return fft_runbench(num_iterations, 457, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_461, "fft execute, nfft=461", "fft,prime") {return fft_runbench(num_iterations, 461, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_463, "fft execute, nfft=463", "fft,prime") {return fft_runbench(num_iterations, 463, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_467, "fft execute, nfft=467", "fft,prime") {return fft_runbench(num_iterations, 467, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_479, "fft execute, nfft=479", "fft,prime") {return fft_runbench(num_iterations, 479, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_487, "fft execute, nfft=487", "fft,prime") {return fft_runbench(num_iterations, 487, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_491, "fft execute, nfft=491", "fft,prime") {return fft_runbench(num_iterations, 491, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_499, "fft execute, nfft=499", "fft,prime") {return fft_runbench(num_iterations, 499, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_503, "fft execute, nfft=503", "fft,prime") {return fft_runbench(num_iterations, 503, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_509, "fft execute, nfft=509", "fft,prime") {return fft_runbench(num_iterations, 509, LIQUID_FFT_FORWARD); }


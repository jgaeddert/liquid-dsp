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

// benchmark FFTs of 'composite' length (not prime, not of form 2^m)

#include "src/fft/bench/fft_runbench.h"

// composite numbers
LIQUID_BENCHMARK(fft_6  , "fft execute, nfft=6"  , "fft,composite") {return fft_runbench(num_iterations, 6  , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_9  , "fft execute, nfft=9"  , "fft,composite") {return fft_runbench(num_iterations, 9  , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_10 , "fft execute, nfft=10" , "fft,composite") {return fft_runbench(num_iterations, 10 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_12 , "fft execute, nfft=12" , "fft,composite") {return fft_runbench(num_iterations, 12 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_14 , "fft execute, nfft=14" , "fft,composite") {return fft_runbench(num_iterations, 14 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_15 , "fft execute, nfft=15" , "fft,composite") {return fft_runbench(num_iterations, 15 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_18 , "fft execute, nfft=18" , "fft,composite") {return fft_runbench(num_iterations, 18 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_20 , "fft execute, nfft=20" , "fft,composite") {return fft_runbench(num_iterations, 20 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_21 , "fft execute, nfft=21" , "fft,composite") {return fft_runbench(num_iterations, 21 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_22 , "fft execute, nfft=22" , "fft,composite") {return fft_runbench(num_iterations, 22 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_24 , "fft execute, nfft=24" , "fft,composite") {return fft_runbench(num_iterations, 24 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_25 , "fft execute, nfft=25" , "fft,composite") {return fft_runbench(num_iterations, 25 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_26 , "fft execute, nfft=26" , "fft,composite") {return fft_runbench(num_iterations, 26 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_27 , "fft execute, nfft=27" , "fft,composite") {return fft_runbench(num_iterations, 27 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_28 , "fft execute, nfft=28" , "fft,composite") {return fft_runbench(num_iterations, 28 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_30 , "fft execute, nfft=30" , "fft,composite") {return fft_runbench(num_iterations, 30 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_33 , "fft execute, nfft=33" , "fft,composite") {return fft_runbench(num_iterations, 33 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_34 , "fft execute, nfft=34" , "fft,composite") {return fft_runbench(num_iterations, 34 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_35 , "fft execute, nfft=35" , "fft,composite") {return fft_runbench(num_iterations, 35 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_36 , "fft execute, nfft=36" , "fft,composite") {return fft_runbench(num_iterations, 36 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_38 , "fft execute, nfft=38" , "fft,composite") {return fft_runbench(num_iterations, 38 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_39 , "fft execute, nfft=39" , "fft,composite") {return fft_runbench(num_iterations, 39 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_40 , "fft execute, nfft=40" , "fft,composite") {return fft_runbench(num_iterations, 40 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_42 , "fft execute, nfft=42" , "fft,composite") {return fft_runbench(num_iterations, 42 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_44 , "fft execute, nfft=44" , "fft,composite") {return fft_runbench(num_iterations, 44 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_45 , "fft execute, nfft=45" , "fft,composite") {return fft_runbench(num_iterations, 45 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_46 , "fft execute, nfft=46" , "fft,composite") {return fft_runbench(num_iterations, 46 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_48 , "fft execute, nfft=48" , "fft,composite") {return fft_runbench(num_iterations, 48 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_49 , "fft execute, nfft=49" , "fft,composite") {return fft_runbench(num_iterations, 49 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_50 , "fft execute, nfft=50" , "fft,composite") {return fft_runbench(num_iterations, 50 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_51 , "fft execute, nfft=51" , "fft,composite") {return fft_runbench(num_iterations, 51 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_52 , "fft execute, nfft=52" , "fft,composite") {return fft_runbench(num_iterations, 52 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_54 , "fft execute, nfft=54" , "fft,composite") {return fft_runbench(num_iterations, 54 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_55 , "fft execute, nfft=55" , "fft,composite") {return fft_runbench(num_iterations, 55 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_56 , "fft execute, nfft=56" , "fft,composite") {return fft_runbench(num_iterations, 56 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_57 , "fft execute, nfft=57" , "fft,composite") {return fft_runbench(num_iterations, 57 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_58 , "fft execute, nfft=58" , "fft,composite") {return fft_runbench(num_iterations, 58 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_60 , "fft execute, nfft=60" , "fft,composite") {return fft_runbench(num_iterations, 60 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_62 , "fft execute, nfft=62" , "fft,composite") {return fft_runbench(num_iterations, 62 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_63 , "fft execute, nfft=63" , "fft,composite") {return fft_runbench(num_iterations, 63 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_65 , "fft execute, nfft=65" , "fft,composite") {return fft_runbench(num_iterations, 65 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_66 , "fft execute, nfft=66" , "fft,composite") {return fft_runbench(num_iterations, 66 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_68 , "fft execute, nfft=68" , "fft,composite") {return fft_runbench(num_iterations, 68 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_69 , "fft execute, nfft=69" , "fft,composite") {return fft_runbench(num_iterations, 69 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_70 , "fft execute, nfft=70" , "fft,composite") {return fft_runbench(num_iterations, 70 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_72 , "fft execute, nfft=72" , "fft,composite") {return fft_runbench(num_iterations, 72 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_74 , "fft execute, nfft=74" , "fft,composite") {return fft_runbench(num_iterations, 74 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_75 , "fft execute, nfft=75" , "fft,composite") {return fft_runbench(num_iterations, 75 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_76 , "fft execute, nfft=76" , "fft,composite") {return fft_runbench(num_iterations, 76 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_77 , "fft execute, nfft=77" , "fft,composite") {return fft_runbench(num_iterations, 77 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_78 , "fft execute, nfft=78" , "fft,composite") {return fft_runbench(num_iterations, 78 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_80 , "fft execute, nfft=80" , "fft,composite") {return fft_runbench(num_iterations, 80 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_81 , "fft execute, nfft=81" , "fft,composite") {return fft_runbench(num_iterations, 81 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_82 , "fft execute, nfft=82" , "fft,composite") {return fft_runbench(num_iterations, 82 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_84 , "fft execute, nfft=84" , "fft,composite") {return fft_runbench(num_iterations, 84 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_85 , "fft execute, nfft=85" , "fft,composite") {return fft_runbench(num_iterations, 85 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_86 , "fft execute, nfft=86" , "fft,composite") {return fft_runbench(num_iterations, 86 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_87 , "fft execute, nfft=87" , "fft,composite") {return fft_runbench(num_iterations, 87 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_88 , "fft execute, nfft=88" , "fft,composite") {return fft_runbench(num_iterations, 88 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_90 , "fft execute, nfft=90" , "fft,composite") {return fft_runbench(num_iterations, 90 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_91 , "fft execute, nfft=91" , "fft,composite") {return fft_runbench(num_iterations, 91 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_92 , "fft execute, nfft=92" , "fft,composite") {return fft_runbench(num_iterations, 92 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_93 , "fft execute, nfft=93" , "fft,composite") {return fft_runbench(num_iterations, 93 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_94 , "fft execute, nfft=94" , "fft,composite") {return fft_runbench(num_iterations, 94 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_95 , "fft execute, nfft=95" , "fft,composite") {return fft_runbench(num_iterations, 95 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_96 , "fft execute, nfft=96" , "fft,composite") {return fft_runbench(num_iterations, 96 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_98 , "fft execute, nfft=98" , "fft,composite") {return fft_runbench(num_iterations, 98 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_99 , "fft execute, nfft=99" , "fft,composite") {return fft_runbench(num_iterations, 99 , LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_100, "fft execute, nfft=100", "fft,composite") {return fft_runbench(num_iterations, 100, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_102, "fft execute, nfft=102", "fft,composite") {return fft_runbench(num_iterations, 102, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_104, "fft execute, nfft=104", "fft,composite") {return fft_runbench(num_iterations, 104, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_105, "fft execute, nfft=105", "fft,composite") {return fft_runbench(num_iterations, 105, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_106, "fft execute, nfft=106", "fft,composite") {return fft_runbench(num_iterations, 106, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_108, "fft execute, nfft=108", "fft,composite") {return fft_runbench(num_iterations, 108, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_110, "fft execute, nfft=110", "fft,composite") {return fft_runbench(num_iterations, 110, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_111, "fft execute, nfft=111", "fft,composite") {return fft_runbench(num_iterations, 111, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_112, "fft execute, nfft=112", "fft,composite") {return fft_runbench(num_iterations, 112, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_114, "fft execute, nfft=114", "fft,composite") {return fft_runbench(num_iterations, 114, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_115, "fft execute, nfft=115", "fft,composite") {return fft_runbench(num_iterations, 115, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_116, "fft execute, nfft=116", "fft,composite") {return fft_runbench(num_iterations, 116, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_117, "fft execute, nfft=117", "fft,composite") {return fft_runbench(num_iterations, 117, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_118, "fft execute, nfft=118", "fft,composite") {return fft_runbench(num_iterations, 118, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_119, "fft execute, nfft=119", "fft,composite") {return fft_runbench(num_iterations, 119, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_120, "fft execute, nfft=120", "fft,composite") {return fft_runbench(num_iterations, 120, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_121, "fft execute, nfft=121", "fft,composite") {return fft_runbench(num_iterations, 121, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_122, "fft execute, nfft=122", "fft,composite") {return fft_runbench(num_iterations, 122, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_123, "fft execute, nfft=123", "fft,composite") {return fft_runbench(num_iterations, 123, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_124, "fft execute, nfft=124", "fft,composite") {return fft_runbench(num_iterations, 124, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_125, "fft execute, nfft=125", "fft,composite") {return fft_runbench(num_iterations, 125, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_126, "fft execute, nfft=126", "fft,composite") {return fft_runbench(num_iterations, 126, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_129, "fft execute, nfft=129", "fft,composite") {return fft_runbench(num_iterations, 129, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_130, "fft execute, nfft=130", "fft,composite") {return fft_runbench(num_iterations, 130, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_132, "fft execute, nfft=132", "fft,composite") {return fft_runbench(num_iterations, 132, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_133, "fft execute, nfft=133", "fft,composite") {return fft_runbench(num_iterations, 133, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_134, "fft execute, nfft=134", "fft,composite") {return fft_runbench(num_iterations, 134, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_135, "fft execute, nfft=135", "fft,composite") {return fft_runbench(num_iterations, 135, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_136, "fft execute, nfft=136", "fft,composite") {return fft_runbench(num_iterations, 136, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_138, "fft execute, nfft=138", "fft,composite") {return fft_runbench(num_iterations, 138, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_140, "fft execute, nfft=140", "fft,composite") {return fft_runbench(num_iterations, 140, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_141, "fft execute, nfft=141", "fft,composite") {return fft_runbench(num_iterations, 141, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_142, "fft execute, nfft=142", "fft,composite") {return fft_runbench(num_iterations, 142, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_143, "fft execute, nfft=143", "fft,composite") {return fft_runbench(num_iterations, 143, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_144, "fft execute, nfft=144", "fft,composite") {return fft_runbench(num_iterations, 144, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_145, "fft execute, nfft=145", "fft,composite") {return fft_runbench(num_iterations, 145, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_146, "fft execute, nfft=146", "fft,composite") {return fft_runbench(num_iterations, 146, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_147, "fft execute, nfft=147", "fft,composite") {return fft_runbench(num_iterations, 147, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_148, "fft execute, nfft=148", "fft,composite") {return fft_runbench(num_iterations, 148, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_150, "fft execute, nfft=150", "fft,composite") {return fft_runbench(num_iterations, 150, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_152, "fft execute, nfft=152", "fft,composite") {return fft_runbench(num_iterations, 152, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_153, "fft execute, nfft=153", "fft,composite") {return fft_runbench(num_iterations, 153, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_154, "fft execute, nfft=154", "fft,composite") {return fft_runbench(num_iterations, 154, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_155, "fft execute, nfft=155", "fft,composite") {return fft_runbench(num_iterations, 155, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_156, "fft execute, nfft=156", "fft,composite") {return fft_runbench(num_iterations, 156, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_158, "fft execute, nfft=158", "fft,composite") {return fft_runbench(num_iterations, 158, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_159, "fft execute, nfft=159", "fft,composite") {return fft_runbench(num_iterations, 159, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_160, "fft execute, nfft=160", "fft,composite") {return fft_runbench(num_iterations, 160, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_161, "fft execute, nfft=161", "fft,composite") {return fft_runbench(num_iterations, 161, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_162, "fft execute, nfft=162", "fft,composite") {return fft_runbench(num_iterations, 162, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_164, "fft execute, nfft=164", "fft,composite") {return fft_runbench(num_iterations, 164, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_165, "fft execute, nfft=165", "fft,composite") {return fft_runbench(num_iterations, 165, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_166, "fft execute, nfft=166", "fft,composite") {return fft_runbench(num_iterations, 166, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_168, "fft execute, nfft=168", "fft,composite") {return fft_runbench(num_iterations, 168, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_169, "fft execute, nfft=169", "fft,composite") {return fft_runbench(num_iterations, 169, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_170, "fft execute, nfft=170", "fft,composite") {return fft_runbench(num_iterations, 170, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_171, "fft execute, nfft=171", "fft,composite") {return fft_runbench(num_iterations, 171, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_172, "fft execute, nfft=172", "fft,composite") {return fft_runbench(num_iterations, 172, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_174, "fft execute, nfft=174", "fft,composite") {return fft_runbench(num_iterations, 174, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_175, "fft execute, nfft=175", "fft,composite") {return fft_runbench(num_iterations, 175, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_176, "fft execute, nfft=176", "fft,composite") {return fft_runbench(num_iterations, 176, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_177, "fft execute, nfft=177", "fft,composite") {return fft_runbench(num_iterations, 177, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_178, "fft execute, nfft=178", "fft,composite") {return fft_runbench(num_iterations, 178, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_180, "fft execute, nfft=180", "fft,composite") {return fft_runbench(num_iterations, 180, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_182, "fft execute, nfft=182", "fft,composite") {return fft_runbench(num_iterations, 182, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_183, "fft execute, nfft=183", "fft,composite") {return fft_runbench(num_iterations, 183, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_184, "fft execute, nfft=184", "fft,composite") {return fft_runbench(num_iterations, 184, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_185, "fft execute, nfft=185", "fft,composite") {return fft_runbench(num_iterations, 185, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_186, "fft execute, nfft=186", "fft,composite") {return fft_runbench(num_iterations, 186, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_187, "fft execute, nfft=187", "fft,composite") {return fft_runbench(num_iterations, 187, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_188, "fft execute, nfft=188", "fft,composite") {return fft_runbench(num_iterations, 188, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_189, "fft execute, nfft=189", "fft,composite") {return fft_runbench(num_iterations, 189, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_190, "fft execute, nfft=190", "fft,composite") {return fft_runbench(num_iterations, 190, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_192, "fft execute, nfft=192", "fft,composite") {return fft_runbench(num_iterations, 192, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_194, "fft execute, nfft=194", "fft,composite") {return fft_runbench(num_iterations, 194, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_195, "fft execute, nfft=195", "fft,composite") {return fft_runbench(num_iterations, 195, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_196, "fft execute, nfft=196", "fft,composite") {return fft_runbench(num_iterations, 196, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_198, "fft execute, nfft=198", "fft,composite") {return fft_runbench(num_iterations, 198, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_200, "fft execute, nfft=200", "fft,composite") {return fft_runbench(num_iterations, 200, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_201, "fft execute, nfft=201", "fft,composite") {return fft_runbench(num_iterations, 201, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_202, "fft execute, nfft=202", "fft,composite") {return fft_runbench(num_iterations, 202, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_203, "fft execute, nfft=203", "fft,composite") {return fft_runbench(num_iterations, 203, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_204, "fft execute, nfft=204", "fft,composite") {return fft_runbench(num_iterations, 204, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_205, "fft execute, nfft=205", "fft,composite") {return fft_runbench(num_iterations, 205, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_206, "fft execute, nfft=206", "fft,composite") {return fft_runbench(num_iterations, 206, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_207, "fft execute, nfft=207", "fft,composite") {return fft_runbench(num_iterations, 207, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_208, "fft execute, nfft=208", "fft,composite") {return fft_runbench(num_iterations, 208, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_209, "fft execute, nfft=209", "fft,composite") {return fft_runbench(num_iterations, 209, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_210, "fft execute, nfft=210", "fft,composite") {return fft_runbench(num_iterations, 210, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_212, "fft execute, nfft=212", "fft,composite") {return fft_runbench(num_iterations, 212, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_213, "fft execute, nfft=213", "fft,composite") {return fft_runbench(num_iterations, 213, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_214, "fft execute, nfft=214", "fft,composite") {return fft_runbench(num_iterations, 214, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_215, "fft execute, nfft=215", "fft,composite") {return fft_runbench(num_iterations, 215, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_216, "fft execute, nfft=216", "fft,composite") {return fft_runbench(num_iterations, 216, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_217, "fft execute, nfft=217", "fft,composite") {return fft_runbench(num_iterations, 217, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_218, "fft execute, nfft=218", "fft,composite") {return fft_runbench(num_iterations, 218, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_219, "fft execute, nfft=219", "fft,composite") {return fft_runbench(num_iterations, 219, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_220, "fft execute, nfft=220", "fft,composite") {return fft_runbench(num_iterations, 220, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_221, "fft execute, nfft=221", "fft,composite") {return fft_runbench(num_iterations, 221, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_222, "fft execute, nfft=222", "fft,composite") {return fft_runbench(num_iterations, 222, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_224, "fft execute, nfft=224", "fft,composite") {return fft_runbench(num_iterations, 224, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_225, "fft execute, nfft=225", "fft,composite") {return fft_runbench(num_iterations, 225, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_226, "fft execute, nfft=226", "fft,composite") {return fft_runbench(num_iterations, 226, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_228, "fft execute, nfft=228", "fft,composite") {return fft_runbench(num_iterations, 228, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_230, "fft execute, nfft=230", "fft,composite") {return fft_runbench(num_iterations, 230, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_231, "fft execute, nfft=231", "fft,composite") {return fft_runbench(num_iterations, 231, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_232, "fft execute, nfft=232", "fft,composite") {return fft_runbench(num_iterations, 232, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_234, "fft execute, nfft=234", "fft,composite") {return fft_runbench(num_iterations, 234, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_235, "fft execute, nfft=235", "fft,composite") {return fft_runbench(num_iterations, 235, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_236, "fft execute, nfft=236", "fft,composite") {return fft_runbench(num_iterations, 236, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_237, "fft execute, nfft=237", "fft,composite") {return fft_runbench(num_iterations, 237, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_238, "fft execute, nfft=238", "fft,composite") {return fft_runbench(num_iterations, 238, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_240, "fft execute, nfft=240", "fft,composite") {return fft_runbench(num_iterations, 240, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_242, "fft execute, nfft=242", "fft,composite") {return fft_runbench(num_iterations, 242, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_243, "fft execute, nfft=243", "fft,composite") {return fft_runbench(num_iterations, 243, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_244, "fft execute, nfft=244", "fft,composite") {return fft_runbench(num_iterations, 244, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_245, "fft execute, nfft=245", "fft,composite") {return fft_runbench(num_iterations, 245, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_246, "fft execute, nfft=246", "fft,composite") {return fft_runbench(num_iterations, 246, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_247, "fft execute, nfft=247", "fft,composite") {return fft_runbench(num_iterations, 247, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_248, "fft execute, nfft=248", "fft,composite") {return fft_runbench(num_iterations, 248, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_249, "fft execute, nfft=249", "fft,composite") {return fft_runbench(num_iterations, 249, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_250, "fft execute, nfft=250", "fft,composite") {return fft_runbench(num_iterations, 250, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_252, "fft execute, nfft=252", "fft,composite") {return fft_runbench(num_iterations, 252, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_253, "fft execute, nfft=253", "fft,composite") {return fft_runbench(num_iterations, 253, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_254, "fft execute, nfft=254", "fft,composite") {return fft_runbench(num_iterations, 254, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_255, "fft execute, nfft=255", "fft,composite") {return fft_runbench(num_iterations, 255, LIQUID_FFT_FORWARD); }

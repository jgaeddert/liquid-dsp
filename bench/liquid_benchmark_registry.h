#ifndef __LIQUID_BENCHMARK_REGISTRY_H__
#define __LIQUID_BENCHMARK_REGISTRY_H__

#include "liquid.benchmark.h"

// ./src/agc/bench/agc_crcf_benchmark.c
extern struct liquid_benchmark_s agc_crcf_s;
// ./src/audio/bench/cvsd_benchmark.c
extern struct liquid_benchmark_s cvsd_encode_s;
extern struct liquid_benchmark_s cvsd_decode_s;
// ./src/buffer/bench/cbuffercf_benchmark.c
extern struct liquid_benchmark_s cbuffercf_n16_s;
extern struct liquid_benchmark_s cbuffercf_n32_s;
extern struct liquid_benchmark_s cbuffercf_n64_s;
extern struct liquid_benchmark_s cbuffercf_n128_s;
extern struct liquid_benchmark_s cbuffercf_n256_s;
extern struct liquid_benchmark_s cbuffercf_n512_s;
extern struct liquid_benchmark_s cbuffercf_n1024_s;
// ./src/buffer/bench/window_push_benchmark.c
extern struct liquid_benchmark_s windowcf_push_n16_s;
extern struct liquid_benchmark_s windowcf_push_n32_s;
extern struct liquid_benchmark_s windowcf_push_n64_s;
extern struct liquid_benchmark_s windowcf_push_n128_s;
extern struct liquid_benchmark_s windowcf_push_n256_s;
// ./src/buffer/bench/window_read_benchmark.c
extern struct liquid_benchmark_s windowcf_read_n16_s;
extern struct liquid_benchmark_s windowcf_read_n32_s;
extern struct liquid_benchmark_s windowcf_read_n64_s;
extern struct liquid_benchmark_s windowcf_read_n128_s;
extern struct liquid_benchmark_s windowcf_read_n256_s;
// ./src/core/bench/logging_benchmark.c
extern struct liquid_benchmark_s logging_s;
// ./src/dotprod/bench/dotprod_cccf_benchmark.c
extern struct liquid_benchmark_s dotprod_cccf_4_s;
extern struct liquid_benchmark_s dotprod_cccf_16_s;
extern struct liquid_benchmark_s dotprod_cccf_64_s;
extern struct liquid_benchmark_s dotprod_cccf_256_s;
// ./src/dotprod/bench/dotprod_crcf_benchmark.c
extern struct liquid_benchmark_s dotprod_crcf_4_s;
extern struct liquid_benchmark_s dotprod_crcf_16_s;
extern struct liquid_benchmark_s dotprod_crcf_64_s;
extern struct liquid_benchmark_s dotprod_crcf_256_s;
// ./src/dotprod/bench/dotprod_rrrf_benchmark.c
extern struct liquid_benchmark_s dotprod_rrrf_4_s;
extern struct liquid_benchmark_s dotprod_rrrf_16_s;
extern struct liquid_benchmark_s dotprod_rrrf_64_s;
extern struct liquid_benchmark_s dotprod_rrrf_256_s;
// ./src/dotprod/bench/sumsqcf_benchmark.c
extern struct liquid_benchmark_s sumsqcf_4_s;
extern struct liquid_benchmark_s sumsqcf_16_s;
extern struct liquid_benchmark_s sumsqcf_64_s;
extern struct liquid_benchmark_s sumsqcf_256_s;
// ./src/dotprod/bench/sumsqf_benchmark.c
extern struct liquid_benchmark_s sumsqf_4_s;
extern struct liquid_benchmark_s sumsqf_16_s;
extern struct liquid_benchmark_s sumsqf_64_s;
extern struct liquid_benchmark_s sumsqf_256_s;
// ./src/equalization/bench/eqlms_cccf_benchmark.c
extern struct liquid_benchmark_s eqlms_cccf_n4_s;
extern struct liquid_benchmark_s eqlms_cccf_n8_s;
extern struct liquid_benchmark_s eqlms_cccf_n16_s;
extern struct liquid_benchmark_s eqlms_cccf_n32_s;
extern struct liquid_benchmark_s eqlms_cccf_n64_s;
// ./src/equalization/bench/eqrls_cccf_benchmark.c
extern struct liquid_benchmark_s eqrls_cccf_n4_s;
extern struct liquid_benchmark_s eqrls_cccf_n8_s;
extern struct liquid_benchmark_s eqrls_cccf_n16_s;
extern struct liquid_benchmark_s eqrls_cccf_n32_s;
extern struct liquid_benchmark_s eqrls_cccf_n64_s;
// ./src/fft/bench/asgramcf_benchmark.c
extern struct liquid_benchmark_s asgramcf_64_s;
extern struct liquid_benchmark_s asgramcf_80_s;
extern struct liquid_benchmark_s asgramcf_96_s;
extern struct liquid_benchmark_s asgramcf_120_s;
extern struct liquid_benchmark_s asgramcf_64_autoscale_s;
extern struct liquid_benchmark_s asgramcf_80_autoscale_s;
extern struct liquid_benchmark_s asgramcf_96_autoscale_s;
extern struct liquid_benchmark_s asgramcf_120_autoscale_s;
// ./src/fft/bench/fft_composite_benchmark.c
extern struct liquid_benchmark_s fft_6_s;
extern struct liquid_benchmark_s fft_9_s;
extern struct liquid_benchmark_s fft_10_s;
extern struct liquid_benchmark_s fft_12_s;
extern struct liquid_benchmark_s fft_14_s;
extern struct liquid_benchmark_s fft_15_s;
extern struct liquid_benchmark_s fft_18_s;
extern struct liquid_benchmark_s fft_20_s;
extern struct liquid_benchmark_s fft_21_s;
extern struct liquid_benchmark_s fft_22_s;
extern struct liquid_benchmark_s fft_24_s;
extern struct liquid_benchmark_s fft_25_s;
extern struct liquid_benchmark_s fft_26_s;
extern struct liquid_benchmark_s fft_27_s;
extern struct liquid_benchmark_s fft_28_s;
extern struct liquid_benchmark_s fft_30_s;
extern struct liquid_benchmark_s fft_33_s;
extern struct liquid_benchmark_s fft_34_s;
extern struct liquid_benchmark_s fft_35_s;
extern struct liquid_benchmark_s fft_36_s;
extern struct liquid_benchmark_s fft_38_s;
extern struct liquid_benchmark_s fft_39_s;
extern struct liquid_benchmark_s fft_40_s;
extern struct liquid_benchmark_s fft_42_s;
extern struct liquid_benchmark_s fft_44_s;
extern struct liquid_benchmark_s fft_45_s;
extern struct liquid_benchmark_s fft_46_s;
extern struct liquid_benchmark_s fft_48_s;
extern struct liquid_benchmark_s fft_49_s;
extern struct liquid_benchmark_s fft_50_s;
extern struct liquid_benchmark_s fft_51_s;
extern struct liquid_benchmark_s fft_52_s;
extern struct liquid_benchmark_s fft_54_s;
extern struct liquid_benchmark_s fft_55_s;
extern struct liquid_benchmark_s fft_56_s;
extern struct liquid_benchmark_s fft_57_s;
extern struct liquid_benchmark_s fft_58_s;
extern struct liquid_benchmark_s fft_60_s;
extern struct liquid_benchmark_s fft_62_s;
extern struct liquid_benchmark_s fft_63_s;
extern struct liquid_benchmark_s fft_65_s;
extern struct liquid_benchmark_s fft_66_s;
extern struct liquid_benchmark_s fft_68_s;
extern struct liquid_benchmark_s fft_69_s;
extern struct liquid_benchmark_s fft_70_s;
extern struct liquid_benchmark_s fft_72_s;
extern struct liquid_benchmark_s fft_74_s;
extern struct liquid_benchmark_s fft_75_s;
extern struct liquid_benchmark_s fft_76_s;
extern struct liquid_benchmark_s fft_77_s;
extern struct liquid_benchmark_s fft_78_s;
extern struct liquid_benchmark_s fft_80_s;
extern struct liquid_benchmark_s fft_81_s;
extern struct liquid_benchmark_s fft_82_s;
extern struct liquid_benchmark_s fft_84_s;
extern struct liquid_benchmark_s fft_85_s;
extern struct liquid_benchmark_s fft_86_s;
extern struct liquid_benchmark_s fft_87_s;
extern struct liquid_benchmark_s fft_88_s;
extern struct liquid_benchmark_s fft_90_s;
extern struct liquid_benchmark_s fft_91_s;
extern struct liquid_benchmark_s fft_92_s;
extern struct liquid_benchmark_s fft_93_s;
extern struct liquid_benchmark_s fft_94_s;
extern struct liquid_benchmark_s fft_95_s;
extern struct liquid_benchmark_s fft_96_s;
extern struct liquid_benchmark_s fft_98_s;
extern struct liquid_benchmark_s fft_99_s;
extern struct liquid_benchmark_s fft_100_s;
extern struct liquid_benchmark_s fft_102_s;
extern struct liquid_benchmark_s fft_104_s;
extern struct liquid_benchmark_s fft_105_s;
extern struct liquid_benchmark_s fft_106_s;
extern struct liquid_benchmark_s fft_108_s;
extern struct liquid_benchmark_s fft_110_s;
extern struct liquid_benchmark_s fft_111_s;
extern struct liquid_benchmark_s fft_112_s;
extern struct liquid_benchmark_s fft_114_s;
extern struct liquid_benchmark_s fft_115_s;
extern struct liquid_benchmark_s fft_116_s;
extern struct liquid_benchmark_s fft_117_s;
extern struct liquid_benchmark_s fft_118_s;
extern struct liquid_benchmark_s fft_119_s;
extern struct liquid_benchmark_s fft_120_s;
extern struct liquid_benchmark_s fft_121_s;
extern struct liquid_benchmark_s fft_122_s;
extern struct liquid_benchmark_s fft_123_s;
extern struct liquid_benchmark_s fft_124_s;
extern struct liquid_benchmark_s fft_125_s;
extern struct liquid_benchmark_s fft_126_s;
extern struct liquid_benchmark_s fft_129_s;
extern struct liquid_benchmark_s fft_130_s;
extern struct liquid_benchmark_s fft_132_s;
extern struct liquid_benchmark_s fft_133_s;
extern struct liquid_benchmark_s fft_134_s;
extern struct liquid_benchmark_s fft_135_s;
extern struct liquid_benchmark_s fft_136_s;
extern struct liquid_benchmark_s fft_138_s;
extern struct liquid_benchmark_s fft_140_s;
extern struct liquid_benchmark_s fft_141_s;
extern struct liquid_benchmark_s fft_142_s;
extern struct liquid_benchmark_s fft_143_s;
extern struct liquid_benchmark_s fft_144_s;
extern struct liquid_benchmark_s fft_145_s;
extern struct liquid_benchmark_s fft_146_s;
extern struct liquid_benchmark_s fft_147_s;
extern struct liquid_benchmark_s fft_148_s;
extern struct liquid_benchmark_s fft_150_s;
extern struct liquid_benchmark_s fft_152_s;
extern struct liquid_benchmark_s fft_153_s;
extern struct liquid_benchmark_s fft_154_s;
extern struct liquid_benchmark_s fft_155_s;
extern struct liquid_benchmark_s fft_156_s;
extern struct liquid_benchmark_s fft_158_s;
extern struct liquid_benchmark_s fft_159_s;
extern struct liquid_benchmark_s fft_160_s;
extern struct liquid_benchmark_s fft_161_s;
extern struct liquid_benchmark_s fft_162_s;
extern struct liquid_benchmark_s fft_164_s;
extern struct liquid_benchmark_s fft_165_s;
extern struct liquid_benchmark_s fft_166_s;
extern struct liquid_benchmark_s fft_168_s;
extern struct liquid_benchmark_s fft_169_s;
extern struct liquid_benchmark_s fft_170_s;
extern struct liquid_benchmark_s fft_171_s;
extern struct liquid_benchmark_s fft_172_s;
extern struct liquid_benchmark_s fft_174_s;
extern struct liquid_benchmark_s fft_175_s;
extern struct liquid_benchmark_s fft_176_s;
extern struct liquid_benchmark_s fft_177_s;
extern struct liquid_benchmark_s fft_178_s;
extern struct liquid_benchmark_s fft_180_s;
extern struct liquid_benchmark_s fft_182_s;
extern struct liquid_benchmark_s fft_183_s;
extern struct liquid_benchmark_s fft_184_s;
extern struct liquid_benchmark_s fft_185_s;
extern struct liquid_benchmark_s fft_186_s;
extern struct liquid_benchmark_s fft_187_s;
extern struct liquid_benchmark_s fft_188_s;
extern struct liquid_benchmark_s fft_189_s;
extern struct liquid_benchmark_s fft_190_s;
extern struct liquid_benchmark_s fft_192_s;
extern struct liquid_benchmark_s fft_194_s;
extern struct liquid_benchmark_s fft_195_s;
extern struct liquid_benchmark_s fft_196_s;
extern struct liquid_benchmark_s fft_198_s;
extern struct liquid_benchmark_s fft_200_s;
extern struct liquid_benchmark_s fft_201_s;
extern struct liquid_benchmark_s fft_202_s;
extern struct liquid_benchmark_s fft_203_s;
extern struct liquid_benchmark_s fft_204_s;
extern struct liquid_benchmark_s fft_205_s;
extern struct liquid_benchmark_s fft_206_s;
extern struct liquid_benchmark_s fft_207_s;
extern struct liquid_benchmark_s fft_208_s;
extern struct liquid_benchmark_s fft_209_s;
extern struct liquid_benchmark_s fft_210_s;
extern struct liquid_benchmark_s fft_212_s;
extern struct liquid_benchmark_s fft_213_s;
extern struct liquid_benchmark_s fft_214_s;
extern struct liquid_benchmark_s fft_215_s;
extern struct liquid_benchmark_s fft_216_s;
extern struct liquid_benchmark_s fft_217_s;
extern struct liquid_benchmark_s fft_218_s;
extern struct liquid_benchmark_s fft_219_s;
extern struct liquid_benchmark_s fft_220_s;
extern struct liquid_benchmark_s fft_221_s;
extern struct liquid_benchmark_s fft_222_s;
extern struct liquid_benchmark_s fft_224_s;
extern struct liquid_benchmark_s fft_225_s;
extern struct liquid_benchmark_s fft_226_s;
extern struct liquid_benchmark_s fft_228_s;
extern struct liquid_benchmark_s fft_230_s;
extern struct liquid_benchmark_s fft_231_s;
extern struct liquid_benchmark_s fft_232_s;
extern struct liquid_benchmark_s fft_234_s;
extern struct liquid_benchmark_s fft_235_s;
extern struct liquid_benchmark_s fft_236_s;
extern struct liquid_benchmark_s fft_237_s;
extern struct liquid_benchmark_s fft_238_s;
extern struct liquid_benchmark_s fft_240_s;
extern struct liquid_benchmark_s fft_242_s;
extern struct liquid_benchmark_s fft_243_s;
extern struct liquid_benchmark_s fft_244_s;
extern struct liquid_benchmark_s fft_245_s;
extern struct liquid_benchmark_s fft_246_s;
extern struct liquid_benchmark_s fft_247_s;
extern struct liquid_benchmark_s fft_248_s;
extern struct liquid_benchmark_s fft_249_s;
extern struct liquid_benchmark_s fft_250_s;
extern struct liquid_benchmark_s fft_252_s;
extern struct liquid_benchmark_s fft_253_s;
extern struct liquid_benchmark_s fft_254_s;
extern struct liquid_benchmark_s fft_255_s;
// ./src/fft/bench/fft_prime_benchmark.c
extern struct liquid_benchmark_s fft_3_s;
extern struct liquid_benchmark_s fft_5_s;
extern struct liquid_benchmark_s fft_7_s;
extern struct liquid_benchmark_s fft_11_s;
extern struct liquid_benchmark_s fft_13_s;
extern struct liquid_benchmark_s fft_17_s;
extern struct liquid_benchmark_s fft_19_s;
extern struct liquid_benchmark_s fft_23_s;
extern struct liquid_benchmark_s fft_29_s;
extern struct liquid_benchmark_s fft_31_s;
extern struct liquid_benchmark_s fft_37_s;
extern struct liquid_benchmark_s fft_41_s;
extern struct liquid_benchmark_s fft_43_s;
extern struct liquid_benchmark_s fft_47_s;
extern struct liquid_benchmark_s fft_53_s;
extern struct liquid_benchmark_s fft_59_s;
extern struct liquid_benchmark_s fft_61_s;
extern struct liquid_benchmark_s fft_67_s;
extern struct liquid_benchmark_s fft_71_s;
extern struct liquid_benchmark_s fft_73_s;
extern struct liquid_benchmark_s fft_79_s;
extern struct liquid_benchmark_s fft_83_s;
extern struct liquid_benchmark_s fft_89_s;
extern struct liquid_benchmark_s fft_97_s;
extern struct liquid_benchmark_s fft_101_s;
extern struct liquid_benchmark_s fft_103_s;
extern struct liquid_benchmark_s fft_107_s;
extern struct liquid_benchmark_s fft_109_s;
extern struct liquid_benchmark_s fft_113_s;
extern struct liquid_benchmark_s fft_127_s;
extern struct liquid_benchmark_s fft_131_s;
extern struct liquid_benchmark_s fft_137_s;
extern struct liquid_benchmark_s fft_139_s;
extern struct liquid_benchmark_s fft_149_s;
extern struct liquid_benchmark_s fft_151_s;
extern struct liquid_benchmark_s fft_157_s;
extern struct liquid_benchmark_s fft_163_s;
extern struct liquid_benchmark_s fft_167_s;
extern struct liquid_benchmark_s fft_173_s;
extern struct liquid_benchmark_s fft_179_s;
extern struct liquid_benchmark_s fft_181_s;
extern struct liquid_benchmark_s fft_191_s;
extern struct liquid_benchmark_s fft_193_s;
extern struct liquid_benchmark_s fft_197_s;
extern struct liquid_benchmark_s fft_199_s;
extern struct liquid_benchmark_s fft_211_s;
extern struct liquid_benchmark_s fft_223_s;
extern struct liquid_benchmark_s fft_227_s;
extern struct liquid_benchmark_s fft_229_s;
extern struct liquid_benchmark_s fft_233_s;
extern struct liquid_benchmark_s fft_239_s;
extern struct liquid_benchmark_s fft_241_s;
extern struct liquid_benchmark_s fft_251_s;
extern struct liquid_benchmark_s fft_257_s;
extern struct liquid_benchmark_s fft_263_s;
extern struct liquid_benchmark_s fft_269_s;
extern struct liquid_benchmark_s fft_271_s;
extern struct liquid_benchmark_s fft_277_s;
extern struct liquid_benchmark_s fft_281_s;
extern struct liquid_benchmark_s fft_283_s;
extern struct liquid_benchmark_s fft_293_s;
extern struct liquid_benchmark_s fft_307_s;
extern struct liquid_benchmark_s fft_311_s;
extern struct liquid_benchmark_s fft_313_s;
extern struct liquid_benchmark_s fft_317_s;
extern struct liquid_benchmark_s fft_331_s;
extern struct liquid_benchmark_s fft_337_s;
extern struct liquid_benchmark_s fft_347_s;
extern struct liquid_benchmark_s fft_349_s;
extern struct liquid_benchmark_s fft_353_s;
extern struct liquid_benchmark_s fft_359_s;
extern struct liquid_benchmark_s fft_367_s;
extern struct liquid_benchmark_s fft_373_s;
extern struct liquid_benchmark_s fft_379_s;
extern struct liquid_benchmark_s fft_383_s;
extern struct liquid_benchmark_s fft_389_s;
extern struct liquid_benchmark_s fft_397_s;
extern struct liquid_benchmark_s fft_401_s;
extern struct liquid_benchmark_s fft_409_s;
extern struct liquid_benchmark_s fft_419_s;
extern struct liquid_benchmark_s fft_421_s;
extern struct liquid_benchmark_s fft_431_s;
extern struct liquid_benchmark_s fft_433_s;
extern struct liquid_benchmark_s fft_439_s;
extern struct liquid_benchmark_s fft_443_s;
extern struct liquid_benchmark_s fft_449_s;
extern struct liquid_benchmark_s fft_457_s;
extern struct liquid_benchmark_s fft_461_s;
extern struct liquid_benchmark_s fft_463_s;
extern struct liquid_benchmark_s fft_467_s;
extern struct liquid_benchmark_s fft_479_s;
extern struct liquid_benchmark_s fft_487_s;
extern struct liquid_benchmark_s fft_491_s;
extern struct liquid_benchmark_s fft_499_s;
extern struct liquid_benchmark_s fft_503_s;
extern struct liquid_benchmark_s fft_509_s;
// ./src/fft/bench/fft_r2r_benchmark.c
extern struct liquid_benchmark_s fft_REDFT00_128_s;
extern struct liquid_benchmark_s fft_REDFT01_128_s;
extern struct liquid_benchmark_s fft_REDFT10_128_s;
extern struct liquid_benchmark_s fft_REDFT11_128_s;
extern struct liquid_benchmark_s fft_RODFT00_128_s;
extern struct liquid_benchmark_s fft_RODFT01_128_s;
extern struct liquid_benchmark_s fft_RODFT10_128_s;
extern struct liquid_benchmark_s fft_RODFT11_128_s;
extern struct liquid_benchmark_s fft_REDFT00_127_s;
extern struct liquid_benchmark_s fft_REDFT01_127_s;
extern struct liquid_benchmark_s fft_REDFT10_127_s;
extern struct liquid_benchmark_s fft_REDFT11_127_s;
extern struct liquid_benchmark_s fft_RODFT00_127_s;
extern struct liquid_benchmark_s fft_RODFT01_127_s;
extern struct liquid_benchmark_s fft_RODFT10_127_s;
extern struct liquid_benchmark_s fft_RODFT11_127_s;
// ./src/fft/bench/fft_radix2_benchmark.c
extern struct liquid_benchmark_s fft_2_s;
extern struct liquid_benchmark_s fft_4_s;
extern struct liquid_benchmark_s fft_8_s;
extern struct liquid_benchmark_s fft_16_s;
extern struct liquid_benchmark_s fft_32_s;
extern struct liquid_benchmark_s fft_64_s;
extern struct liquid_benchmark_s fft_128_s;
extern struct liquid_benchmark_s fft_256_s;
extern struct liquid_benchmark_s fft_512_s;
extern struct liquid_benchmark_s fft_1024_s;
extern struct liquid_benchmark_s fft_2048_s;
extern struct liquid_benchmark_s fft_4096_s;
extern struct liquid_benchmark_s fft_8192_s;
extern struct liquid_benchmark_s fft_16384_s;
extern struct liquid_benchmark_s fft_32768_s;
// ./src/fft/bench/spgramcf_benchmark.c
extern struct liquid_benchmark_s spgramcf_1200_s;
extern struct liquid_benchmark_s spgramcf_9600_s;
extern struct liquid_benchmark_s spgramcf_76800_s;
extern struct liquid_benchmark_s spgramcf_614400_s;
// ./src/filter/bench/fftfilt_crcf_benchmark.c
extern struct liquid_benchmark_s fftfilt_crcf_4_s;
extern struct liquid_benchmark_s fftfilt_crcf_8_s;
extern struct liquid_benchmark_s fftfilt_crcf_16_s;
extern struct liquid_benchmark_s fftfilt_crcf_32_s;
extern struct liquid_benchmark_s fftfilt_crcf_64_s;
// ./src/filter/bench/firdecim_crcf_benchmark.c
extern struct liquid_benchmark_s firdecim_crcf_m2_h8_s;
extern struct liquid_benchmark_s firdecim_crcf_m4_h16_s;
extern struct liquid_benchmark_s firdecim_crcf_m8_h32_s;
extern struct liquid_benchmark_s firdecim_crcf_m16_h64_s;
extern struct liquid_benchmark_s firdecim_crcf_m32_h128_s;
// ./src/filter/bench/firfilt_crcf_benchmark.c
extern struct liquid_benchmark_s firfilt_crcf_4_s;
extern struct liquid_benchmark_s firfilt_crcf_8_s;
extern struct liquid_benchmark_s firfilt_crcf_16_s;
extern struct liquid_benchmark_s firfilt_crcf_32_s;
extern struct liquid_benchmark_s firfilt_crcf_64_s;
// ./src/filter/bench/firhilb_benchmark.c
extern struct liquid_benchmark_s firhilbf_decim_m3_s;
extern struct liquid_benchmark_s firhilbf_decim_m5_s;
extern struct liquid_benchmark_s firhilbf_decim_m9_s;
extern struct liquid_benchmark_s firhilbf_decim_m13_s;
// ./src/filter/bench/firinterp_crcf_benchmark.c
extern struct liquid_benchmark_s firinterp_crcf_m2_h8_s;
extern struct liquid_benchmark_s firinterp_crcf_m4_h16_s;
extern struct liquid_benchmark_s firinterp_crcf_m8_h32_s;
extern struct liquid_benchmark_s firinterp_crcf_m16_h64_s;
extern struct liquid_benchmark_s firinterp_crcf_m32_h128_s;
// ./src/filter/bench/iirdecim_crcf_benchmark.c
extern struct liquid_benchmark_s iirdecim_crcf_M2_s;
extern struct liquid_benchmark_s iirdecim_crcf_M4_s;
extern struct liquid_benchmark_s iirdecim_crcf_M8_s;
extern struct liquid_benchmark_s iirdecim_crcf_M16_s;
extern struct liquid_benchmark_s iirdecim_crcf_M32_s;
// ./src/filter/bench/iirfilt_crcf_benchmark.c
extern struct liquid_benchmark_s iirfilt_crcf_4_s;
extern struct liquid_benchmark_s iirfilt_crcf_8_s;
extern struct liquid_benchmark_s iirfilt_crcf_16_s;
extern struct liquid_benchmark_s iirfilt_crcf_32_s;
extern struct liquid_benchmark_s iirfilt_crcf_64_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_4_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_8_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_16_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_32_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_64_s;
extern struct liquid_benchmark_s iirfilt_crcf_dcblock_s;
// ./src/filter/bench/iirinterp_crcf_benchmark.c
extern struct liquid_benchmark_s iirinterp_crcf_M2_s;
extern struct liquid_benchmark_s iirinterp_crcf_M4_s;
extern struct liquid_benchmark_s iirinterp_crcf_M8_s;
extern struct liquid_benchmark_s iirinterp_crcf_M16_s;
extern struct liquid_benchmark_s iirinterp_crcf_M32_s;
// ./src/filter/bench/resamp2_crcf_benchmark.c
extern struct liquid_benchmark_s resamp2_crcf_decim_m2_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m4_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m8_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m16_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m32_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m64_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m128_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m256_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m2_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m4_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m8_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m16_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m32_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m64_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m128_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m256_s;
// ./src/filter/bench/resamp_crcf_benchmark.c
extern struct liquid_benchmark_s resamp_crcf_P17_Q1_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q2_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q4_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q8_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q16_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q32_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q64_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q128_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q256_s;
// ./src/filter/bench/rresamp_crcf_benchmark.c
extern struct liquid_benchmark_s rresamp_crcf_P17_Q1_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q2_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q4_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q8_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q16_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q32_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q64_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q128_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q256_s;
// ./src/filter/bench/symsync_crcf_benchmark.c
extern struct liquid_benchmark_s symsync_crcf_k2_m2_s;
extern struct liquid_benchmark_s symsync_crcf_k2_m4_s;
extern struct liquid_benchmark_s symsync_crcf_k2_m8_s;
extern struct liquid_benchmark_s symsync_crcf_k2_m16_s;
// ./src/framing/bench/bpacketsync_benchmark.c
extern struct liquid_benchmark_s bpacketsync_s;
// ./src/framing/bench/bpresync_benchmark.c
extern struct liquid_benchmark_s bpresync_cccf_16_s;
extern struct liquid_benchmark_s bpresync_cccf_32_s;
extern struct liquid_benchmark_s bpresync_cccf_64_s;
extern struct liquid_benchmark_s bpresync_cccf_128_s;
extern struct liquid_benchmark_s bpresync_cccf_256_s;
// ./src/framing/bench/bsync_benchmark.c
extern struct liquid_benchmark_s bsync_cccf_16_s;
extern struct liquid_benchmark_s bsync_cccf_32_s;
extern struct liquid_benchmark_s bsync_cccf_64_s;
extern struct liquid_benchmark_s bsync_cccf_128_s;
extern struct liquid_benchmark_s bsync_cccf_256_s;
// ./src/framing/bench/detector_benchmark.c
extern struct liquid_benchmark_s detector_cccf_16_s;
extern struct liquid_benchmark_s detector_cccf_32_s;
extern struct liquid_benchmark_s detector_cccf_64_s;
extern struct liquid_benchmark_s detector_cccf_128_s;
extern struct liquid_benchmark_s detector_cccf_256_s;
// ./src/framing/bench/flexframesync_benchmark.c
extern struct liquid_benchmark_s flexframesync_s;
// ./src/framing/bench/framesync64_benchmark.c
extern struct liquid_benchmark_s framesync64_s;
// ./src/framing/bench/gmskframesync_benchmark.c
extern struct liquid_benchmark_s gmskframesync_s;
extern struct liquid_benchmark_s gmskframesync_noise_s;
// ./src/framing/bench/presync_benchmark.c
extern struct liquid_benchmark_s presync_cccf_16_s;
extern struct liquid_benchmark_s presync_cccf_32_s;
extern struct liquid_benchmark_s presync_cccf_64_s;
extern struct liquid_benchmark_s presync_cccf_128_s;
extern struct liquid_benchmark_s presync_cccf_256_s;
// ./src/framing/bench/qdetector_benchmark.c
extern struct liquid_benchmark_s qdetector_cccf_16_s;
extern struct liquid_benchmark_s qdetector_cccf_32_s;
extern struct liquid_benchmark_s qdetector_cccf_64_s;
extern struct liquid_benchmark_s qdetector_cccf_128_s;
extern struct liquid_benchmark_s qdetector_cccf_256_s;
extern struct liquid_benchmark_s qdetector_cccf_512_s;
extern struct liquid_benchmark_s qdetector_cccf_1024_s;
extern struct liquid_benchmark_s qdetector_cccf_2048_s;
extern struct liquid_benchmark_s qdetector_cccf_4096_s;
extern struct liquid_benchmark_s qdetector_cccf_8192_s;
extern struct liquid_benchmark_s qdetector_cccf_16384_s;
// ./src/math/bench/polyfit_benchmark.c
extern struct liquid_benchmark_s polyfit_q3_n8_s;
extern struct liquid_benchmark_s polyfit_q3_n16_s;
extern struct liquid_benchmark_s polyfit_q3_n32_s;
extern struct liquid_benchmark_s polyfit_q3_n64_s;
extern struct liquid_benchmark_s polyfit_q3_n128_s;
// ./src/matrix/bench/matrixf_inv_benchmark.c
extern struct liquid_benchmark_s matrixf_inv_n2_s;
extern struct liquid_benchmark_s matrixf_inv_n4_s;
extern struct liquid_benchmark_s matrixf_inv_n8_s;
extern struct liquid_benchmark_s matrixf_inv_n16_s;
extern struct liquid_benchmark_s matrixf_inv_n32_s;
extern struct liquid_benchmark_s matrixf_inv_n64_s;
// ./src/matrix/bench/matrixf_linsolve_benchmark.c
extern struct liquid_benchmark_s matrixf_linsolve_n2_s;
extern struct liquid_benchmark_s matrixf_linsolve_n4_s;
extern struct liquid_benchmark_s matrixf_linsolve_n8_s;
extern struct liquid_benchmark_s matrixf_linsolve_n16_s;
extern struct liquid_benchmark_s matrixf_linsolve_n32_s;
extern struct liquid_benchmark_s matrixf_linsolve_n64_s;
// ./src/matrix/bench/matrixf_mul_benchmark.c
extern struct liquid_benchmark_s matrixf_mul_n2_s;
extern struct liquid_benchmark_s matrixf_mul_n4_s;
extern struct liquid_benchmark_s matrixf_mul_n8_s;
extern struct liquid_benchmark_s matrixf_mul_n16_s;
extern struct liquid_benchmark_s matrixf_mul_n32_s;
extern struct liquid_benchmark_s matrixf_mul_n64_s;
// ./src/matrix/bench/smatrixf_mul_benchmark.c
extern struct liquid_benchmark_s smatrixf_mul_n32_s;
extern struct liquid_benchmark_s smatrixf_mul_n64_s;
extern struct liquid_benchmark_s smatrixf_mul_n128_s;
extern struct liquid_benchmark_s smatrixf_mul_n256_s;
extern struct liquid_benchmark_s smatrixf_mul_n512_s;
// ./src/modem/bench/freqdem_benchmark.c
extern struct liquid_benchmark_s freqdem_s;
// ./src/modem/bench/freqmod_benchmark.c
extern struct liquid_benchmark_s freqmod_s;
// ./src/modem/bench/fskdem_benchmark.c
extern struct liquid_benchmark_s fskdem_norm_M2_s;
extern struct liquid_benchmark_s fskdem_norm_M4_s;
extern struct liquid_benchmark_s fskdem_norm_M8_s;
extern struct liquid_benchmark_s fskdem_norm_M16_s;
extern struct liquid_benchmark_s fskdem_norm_M32_s;
extern struct liquid_benchmark_s fskdem_norm_M64_s;
extern struct liquid_benchmark_s fskdem_norm_M128_s;
extern struct liquid_benchmark_s fskdem_norm_M256_s;
extern struct liquid_benchmark_s fskdem_norm_M512_s;
extern struct liquid_benchmark_s fskdem_norm_M1024_s;
extern struct liquid_benchmark_s fskdem_misc_M2_s;
extern struct liquid_benchmark_s fskdem_misc_M4_s;
extern struct liquid_benchmark_s fskdem_misc_M8_s;
extern struct liquid_benchmark_s fskdem_misc_M16_s;
extern struct liquid_benchmark_s fskdem_misc_M32_s;
extern struct liquid_benchmark_s fskdem_misc_M64_s;
extern struct liquid_benchmark_s fskdem_misc_M128_s;
extern struct liquid_benchmark_s fskdem_misc_M256_s;
extern struct liquid_benchmark_s fskdem_misc_M512_s;
extern struct liquid_benchmark_s fskdem_misc_M1024_s;
// ./src/modem/bench/fskmod_benchmark.c
extern struct liquid_benchmark_s fskmod_norm_M2_s;
extern struct liquid_benchmark_s fskmod_norm_M4_s;
extern struct liquid_benchmark_s fskmod_norm_M8_s;
extern struct liquid_benchmark_s fskmod_norm_M16_s;
extern struct liquid_benchmark_s fskmod_norm_M32_s;
extern struct liquid_benchmark_s fskmod_norm_M64_s;
extern struct liquid_benchmark_s fskmod_norm_M128_s;
extern struct liquid_benchmark_s fskmod_norm_M256_s;
extern struct liquid_benchmark_s fskmod_norm_M512_s;
extern struct liquid_benchmark_s fskmod_norm_M1024_s;
extern struct liquid_benchmark_s fskmod_misc_M2_s;
extern struct liquid_benchmark_s fskmod_misc_M4_s;
extern struct liquid_benchmark_s fskmod_misc_M8_s;
extern struct liquid_benchmark_s fskmod_misc_M16_s;
extern struct liquid_benchmark_s fskmod_misc_M32_s;
extern struct liquid_benchmark_s fskmod_misc_M64_s;
extern struct liquid_benchmark_s fskmod_misc_M128_s;
extern struct liquid_benchmark_s fskmod_misc_M256_s;
extern struct liquid_benchmark_s fskmod_misc_M512_s;
extern struct liquid_benchmark_s fskmod_misc_M1024_s;
// ./src/modem/bench/gmskmodem_benchmark.c
extern struct liquid_benchmark_s gmskmodem_modulate_s;
extern struct liquid_benchmark_s gmskmodem_demodulate_s;
// ./src/modem/bench/modem_demodsoft_benchmark.c
extern struct liquid_benchmark_s modem_demodsoft_bpsk_s;
extern struct liquid_benchmark_s modem_demodsoft_qpsk_s;
extern struct liquid_benchmark_s modem_demodsoft_ook_s;
extern struct liquid_benchmark_s modem_demodsoft_sqam32_s;
extern struct liquid_benchmark_s modem_demodsoft_sqam128_s;
extern struct liquid_benchmark_s modem_demodsoft_ask2_s;
extern struct liquid_benchmark_s modem_demodsoft_ask4_s;
extern struct liquid_benchmark_s modem_demodsoft_ask8_s;
extern struct liquid_benchmark_s modem_demodsoft_ask16_s;
extern struct liquid_benchmark_s modem_demodsoft_psk2_s;
extern struct liquid_benchmark_s modem_demodsoft_psk4_s;
extern struct liquid_benchmark_s modem_demodsoft_psk8_s;
extern struct liquid_benchmark_s modem_demodsoft_psk16_s;
extern struct liquid_benchmark_s modem_demodsoft_psk32_s;
extern struct liquid_benchmark_s modem_demodsoft_psk64_s;
extern struct liquid_benchmark_s modem_demodsoft_dpsk2_s;
extern struct liquid_benchmark_s modem_demodsoft_dpsk4_s;
extern struct liquid_benchmark_s modem_demodsoft_dpsk8_s;
extern struct liquid_benchmark_s modem_demodsoft_dpsk16_s;
extern struct liquid_benchmark_s modem_demodsoft_dpsk32_s;
extern struct liquid_benchmark_s modem_demodsoft_dpsk64_s;
extern struct liquid_benchmark_s modem_demodsoft_qam4_s;
extern struct liquid_benchmark_s modem_demodsoft_qam8_s;
extern struct liquid_benchmark_s modem_demodsoft_qam16_s;
extern struct liquid_benchmark_s modem_demodsoft_qam32_s;
extern struct liquid_benchmark_s modem_demodsoft_qam64_s;
extern struct liquid_benchmark_s modem_demodsoft_qam128_s;
extern struct liquid_benchmark_s modem_demodsoft_qam256_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk4_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk8_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk16_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk32_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk64_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk128_s;
extern struct liquid_benchmark_s modem_demodsoft_apsk256_s;
extern struct liquid_benchmark_s modem_demodsoft_arbV29_s;
extern struct liquid_benchmark_s modem_demodsoft_arb16opt_s;
extern struct liquid_benchmark_s modem_demodsoft_arb32opt_s;
extern struct liquid_benchmark_s modem_demodsoft_arb64opt_s;
extern struct liquid_benchmark_s modem_demodsoft_arb128opt_s;
extern struct liquid_benchmark_s modem_demodsoft_arb256opt_s;
extern struct liquid_benchmark_s modem_demodsoft_arb64vt_s;
// ./src/modem/bench/modem_demodulate_benchmark.c
extern struct liquid_benchmark_s modem_demodulate_bpsk_s;
extern struct liquid_benchmark_s modem_demodulate_qpsk_s;
extern struct liquid_benchmark_s modem_demodulate_ook_s;
extern struct liquid_benchmark_s modem_demodulate_sqam32_s;
extern struct liquid_benchmark_s modem_demodulate_sqam128_s;
extern struct liquid_benchmark_s modem_demodulate_ask2_s;
extern struct liquid_benchmark_s modem_demodulate_ask4_s;
extern struct liquid_benchmark_s modem_demodulate_ask8_s;
extern struct liquid_benchmark_s modem_demodulate_ask16_s;
extern struct liquid_benchmark_s modem_demodulate_psk2_s;
extern struct liquid_benchmark_s modem_demodulate_psk4_s;
extern struct liquid_benchmark_s modem_demodulate_psk8_s;
extern struct liquid_benchmark_s modem_demodulate_psk16_s;
extern struct liquid_benchmark_s modem_demodulate_psk32_s;
extern struct liquid_benchmark_s modem_demodulate_psk64_s;
extern struct liquid_benchmark_s modem_demodulate_dpsk2_s;
extern struct liquid_benchmark_s modem_demodulate_dpsk4_s;
extern struct liquid_benchmark_s modem_demodulate_dpsk8_s;
extern struct liquid_benchmark_s modem_demodulate_dpsk16_s;
extern struct liquid_benchmark_s modem_demodulate_dpsk32_s;
extern struct liquid_benchmark_s modem_demodulate_dpsk64_s;
extern struct liquid_benchmark_s modem_demodulate_qam4_s;
extern struct liquid_benchmark_s modem_demodulate_qam8_s;
extern struct liquid_benchmark_s modem_demodulate_qam16_s;
extern struct liquid_benchmark_s modem_demodulate_qam32_s;
extern struct liquid_benchmark_s modem_demodulate_qam64_s;
extern struct liquid_benchmark_s modem_demodulate_qam128_s;
extern struct liquid_benchmark_s modem_demodulate_qam256_s;
extern struct liquid_benchmark_s modem_demodulate_apsk4_s;
extern struct liquid_benchmark_s modem_demodulate_apsk8_s;
extern struct liquid_benchmark_s modem_demodulate_apsk16_s;
extern struct liquid_benchmark_s modem_demodulate_apsk32_s;
extern struct liquid_benchmark_s modem_demodulate_apsk64_s;
extern struct liquid_benchmark_s modem_demodulate_apsk128_s;
extern struct liquid_benchmark_s modem_demodulate_apsk256_s;
extern struct liquid_benchmark_s modem_demodulate_arbV29_s;
extern struct liquid_benchmark_s modem_demodulate_arb16opt_s;
extern struct liquid_benchmark_s modem_demodulate_arb32opt_s;
extern struct liquid_benchmark_s modem_demodulate_arb64opt_s;
extern struct liquid_benchmark_s modem_demodulate_arb128opt_s;
extern struct liquid_benchmark_s modem_demodulate_arb256opt_s;
extern struct liquid_benchmark_s modem_demodulate_arb64vt_s;
// ./src/modem/bench/modem_modulate_benchmark.c
extern struct liquid_benchmark_s modem_modulate_bpsk_s;
extern struct liquid_benchmark_s modem_modulate_qpsk_s;
extern struct liquid_benchmark_s modem_modulate_ook_s;
extern struct liquid_benchmark_s modem_modulate_sqam32_s;
extern struct liquid_benchmark_s modem_modulate_sqam128_s;
extern struct liquid_benchmark_s modem_modulate_ask2_s;
extern struct liquid_benchmark_s modem_modulate_ask4_s;
extern struct liquid_benchmark_s modem_modulate_ask8_s;
extern struct liquid_benchmark_s modem_modulate_ask16_s;
extern struct liquid_benchmark_s modem_modulate_psk2_s;
extern struct liquid_benchmark_s modem_modulate_psk4_s;
extern struct liquid_benchmark_s modem_modulate_psk8_s;
extern struct liquid_benchmark_s modem_modulate_psk16_s;
extern struct liquid_benchmark_s modem_modulate_psk32_s;
extern struct liquid_benchmark_s modem_modulate_psk64_s;
extern struct liquid_benchmark_s modem_modulate_dpsk2_s;
extern struct liquid_benchmark_s modem_modulate_dpsk4_s;
extern struct liquid_benchmark_s modem_modulate_dpsk8_s;
extern struct liquid_benchmark_s modem_modulate_dpsk16_s;
extern struct liquid_benchmark_s modem_modulate_dpsk32_s;
extern struct liquid_benchmark_s modem_modulate_dpsk64_s;
extern struct liquid_benchmark_s modem_modulate_qam4_s;
extern struct liquid_benchmark_s modem_modulate_qam8_s;
extern struct liquid_benchmark_s modem_modulate_qam16_s;
extern struct liquid_benchmark_s modem_modulate_qam32_s;
extern struct liquid_benchmark_s modem_modulate_qam64_s;
extern struct liquid_benchmark_s modem_modulate_qam128_s;
extern struct liquid_benchmark_s modem_modulate_qam256_s;
extern struct liquid_benchmark_s modem_modulate_apsk4_s;
extern struct liquid_benchmark_s modem_modulate_apsk8_s;
extern struct liquid_benchmark_s modem_modulate_apsk16_s;
extern struct liquid_benchmark_s modem_modulate_apsk32_s;
extern struct liquid_benchmark_s modem_modulate_apsk64_s;
extern struct liquid_benchmark_s modem_modulate_apsk128_s;
extern struct liquid_benchmark_s modem_modulate_apsk256_s;
extern struct liquid_benchmark_s modem_modulate_arbV29_s;
extern struct liquid_benchmark_s modem_modulate_arb16opt_s;
extern struct liquid_benchmark_s modem_modulate_arb32opt_s;
extern struct liquid_benchmark_s modem_modulate_arb64opt_s;
extern struct liquid_benchmark_s modem_modulate_arb128opt_s;
extern struct liquid_benchmark_s modem_modulate_arb256opt_s;
extern struct liquid_benchmark_s modem_modulate_arb64vt_s;
// ./src/multichannel/bench/firpfbch2_crcf_benchmark.c
extern struct liquid_benchmark_s firpfbch2_crcf_a4_s;
extern struct liquid_benchmark_s firpfbch2_crcf_a16_s;
extern struct liquid_benchmark_s firpfbch2_crcf_a64_s;
extern struct liquid_benchmark_s firpfbch2_crcf_a256_s;
extern struct liquid_benchmark_s firpfbch2_crcf_a512_s;
extern struct liquid_benchmark_s firpfbch2_crcf_a1024_s;
extern struct liquid_benchmark_s firpfbch2_crcf_s4_s;
extern struct liquid_benchmark_s firpfbch2_crcf_s16_s;
extern struct liquid_benchmark_s firpfbch2_crcf_s64_s;
extern struct liquid_benchmark_s firpfbch2_crcf_s256_s;
extern struct liquid_benchmark_s firpfbch2_crcf_s512_s;
extern struct liquid_benchmark_s firpfbch2_crcf_s1024_s;
// ./src/multichannel/bench/firpfbch_crcf_benchmark.c
extern struct liquid_benchmark_s firpfbch_crcf_a4_s;
extern struct liquid_benchmark_s firpfbch_crcf_a16_s;
extern struct liquid_benchmark_s firpfbch_crcf_a64_s;
extern struct liquid_benchmark_s firpfbch_crcf_a256_s;
extern struct liquid_benchmark_s firpfbch_crcf_a512_s;
extern struct liquid_benchmark_s firpfbch_crcf_a1024_s;
// ./src/multichannel/bench/firpfbchr_crcf_benchmark.c
extern struct liquid_benchmark_s firpfbchr_crcf_M0064_P0063_s;
extern struct liquid_benchmark_s firpfbchr_crcf_M0128_P0127_s;
extern struct liquid_benchmark_s firpfbchr_crcf_M0256_P0255_s;
extern struct liquid_benchmark_s firpfbchr_crcf_M0512_P0511_s;
extern struct liquid_benchmark_s firpfbchr_crcf_M1024_P1023_s;
extern struct liquid_benchmark_s firpfbchr_crcf_M2048_P2047_s;
extern struct liquid_benchmark_s firpfbchr_crcf_M4096_P4095_s;
// ./src/multichannel/bench/ofdmframesync_acquire_benchmark.c
extern struct liquid_benchmark_s ofdmframesync_acquire_n64_s;
extern struct liquid_benchmark_s ofdmframesync_acquire_n128_s;
extern struct liquid_benchmark_s ofdmframesync_acquire_n256_s;
extern struct liquid_benchmark_s ofdmframesync_acquire_n512_s;
// ./src/multichannel/bench/ofdmframesync_rxsymbol_benchmark.c
extern struct liquid_benchmark_s ofdmframesync_rxsymbol_n64_s;
extern struct liquid_benchmark_s ofdmframesync_rxsymbol_n128_s;
extern struct liquid_benchmark_s ofdmframesync_rxsymbol_n256_s;
extern struct liquid_benchmark_s ofdmframesync_rxsymbol_n512_s;
// ./src/nco/bench/nco_benchmark.c
extern struct liquid_benchmark_s nco_sincos_s;
extern struct liquid_benchmark_s nco_mix_up_s;
extern struct liquid_benchmark_s nco_mix_block_up_s;
// ./src/nco/bench/vco_benchmark.c
extern struct liquid_benchmark_s vco_sincos_s;
extern struct liquid_benchmark_s vco_mix_up_s;
extern struct liquid_benchmark_s vco_mix_block_up_s;
// ./src/quantization/bench/compander_benchmark.c
extern struct liquid_benchmark_s compress_mulaw_s;
extern struct liquid_benchmark_s expand_mulaw_s;
// ./src/quantization/bench/quantizer_benchmark.c
extern struct liquid_benchmark_s quantize_adc_s;
extern struct liquid_benchmark_s quantize_dac_s;
// ./src/random/bench/random_benchmark.c
extern struct liquid_benchmark_s random_uniform_s;
extern struct liquid_benchmark_s random_normal_s;
extern struct liquid_benchmark_s random_complex_normal_s;
extern struct liquid_benchmark_s random_weibull_s;
extern struct liquid_benchmark_s random_ricek_s;
// ./src/sequence/bench/bsequence_benchmark.c
extern struct liquid_benchmark_s bsequence_xcorr_n16_s;
extern struct liquid_benchmark_s bsequence_xcorr_n64_s;
extern struct liquid_benchmark_s bsequence_xcorr_n256_s;
extern struct liquid_benchmark_s bsequence_xcorr_n1024_s;
// ./src/utility/bench/byte_utilities_benchmark.c
extern struct liquid_benchmark_s count_ones_s;
// ./src/vector/bench/vectorcf_benchmark.c
extern struct liquid_benchmark_s vectorcf_4_s;
extern struct liquid_benchmark_s vectorcf_16_s;
extern struct liquid_benchmark_s vectorcf_64_s;
extern struct liquid_benchmark_s vectorcf_256_s;
extern struct liquid_benchmark_s vectorcf_1024_s;
// ./src/vector/bench/vectorf_benchmark.c
extern struct liquid_benchmark_s vectorf_4_s;
extern struct liquid_benchmark_s vectorf_16_s;
extern struct liquid_benchmark_s vectorf_64_s;
extern struct liquid_benchmark_s vectorf_256_s;
extern struct liquid_benchmark_s vectorf_1024_s;

// compile benchmark registry
liquid_benchmark liquid_benchmarks[] =
{
    &agc_crcf_s,
    &cvsd_encode_s,
    &cvsd_decode_s,
    &cbuffercf_n16_s,
    &cbuffercf_n32_s,
    &cbuffercf_n64_s,
    &cbuffercf_n128_s,
    &cbuffercf_n256_s,
    &cbuffercf_n512_s,
    &cbuffercf_n1024_s,
    &windowcf_push_n16_s,
    &windowcf_push_n32_s,
    &windowcf_push_n64_s,
    &windowcf_push_n128_s,
    &windowcf_push_n256_s,
    &windowcf_read_n16_s,
    &windowcf_read_n32_s,
    &windowcf_read_n64_s,
    &windowcf_read_n128_s,
    &windowcf_read_n256_s,
    &logging_s,
    &dotprod_cccf_4_s,
    &dotprod_cccf_16_s,
    &dotprod_cccf_64_s,
    &dotprod_cccf_256_s,
    &dotprod_crcf_4_s,
    &dotprod_crcf_16_s,
    &dotprod_crcf_64_s,
    &dotprod_crcf_256_s,
    &dotprod_rrrf_4_s,
    &dotprod_rrrf_16_s,
    &dotprod_rrrf_64_s,
    &dotprod_rrrf_256_s,
    &sumsqcf_4_s,
    &sumsqcf_16_s,
    &sumsqcf_64_s,
    &sumsqcf_256_s,
    &sumsqf_4_s,
    &sumsqf_16_s,
    &sumsqf_64_s,
    &sumsqf_256_s,
    &eqlms_cccf_n4_s,
    &eqlms_cccf_n8_s,
    &eqlms_cccf_n16_s,
    &eqlms_cccf_n32_s,
    &eqlms_cccf_n64_s,
    &eqrls_cccf_n4_s,
    &eqrls_cccf_n8_s,
    &eqrls_cccf_n16_s,
    &eqrls_cccf_n32_s,
    &eqrls_cccf_n64_s,
    &asgramcf_64_s,
    &asgramcf_80_s,
    &asgramcf_96_s,
    &asgramcf_120_s,
    &asgramcf_64_autoscale_s,
    &asgramcf_80_autoscale_s,
    &asgramcf_96_autoscale_s,
    &asgramcf_120_autoscale_s,
    &fft_6_s,
    &fft_9_s,
    &fft_10_s,
    &fft_12_s,
    &fft_14_s,
    &fft_15_s,
    &fft_18_s,
    &fft_20_s,
    &fft_21_s,
    &fft_22_s,
    &fft_24_s,
    &fft_25_s,
    &fft_26_s,
    &fft_27_s,
    &fft_28_s,
    &fft_30_s,
    &fft_33_s,
    &fft_34_s,
    &fft_35_s,
    &fft_36_s,
    &fft_38_s,
    &fft_39_s,
    &fft_40_s,
    &fft_42_s,
    &fft_44_s,
    &fft_45_s,
    &fft_46_s,
    &fft_48_s,
    &fft_49_s,
    &fft_50_s,
    &fft_51_s,
    &fft_52_s,
    &fft_54_s,
    &fft_55_s,
    &fft_56_s,
    &fft_57_s,
    &fft_58_s,
    &fft_60_s,
    &fft_62_s,
    &fft_63_s,
    &fft_65_s,
    &fft_66_s,
    &fft_68_s,
    &fft_69_s,
    &fft_70_s,
    &fft_72_s,
    &fft_74_s,
    &fft_75_s,
    &fft_76_s,
    &fft_77_s,
    &fft_78_s,
    &fft_80_s,
    &fft_81_s,
    &fft_82_s,
    &fft_84_s,
    &fft_85_s,
    &fft_86_s,
    &fft_87_s,
    &fft_88_s,
    &fft_90_s,
    &fft_91_s,
    &fft_92_s,
    &fft_93_s,
    &fft_94_s,
    &fft_95_s,
    &fft_96_s,
    &fft_98_s,
    &fft_99_s,
    &fft_100_s,
    &fft_102_s,
    &fft_104_s,
    &fft_105_s,
    &fft_106_s,
    &fft_108_s,
    &fft_110_s,
    &fft_111_s,
    &fft_112_s,
    &fft_114_s,
    &fft_115_s,
    &fft_116_s,
    &fft_117_s,
    &fft_118_s,
    &fft_119_s,
    &fft_120_s,
    &fft_121_s,
    &fft_122_s,
    &fft_123_s,
    &fft_124_s,
    &fft_125_s,
    &fft_126_s,
    &fft_129_s,
    &fft_130_s,
    &fft_132_s,
    &fft_133_s,
    &fft_134_s,
    &fft_135_s,
    &fft_136_s,
    &fft_138_s,
    &fft_140_s,
    &fft_141_s,
    &fft_142_s,
    &fft_143_s,
    &fft_144_s,
    &fft_145_s,
    &fft_146_s,
    &fft_147_s,
    &fft_148_s,
    &fft_150_s,
    &fft_152_s,
    &fft_153_s,
    &fft_154_s,
    &fft_155_s,
    &fft_156_s,
    &fft_158_s,
    &fft_159_s,
    &fft_160_s,
    &fft_161_s,
    &fft_162_s,
    &fft_164_s,
    &fft_165_s,
    &fft_166_s,
    &fft_168_s,
    &fft_169_s,
    &fft_170_s,
    &fft_171_s,
    &fft_172_s,
    &fft_174_s,
    &fft_175_s,
    &fft_176_s,
    &fft_177_s,
    &fft_178_s,
    &fft_180_s,
    &fft_182_s,
    &fft_183_s,
    &fft_184_s,
    &fft_185_s,
    &fft_186_s,
    &fft_187_s,
    &fft_188_s,
    &fft_189_s,
    &fft_190_s,
    &fft_192_s,
    &fft_194_s,
    &fft_195_s,
    &fft_196_s,
    &fft_198_s,
    &fft_200_s,
    &fft_201_s,
    &fft_202_s,
    &fft_203_s,
    &fft_204_s,
    &fft_205_s,
    &fft_206_s,
    &fft_207_s,
    &fft_208_s,
    &fft_209_s,
    &fft_210_s,
    &fft_212_s,
    &fft_213_s,
    &fft_214_s,
    &fft_215_s,
    &fft_216_s,
    &fft_217_s,
    &fft_218_s,
    &fft_219_s,
    &fft_220_s,
    &fft_221_s,
    &fft_222_s,
    &fft_224_s,
    &fft_225_s,
    &fft_226_s,
    &fft_228_s,
    &fft_230_s,
    &fft_231_s,
    &fft_232_s,
    &fft_234_s,
    &fft_235_s,
    &fft_236_s,
    &fft_237_s,
    &fft_238_s,
    &fft_240_s,
    &fft_242_s,
    &fft_243_s,
    &fft_244_s,
    &fft_245_s,
    &fft_246_s,
    &fft_247_s,
    &fft_248_s,
    &fft_249_s,
    &fft_250_s,
    &fft_252_s,
    &fft_253_s,
    &fft_254_s,
    &fft_255_s,
    &fft_3_s,
    &fft_5_s,
    &fft_7_s,
    &fft_11_s,
    &fft_13_s,
    &fft_17_s,
    &fft_19_s,
    &fft_23_s,
    &fft_29_s,
    &fft_31_s,
    &fft_37_s,
    &fft_41_s,
    &fft_43_s,
    &fft_47_s,
    &fft_53_s,
    &fft_59_s,
    &fft_61_s,
    &fft_67_s,
    &fft_71_s,
    &fft_73_s,
    &fft_79_s,
    &fft_83_s,
    &fft_89_s,
    &fft_97_s,
    &fft_101_s,
    &fft_103_s,
    &fft_107_s,
    &fft_109_s,
    &fft_113_s,
    &fft_127_s,
    &fft_131_s,
    &fft_137_s,
    &fft_139_s,
    &fft_149_s,
    &fft_151_s,
    &fft_157_s,
    &fft_163_s,
    &fft_167_s,
    &fft_173_s,
    &fft_179_s,
    &fft_181_s,
    &fft_191_s,
    &fft_193_s,
    &fft_197_s,
    &fft_199_s,
    &fft_211_s,
    &fft_223_s,
    &fft_227_s,
    &fft_229_s,
    &fft_233_s,
    &fft_239_s,
    &fft_241_s,
    &fft_251_s,
    &fft_257_s,
    &fft_263_s,
    &fft_269_s,
    &fft_271_s,
    &fft_277_s,
    &fft_281_s,
    &fft_283_s,
    &fft_293_s,
    &fft_307_s,
    &fft_311_s,
    &fft_313_s,
    &fft_317_s,
    &fft_331_s,
    &fft_337_s,
    &fft_347_s,
    &fft_349_s,
    &fft_353_s,
    &fft_359_s,
    &fft_367_s,
    &fft_373_s,
    &fft_379_s,
    &fft_383_s,
    &fft_389_s,
    &fft_397_s,
    &fft_401_s,
    &fft_409_s,
    &fft_419_s,
    &fft_421_s,
    &fft_431_s,
    &fft_433_s,
    &fft_439_s,
    &fft_443_s,
    &fft_449_s,
    &fft_457_s,
    &fft_461_s,
    &fft_463_s,
    &fft_467_s,
    &fft_479_s,
    &fft_487_s,
    &fft_491_s,
    &fft_499_s,
    &fft_503_s,
    &fft_509_s,
    &fft_REDFT00_128_s,
    &fft_REDFT01_128_s,
    &fft_REDFT10_128_s,
    &fft_REDFT11_128_s,
    &fft_RODFT00_128_s,
    &fft_RODFT01_128_s,
    &fft_RODFT10_128_s,
    &fft_RODFT11_128_s,
    &fft_REDFT00_127_s,
    &fft_REDFT01_127_s,
    &fft_REDFT10_127_s,
    &fft_REDFT11_127_s,
    &fft_RODFT00_127_s,
    &fft_RODFT01_127_s,
    &fft_RODFT10_127_s,
    &fft_RODFT11_127_s,
    &fft_2_s,
    &fft_4_s,
    &fft_8_s,
    &fft_16_s,
    &fft_32_s,
    &fft_64_s,
    &fft_128_s,
    &fft_256_s,
    &fft_512_s,
    &fft_1024_s,
    &fft_2048_s,
    &fft_4096_s,
    &fft_8192_s,
    &fft_16384_s,
    &fft_32768_s,
    &spgramcf_1200_s,
    &spgramcf_9600_s,
    &spgramcf_76800_s,
    &spgramcf_614400_s,
    &fftfilt_crcf_4_s,
    &fftfilt_crcf_8_s,
    &fftfilt_crcf_16_s,
    &fftfilt_crcf_32_s,
    &fftfilt_crcf_64_s,
    &firdecim_crcf_m2_h8_s,
    &firdecim_crcf_m4_h16_s,
    &firdecim_crcf_m8_h32_s,
    &firdecim_crcf_m16_h64_s,
    &firdecim_crcf_m32_h128_s,
    &firfilt_crcf_4_s,
    &firfilt_crcf_8_s,
    &firfilt_crcf_16_s,
    &firfilt_crcf_32_s,
    &firfilt_crcf_64_s,
    &firhilbf_decim_m3_s,
    &firhilbf_decim_m5_s,
    &firhilbf_decim_m9_s,
    &firhilbf_decim_m13_s,
    &firinterp_crcf_m2_h8_s,
    &firinterp_crcf_m4_h16_s,
    &firinterp_crcf_m8_h32_s,
    &firinterp_crcf_m16_h64_s,
    &firinterp_crcf_m32_h128_s,
    &iirdecim_crcf_M2_s,
    &iirdecim_crcf_M4_s,
    &iirdecim_crcf_M8_s,
    &iirdecim_crcf_M16_s,
    &iirdecim_crcf_M32_s,
    &iirfilt_crcf_4_s,
    &iirfilt_crcf_8_s,
    &iirfilt_crcf_16_s,
    &iirfilt_crcf_32_s,
    &iirfilt_crcf_64_s,
    &iirfilt_crcf_sos_4_s,
    &iirfilt_crcf_sos_8_s,
    &iirfilt_crcf_sos_16_s,
    &iirfilt_crcf_sos_32_s,
    &iirfilt_crcf_sos_64_s,
    &iirfilt_crcf_dcblock_s,
    &iirinterp_crcf_M2_s,
    &iirinterp_crcf_M4_s,
    &iirinterp_crcf_M8_s,
    &iirinterp_crcf_M16_s,
    &iirinterp_crcf_M32_s,
    &resamp2_crcf_decim_m2_s,
    &resamp2_crcf_decim_m4_s,
    &resamp2_crcf_decim_m8_s,
    &resamp2_crcf_decim_m16_s,
    &resamp2_crcf_decim_m32_s,
    &resamp2_crcf_decim_m64_s,
    &resamp2_crcf_decim_m128_s,
    &resamp2_crcf_decim_m256_s,
    &resamp2_crcf_interp_m2_s,
    &resamp2_crcf_interp_m4_s,
    &resamp2_crcf_interp_m8_s,
    &resamp2_crcf_interp_m16_s,
    &resamp2_crcf_interp_m32_s,
    &resamp2_crcf_interp_m64_s,
    &resamp2_crcf_interp_m128_s,
    &resamp2_crcf_interp_m256_s,
    &resamp_crcf_P17_Q1_s,
    &resamp_crcf_P17_Q2_s,
    &resamp_crcf_P17_Q4_s,
    &resamp_crcf_P17_Q8_s,
    &resamp_crcf_P17_Q16_s,
    &resamp_crcf_P17_Q32_s,
    &resamp_crcf_P17_Q64_s,
    &resamp_crcf_P17_Q128_s,
    &resamp_crcf_P17_Q256_s,
    &rresamp_crcf_P17_Q1_s,
    &rresamp_crcf_P17_Q2_s,
    &rresamp_crcf_P17_Q4_s,
    &rresamp_crcf_P17_Q8_s,
    &rresamp_crcf_P17_Q16_s,
    &rresamp_crcf_P17_Q32_s,
    &rresamp_crcf_P17_Q64_s,
    &rresamp_crcf_P17_Q128_s,
    &rresamp_crcf_P17_Q256_s,
    &symsync_crcf_k2_m2_s,
    &symsync_crcf_k2_m4_s,
    &symsync_crcf_k2_m8_s,
    &symsync_crcf_k2_m16_s,
    &bpacketsync_s,
    &bpresync_cccf_16_s,
    &bpresync_cccf_32_s,
    &bpresync_cccf_64_s,
    &bpresync_cccf_128_s,
    &bpresync_cccf_256_s,
    &bsync_cccf_16_s,
    &bsync_cccf_32_s,
    &bsync_cccf_64_s,
    &bsync_cccf_128_s,
    &bsync_cccf_256_s,
    &detector_cccf_16_s,
    &detector_cccf_32_s,
    &detector_cccf_64_s,
    &detector_cccf_128_s,
    &detector_cccf_256_s,
    &flexframesync_s,
    &framesync64_s,
    &gmskframesync_s,
    &gmskframesync_noise_s,
    &presync_cccf_16_s,
    &presync_cccf_32_s,
    &presync_cccf_64_s,
    &presync_cccf_128_s,
    &presync_cccf_256_s,
    &qdetector_cccf_16_s,
    &qdetector_cccf_32_s,
    &qdetector_cccf_64_s,
    &qdetector_cccf_128_s,
    &qdetector_cccf_256_s,
    &qdetector_cccf_512_s,
    &qdetector_cccf_1024_s,
    &qdetector_cccf_2048_s,
    &qdetector_cccf_4096_s,
    &qdetector_cccf_8192_s,
    &qdetector_cccf_16384_s,
    &polyfit_q3_n8_s,
    &polyfit_q3_n16_s,
    &polyfit_q3_n32_s,
    &polyfit_q3_n64_s,
    &polyfit_q3_n128_s,
    &matrixf_inv_n2_s,
    &matrixf_inv_n4_s,
    &matrixf_inv_n8_s,
    &matrixf_inv_n16_s,
    &matrixf_inv_n32_s,
    &matrixf_inv_n64_s,
    &matrixf_linsolve_n2_s,
    &matrixf_linsolve_n4_s,
    &matrixf_linsolve_n8_s,
    &matrixf_linsolve_n16_s,
    &matrixf_linsolve_n32_s,
    &matrixf_linsolve_n64_s,
    &matrixf_mul_n2_s,
    &matrixf_mul_n4_s,
    &matrixf_mul_n8_s,
    &matrixf_mul_n16_s,
    &matrixf_mul_n32_s,
    &matrixf_mul_n64_s,
    &smatrixf_mul_n32_s,
    &smatrixf_mul_n64_s,
    &smatrixf_mul_n128_s,
    &smatrixf_mul_n256_s,
    &smatrixf_mul_n512_s,
    &freqdem_s,
    &freqmod_s,
    &fskdem_norm_M2_s,
    &fskdem_norm_M4_s,
    &fskdem_norm_M8_s,
    &fskdem_norm_M16_s,
    &fskdem_norm_M32_s,
    &fskdem_norm_M64_s,
    &fskdem_norm_M128_s,
    &fskdem_norm_M256_s,
    &fskdem_norm_M512_s,
    &fskdem_norm_M1024_s,
    &fskdem_misc_M2_s,
    &fskdem_misc_M4_s,
    &fskdem_misc_M8_s,
    &fskdem_misc_M16_s,
    &fskdem_misc_M32_s,
    &fskdem_misc_M64_s,
    &fskdem_misc_M128_s,
    &fskdem_misc_M256_s,
    &fskdem_misc_M512_s,
    &fskdem_misc_M1024_s,
    &fskmod_norm_M2_s,
    &fskmod_norm_M4_s,
    &fskmod_norm_M8_s,
    &fskmod_norm_M16_s,
    &fskmod_norm_M32_s,
    &fskmod_norm_M64_s,
    &fskmod_norm_M128_s,
    &fskmod_norm_M256_s,
    &fskmod_norm_M512_s,
    &fskmod_norm_M1024_s,
    &fskmod_misc_M2_s,
    &fskmod_misc_M4_s,
    &fskmod_misc_M8_s,
    &fskmod_misc_M16_s,
    &fskmod_misc_M32_s,
    &fskmod_misc_M64_s,
    &fskmod_misc_M128_s,
    &fskmod_misc_M256_s,
    &fskmod_misc_M512_s,
    &fskmod_misc_M1024_s,
    &gmskmodem_modulate_s,
    &gmskmodem_demodulate_s,
    &modem_demodsoft_bpsk_s,
    &modem_demodsoft_qpsk_s,
    &modem_demodsoft_ook_s,
    &modem_demodsoft_sqam32_s,
    &modem_demodsoft_sqam128_s,
    &modem_demodsoft_ask2_s,
    &modem_demodsoft_ask4_s,
    &modem_demodsoft_ask8_s,
    &modem_demodsoft_ask16_s,
    &modem_demodsoft_psk2_s,
    &modem_demodsoft_psk4_s,
    &modem_demodsoft_psk8_s,
    &modem_demodsoft_psk16_s,
    &modem_demodsoft_psk32_s,
    &modem_demodsoft_psk64_s,
    &modem_demodsoft_dpsk2_s,
    &modem_demodsoft_dpsk4_s,
    &modem_demodsoft_dpsk8_s,
    &modem_demodsoft_dpsk16_s,
    &modem_demodsoft_dpsk32_s,
    &modem_demodsoft_dpsk64_s,
    &modem_demodsoft_qam4_s,
    &modem_demodsoft_qam8_s,
    &modem_demodsoft_qam16_s,
    &modem_demodsoft_qam32_s,
    &modem_demodsoft_qam64_s,
    &modem_demodsoft_qam128_s,
    &modem_demodsoft_qam256_s,
    &modem_demodsoft_apsk4_s,
    &modem_demodsoft_apsk8_s,
    &modem_demodsoft_apsk16_s,
    &modem_demodsoft_apsk32_s,
    &modem_demodsoft_apsk64_s,
    &modem_demodsoft_apsk128_s,
    &modem_demodsoft_apsk256_s,
    &modem_demodsoft_arbV29_s,
    &modem_demodsoft_arb16opt_s,
    &modem_demodsoft_arb32opt_s,
    &modem_demodsoft_arb64opt_s,
    &modem_demodsoft_arb128opt_s,
    &modem_demodsoft_arb256opt_s,
    &modem_demodsoft_arb64vt_s,
    &modem_demodulate_bpsk_s,
    &modem_demodulate_qpsk_s,
    &modem_demodulate_ook_s,
    &modem_demodulate_sqam32_s,
    &modem_demodulate_sqam128_s,
    &modem_demodulate_ask2_s,
    &modem_demodulate_ask4_s,
    &modem_demodulate_ask8_s,
    &modem_demodulate_ask16_s,
    &modem_demodulate_psk2_s,
    &modem_demodulate_psk4_s,
    &modem_demodulate_psk8_s,
    &modem_demodulate_psk16_s,
    &modem_demodulate_psk32_s,
    &modem_demodulate_psk64_s,
    &modem_demodulate_dpsk2_s,
    &modem_demodulate_dpsk4_s,
    &modem_demodulate_dpsk8_s,
    &modem_demodulate_dpsk16_s,
    &modem_demodulate_dpsk32_s,
    &modem_demodulate_dpsk64_s,
    &modem_demodulate_qam4_s,
    &modem_demodulate_qam8_s,
    &modem_demodulate_qam16_s,
    &modem_demodulate_qam32_s,
    &modem_demodulate_qam64_s,
    &modem_demodulate_qam128_s,
    &modem_demodulate_qam256_s,
    &modem_demodulate_apsk4_s,
    &modem_demodulate_apsk8_s,
    &modem_demodulate_apsk16_s,
    &modem_demodulate_apsk32_s,
    &modem_demodulate_apsk64_s,
    &modem_demodulate_apsk128_s,
    &modem_demodulate_apsk256_s,
    &modem_demodulate_arbV29_s,
    &modem_demodulate_arb16opt_s,
    &modem_demodulate_arb32opt_s,
    &modem_demodulate_arb64opt_s,
    &modem_demodulate_arb128opt_s,
    &modem_demodulate_arb256opt_s,
    &modem_demodulate_arb64vt_s,
    &modem_modulate_bpsk_s,
    &modem_modulate_qpsk_s,
    &modem_modulate_ook_s,
    &modem_modulate_sqam32_s,
    &modem_modulate_sqam128_s,
    &modem_modulate_ask2_s,
    &modem_modulate_ask4_s,
    &modem_modulate_ask8_s,
    &modem_modulate_ask16_s,
    &modem_modulate_psk2_s,
    &modem_modulate_psk4_s,
    &modem_modulate_psk8_s,
    &modem_modulate_psk16_s,
    &modem_modulate_psk32_s,
    &modem_modulate_psk64_s,
    &modem_modulate_dpsk2_s,
    &modem_modulate_dpsk4_s,
    &modem_modulate_dpsk8_s,
    &modem_modulate_dpsk16_s,
    &modem_modulate_dpsk32_s,
    &modem_modulate_dpsk64_s,
    &modem_modulate_qam4_s,
    &modem_modulate_qam8_s,
    &modem_modulate_qam16_s,
    &modem_modulate_qam32_s,
    &modem_modulate_qam64_s,
    &modem_modulate_qam128_s,
    &modem_modulate_qam256_s,
    &modem_modulate_apsk4_s,
    &modem_modulate_apsk8_s,
    &modem_modulate_apsk16_s,
    &modem_modulate_apsk32_s,
    &modem_modulate_apsk64_s,
    &modem_modulate_apsk128_s,
    &modem_modulate_apsk256_s,
    &modem_modulate_arbV29_s,
    &modem_modulate_arb16opt_s,
    &modem_modulate_arb32opt_s,
    &modem_modulate_arb64opt_s,
    &modem_modulate_arb128opt_s,
    &modem_modulate_arb256opt_s,
    &modem_modulate_arb64vt_s,
    &firpfbch2_crcf_a4_s,
    &firpfbch2_crcf_a16_s,
    &firpfbch2_crcf_a64_s,
    &firpfbch2_crcf_a256_s,
    &firpfbch2_crcf_a512_s,
    &firpfbch2_crcf_a1024_s,
    &firpfbch2_crcf_s4_s,
    &firpfbch2_crcf_s16_s,
    &firpfbch2_crcf_s64_s,
    &firpfbch2_crcf_s256_s,
    &firpfbch2_crcf_s512_s,
    &firpfbch2_crcf_s1024_s,
    &firpfbch_crcf_a4_s,
    &firpfbch_crcf_a16_s,
    &firpfbch_crcf_a64_s,
    &firpfbch_crcf_a256_s,
    &firpfbch_crcf_a512_s,
    &firpfbch_crcf_a1024_s,
    &firpfbchr_crcf_M0064_P0063_s,
    &firpfbchr_crcf_M0128_P0127_s,
    &firpfbchr_crcf_M0256_P0255_s,
    &firpfbchr_crcf_M0512_P0511_s,
    &firpfbchr_crcf_M1024_P1023_s,
    &firpfbchr_crcf_M2048_P2047_s,
    &firpfbchr_crcf_M4096_P4095_s,
    &ofdmframesync_acquire_n64_s,
    &ofdmframesync_acquire_n128_s,
    &ofdmframesync_acquire_n256_s,
    &ofdmframesync_acquire_n512_s,
    &ofdmframesync_rxsymbol_n64_s,
    &ofdmframesync_rxsymbol_n128_s,
    &ofdmframesync_rxsymbol_n256_s,
    &ofdmframesync_rxsymbol_n512_s,
    &nco_sincos_s,
    &nco_mix_up_s,
    &nco_mix_block_up_s,
    &vco_sincos_s,
    &vco_mix_up_s,
    &vco_mix_block_up_s,
    &compress_mulaw_s,
    &expand_mulaw_s,
    &quantize_adc_s,
    &quantize_dac_s,
    &random_uniform_s,
    &random_normal_s,
    &random_complex_normal_s,
    &random_weibull_s,
    &random_ricek_s,
    &bsequence_xcorr_n16_s,
    &bsequence_xcorr_n64_s,
    &bsequence_xcorr_n256_s,
    &bsequence_xcorr_n1024_s,
    &count_ones_s,
    &vectorcf_4_s,
    &vectorcf_16_s,
    &vectorcf_64_s,
    &vectorcf_256_s,
    &vectorcf_1024_s,
    &vectorf_4_s,
    &vectorf_16_s,
    &vectorf_64_s,
    &vectorf_256_s,
    &vectorf_1024_s,
    NULL
};

#endif // __LIQUID_BENCHMARK_REGISTRY_H__


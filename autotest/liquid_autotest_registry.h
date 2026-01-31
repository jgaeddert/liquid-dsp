#ifndef __LIQUID_AUTOTEST_REGISTRY_H__
#define __LIQUID_AUTOTEST_REGISTRY_H__

#include "liquid.autotest.h"

// ./autotest/null_autotest.c
extern struct liquid_autotest_s null_s;
// ./autotest/libliquid_autotest.c
extern struct liquid_autotest_s libliquid_version_s;
// ./src/fec/tests/fec_hamming128_autotest.c
extern struct liquid_autotest_s hamming128_codec_s;
extern struct liquid_autotest_s hamming128_codec_soft_s;
// ./src/fec/tests/fec_secded2216_autotest.c
extern struct liquid_autotest_s secded2216_codec_e0_s;
extern struct liquid_autotest_s secded2216_codec_e1_s;
extern struct liquid_autotest_s secded2216_codec_e2_s;
// ./src/fec/tests/fec_hamming3126_autotest.c
extern struct liquid_autotest_s hamming3126_codec_s;
// ./src/fec/tests/packetizer_autotest.c
extern struct liquid_autotest_s packetizer_n16_0_0_s;
extern struct liquid_autotest_s packetizer_n16_0_1_s;
extern struct liquid_autotest_s packetizer_n16_0_2_s;
// ./src/fec/tests/fec_copy_autotest.c
extern struct liquid_autotest_s fec_copy_r3_s;
extern struct liquid_autotest_s fec_copy_r5_s;
extern struct liquid_autotest_s fec_copy_h74_s;
extern struct liquid_autotest_s fec_copy_h84_s;
extern struct liquid_autotest_s fec_copy_h128_s;
extern struct liquid_autotest_s fec_copy_g2412_s;
extern struct liquid_autotest_s fec_copy_secded2216_s;
extern struct liquid_autotest_s fec_copy_secded3932_s;
extern struct liquid_autotest_s fec_copy_secded7264_s;
extern struct liquid_autotest_s fec_copy_v27_s;
extern struct liquid_autotest_s fec_copy_v29_s;
extern struct liquid_autotest_s fec_copy_v39_s;
extern struct liquid_autotest_s fec_copy_v615_s;
extern struct liquid_autotest_s fec_copy_v27p23_s;
extern struct liquid_autotest_s fec_copy_v27p34_s;
extern struct liquid_autotest_s fec_copy_v27p45_s;
extern struct liquid_autotest_s fec_copy_v27p56_s;
extern struct liquid_autotest_s fec_copy_v27p67_s;
extern struct liquid_autotest_s fec_copy_v27p78_s;
extern struct liquid_autotest_s fec_copy_v29p23_s;
extern struct liquid_autotest_s fec_copy_v29p34_s;
extern struct liquid_autotest_s fec_copy_v29p45_s;
extern struct liquid_autotest_s fec_copy_v29p56_s;
extern struct liquid_autotest_s fec_copy_v29p67_s;
extern struct liquid_autotest_s fec_copy_v29p78_s;
extern struct liquid_autotest_s fec_copy_rs8_s;
// ./src/fec/tests/fec_hamming1511_autotest.c
extern struct liquid_autotest_s hamming1511_codec_s;
// ./src/fec/tests/fec_rep3_autotest.c
extern struct liquid_autotest_s rep3_codec_s;
// ./src/fec/tests/fec_reedsolomon_autotest.c
extern struct liquid_autotest_s reedsolomon_223_255_s;
// ./src/fec/tests/fec_secded7264_autotest.c
extern struct liquid_autotest_s secded7264_codec_e0_s;
extern struct liquid_autotest_s secded7264_codec_e1_s;
extern struct liquid_autotest_s secded7264_codec_e2_s;
// ./src/fec/tests/fec_hamming84_autotest.c
extern struct liquid_autotest_s hamming84_codec_s;
extern struct liquid_autotest_s hamming84_codec_soft_s;
// ./src/fec/tests/packetizer_copy_autotest.c
extern struct liquid_autotest_s packetizer_copy_s;
// ./src/fec/tests/fec_autotest.c
extern struct liquid_autotest_s fec_r3_s;
extern struct liquid_autotest_s fec_r5_s;
extern struct liquid_autotest_s fec_h74_s;
extern struct liquid_autotest_s fec_h84_s;
extern struct liquid_autotest_s fec_h128_s;
extern struct liquid_autotest_s fec_g2412_s;
extern struct liquid_autotest_s fec_secded2216_s;
extern struct liquid_autotest_s fec_secded3932_s;
extern struct liquid_autotest_s fec_secded7264_s;
extern struct liquid_autotest_s fec_v27_s;
extern struct liquid_autotest_s fec_v29_s;
extern struct liquid_autotest_s fec_v39_s;
extern struct liquid_autotest_s fec_v615_s;
extern struct liquid_autotest_s fec_v27p23_s;
extern struct liquid_autotest_s fec_v27p34_s;
extern struct liquid_autotest_s fec_v27p45_s;
extern struct liquid_autotest_s fec_v27p56_s;
extern struct liquid_autotest_s fec_v27p67_s;
extern struct liquid_autotest_s fec_v27p78_s;
extern struct liquid_autotest_s fec_v29p23_s;
extern struct liquid_autotest_s fec_v29p34_s;
extern struct liquid_autotest_s fec_v29p45_s;
extern struct liquid_autotest_s fec_v29p56_s;
extern struct liquid_autotest_s fec_v29p67_s;
extern struct liquid_autotest_s fec_v29p78_s;
extern struct liquid_autotest_s fec_rs8_s;
// ./src/fec/tests/fec_golay2412_autotest.c
extern struct liquid_autotest_s golay2412_codec_s;
// ./src/fec/tests/fec_soft_autotest.c
extern struct liquid_autotest_s fecsoft_r3_s;
extern struct liquid_autotest_s fecsoft_r5_s;
extern struct liquid_autotest_s fecsoft_h74_s;
extern struct liquid_autotest_s fecsoft_h84_s;
extern struct liquid_autotest_s fecsoft_h128_s;
extern struct liquid_autotest_s fecsoft_v27_s;
extern struct liquid_autotest_s fecsoft_v29_s;
extern struct liquid_autotest_s fecsoft_v39_s;
extern struct liquid_autotest_s fecsoft_v615_s;
extern struct liquid_autotest_s fecsoft_v27p23_s;
extern struct liquid_autotest_s fecsoft_v27p34_s;
extern struct liquid_autotest_s fecsoft_v27p45_s;
extern struct liquid_autotest_s fecsoft_v27p56_s;
extern struct liquid_autotest_s fecsoft_v27p67_s;
extern struct liquid_autotest_s fecsoft_v27p78_s;
extern struct liquid_autotest_s fecsoft_v29p23_s;
extern struct liquid_autotest_s fecsoft_v29p34_s;
extern struct liquid_autotest_s fecsoft_v29p45_s;
extern struct liquid_autotest_s fecsoft_v29p56_s;
extern struct liquid_autotest_s fecsoft_v29p67_s;
extern struct liquid_autotest_s fecsoft_v29p78_s;
extern struct liquid_autotest_s fecsoft_rs8_s;
// ./src/fec/tests/fec_config_autotest.c
extern struct liquid_autotest_s fec_config_s;
extern struct liquid_autotest_s fec_str2fec_s;
extern struct liquid_autotest_s fec_is_convolutional_s;
extern struct liquid_autotest_s fec_is_punctured_s;
extern struct liquid_autotest_s fec_is_reedsolomon_s;
extern struct liquid_autotest_s fec_is_hamming_s;
// ./src/fec/tests/fec_hamming74_autotest.c
extern struct liquid_autotest_s hamming74_codec_s;
extern struct liquid_autotest_s hamming74_codec_soft_s;
// ./src/fec/tests/crc_autotest.c
extern struct liquid_autotest_s reverse_byte_s;
extern struct liquid_autotest_s reverse_uint16_s;
extern struct liquid_autotest_s reverse_uint32_s;
extern struct liquid_autotest_s checksum_s;
extern struct liquid_autotest_s crc8_s;
extern struct liquid_autotest_s crc16_s;
extern struct liquid_autotest_s crc24_s;
extern struct liquid_autotest_s crc32_s;
extern struct liquid_autotest_s crc_config_s;
// ./src/fec/tests/fec_rep5_autotest.c
extern struct liquid_autotest_s rep5_codec_s;
// ./src/fec/tests/fec_secded3932_autotest.c
extern struct liquid_autotest_s secded3932_codec_e0_s;
extern struct liquid_autotest_s secded3932_codec_e1_s;
extern struct liquid_autotest_s secded3932_codec_e2_s;
// ./src/fec/tests/interleaver_autotest.c
extern struct liquid_autotest_s interleaver_hard_8_s;
extern struct liquid_autotest_s interleaver_hard_16_s;
extern struct liquid_autotest_s interleaver_hard_64_s;
extern struct liquid_autotest_s interleaver_hard_256_s;
extern struct liquid_autotest_s interleaver_soft_8_s;
extern struct liquid_autotest_s interleaver_soft_16_s;
extern struct liquid_autotest_s interleaver_soft_64_s;
extern struct liquid_autotest_s interleaver_soft_256_s;
// ./src/multichannel/tests/firpfbchr_crcf_autotest.c
extern struct liquid_autotest_s firpfbchr_crcf_s;
extern struct liquid_autotest_s firpfbchr_crcf_config_s;
// ./src/multichannel/tests/firpfbch_crcf_analyzer_autotest.c
extern struct liquid_autotest_s firpfbch_crcf_analysis_s;
// ./src/multichannel/tests/ofdmframe_autotest.c
extern struct liquid_autotest_s ofdmframesync_acquire_n64_s;
extern struct liquid_autotest_s ofdmframesync_acquire_n128_s;
extern struct liquid_autotest_s ofdmframesync_acquire_n256_s;
extern struct liquid_autotest_s ofdmframesync_acquire_n512_s;
extern struct liquid_autotest_s ofdmframe_common_config_s;
extern struct liquid_autotest_s ofdmframegen_config_s;
extern struct liquid_autotest_s ofdmframesync_config_s;
// ./src/multichannel/tests/firpfbch2_crcf_autotest.c
extern struct liquid_autotest_s firpfbch2_crcf_n8_s;
extern struct liquid_autotest_s firpfbch2_crcf_n16_s;
extern struct liquid_autotest_s firpfbch2_crcf_n32_s;
extern struct liquid_autotest_s firpfbch2_crcf_n64_s;
extern struct liquid_autotest_s firpfbch2_crcf_copy_s;
extern struct liquid_autotest_s firpfbch2_crcf_config_s;
// ./src/multichannel/tests/firpfbch_crcf_autotest.c
extern struct liquid_autotest_s firpfbch_crcf_config_s;
// ./src/multichannel/tests/firpfbch_crcf_synthesizer_autotest.c
extern struct liquid_autotest_s firpfbch_crcf_synthesis_s;
// ./src/optim/tests/gasearch_autotest.c
extern struct liquid_autotest_s gasearch_peak_s;
extern struct liquid_autotest_s chromosome_config_s;
extern struct liquid_autotest_s gasearch_config_s;
// ./src/optim/tests/qs1dsearch_autotest.c
extern struct liquid_autotest_s qs1dsearch_min_01_s;
extern struct liquid_autotest_s qs1dsearch_min_02_s;
extern struct liquid_autotest_s qs1dsearch_min_03_s;
extern struct liquid_autotest_s qs1dsearch_min_05_s;
extern struct liquid_autotest_s qs1dsearch_min_06_s;
extern struct liquid_autotest_s qs1dsearch_min_07_s;
extern struct liquid_autotest_s qs1dsearch_min_08_s;
extern struct liquid_autotest_s qs1dsearch_min_10_s;
extern struct liquid_autotest_s qs1dsearch_min_11_s;
extern struct liquid_autotest_s qs1dsearch_min_12_s;
extern struct liquid_autotest_s qs1dsearch_min_13_s;
extern struct liquid_autotest_s qs1dsearch_max_01_s;
extern struct liquid_autotest_s qs1dsearch_max_02_s;
extern struct liquid_autotest_s qs1dsearch_max_03_s;
extern struct liquid_autotest_s qs1dsearch_max_05_s;
extern struct liquid_autotest_s qs1dsearch_max_06_s;
extern struct liquid_autotest_s qs1dsearch_max_07_s;
extern struct liquid_autotest_s qs1dsearch_max_08_s;
extern struct liquid_autotest_s qs1dsearch_max_10_s;
extern struct liquid_autotest_s qs1dsearch_max_11_s;
extern struct liquid_autotest_s qs1dsearch_max_12_s;
extern struct liquid_autotest_s qs1dsearch_max_13_s;
extern struct liquid_autotest_s qs1dsearch_config_s;
// ./src/optim/tests/gradsearch_autotest.c
extern struct liquid_autotest_s gradsearch_rosenbrock_s;
extern struct liquid_autotest_s gradsearch_maxutility_s;
// ./src/optim/tests/utility_autotest.c
extern struct liquid_autotest_s optim_rosenbrock_s;
// ./src/optim/tests/qnsearch_autotest.c
extern struct liquid_autotest_s qnsearch_rosenbrock_s;
extern struct liquid_autotest_s qnsearch_config_s;
// ./src/dotprod/tests/dotprod_crcf_autotest.c
extern struct liquid_autotest_s dotprod_crcf_rand01_s;
extern struct liquid_autotest_s dotprod_crcf_rand02_s;
extern struct liquid_autotest_s dotprod_crcf_struct_vs_ordinal_s;
// ./src/dotprod/tests/sumsqcf_autotest.c
extern struct liquid_autotest_s sumsqcf_3_s;
extern struct liquid_autotest_s sumsqcf_4_s;
extern struct liquid_autotest_s sumsqcf_7_s;
extern struct liquid_autotest_s sumsqcf_8_s;
extern struct liquid_autotest_s sumsqcf_15_s;
extern struct liquid_autotest_s sumsqcf_16_s;
// ./src/dotprod/tests/sumsqf_autotest.c
extern struct liquid_autotest_s sumsqf_3_s;
extern struct liquid_autotest_s sumsqf_4_s;
extern struct liquid_autotest_s sumsqf_7_s;
extern struct liquid_autotest_s sumsqf_8_s;
extern struct liquid_autotest_s sumsqf_15_s;
extern struct liquid_autotest_s sumsqf_16_s;
// ./src/dotprod/tests/dotprod_cccf_autotest.c
extern struct liquid_autotest_s dotprod_cccf_rand16_s;
extern struct liquid_autotest_s dotprod_cccf_struct_lengths_s;
extern struct liquid_autotest_s dotprod_cccf_struct_vs_ordinal_s;
// ./src/dotprod/tests/dotprod_rrrf_autotest.c
extern struct liquid_autotest_s dotprod_rrrf_basic_s;
extern struct liquid_autotest_s dotprod_rrrf_uneven_s;
extern struct liquid_autotest_s dotprod_rrrf_struct_s;
extern struct liquid_autotest_s dotprod_rrrf_struct_align_s;
extern struct liquid_autotest_s dotprod_rrrf_rand01_s;
extern struct liquid_autotest_s dotprod_rrrf_rand02_s;
extern struct liquid_autotest_s dotprod_rrrf_struct_lengths_s;
extern struct liquid_autotest_s dotprod_rrrf_struct_vs_ordinal_s;
// ./src/math/tests/prime_autotest.c
extern struct liquid_autotest_s prime_small_s;
extern struct liquid_autotest_s factors_s;
extern struct liquid_autotest_s totient_s;
// ./src/math/tests/math_gamma_autotest.c
extern struct liquid_autotest_s gamma_s;
extern struct liquid_autotest_s lngamma_s;
extern struct liquid_autotest_s uppergamma_s;
extern struct liquid_autotest_s factorial_s;
extern struct liquid_autotest_s nchoosek_s;
// ./src/math/tests/math_autotest.c
extern struct liquid_autotest_s Q_s;
extern struct liquid_autotest_s MarcumQf_s;
extern struct liquid_autotest_s MarcumQ1f_s;
extern struct liquid_autotest_s sincf_s;
extern struct liquid_autotest_s nextpow2_s;
extern struct liquid_autotest_s math_config_s;
// ./src/math/tests/polynomial_findroots_autotest.c
extern struct liquid_autotest_s polyf_findroots_real_s;
extern struct liquid_autotest_s polyf_findroots_complex_s;
extern struct liquid_autotest_s polyf_findroots_mix_s;
extern struct liquid_autotest_s polyf_findroots_mix2_s;
extern struct liquid_autotest_s polyf_findroots_rand_s;
// ./src/math/tests/math_complex_autotest.c
extern struct liquid_autotest_s cexpf_s;
extern struct liquid_autotest_s clogf_s;
extern struct liquid_autotest_s csqrtf_s;
extern struct liquid_autotest_s casinf_s;
extern struct liquid_autotest_s cacosf_s;
extern struct liquid_autotest_s catanf_s;
extern struct liquid_autotest_s cargf_s;
// ./src/math/tests/gcd_autotest.c
extern struct liquid_autotest_s gcd_one_s;
extern struct liquid_autotest_s gcd_edge_cases_s;
extern struct liquid_autotest_s gcd_base_s;
// ./src/math/tests/polynomial_autotest.c
extern struct liquid_autotest_s polyf_fit_q3n3_s;
extern struct liquid_autotest_s polyf_lagrange_issue165_s;
extern struct liquid_autotest_s polyf_lagrange_s;
extern struct liquid_autotest_s polyf_expandbinomial_4_s;
extern struct liquid_autotest_s polyf_expandroots_4_s;
extern struct liquid_autotest_s polyf_expandroots_11_s;
extern struct liquid_autotest_s polycf_expandroots_4_s;
extern struct liquid_autotest_s polyf_expandroots2_3_s;
extern struct liquid_autotest_s polyf_mul_2_3_s;
extern struct liquid_autotest_s poly_expandbinomial_n6_s;
extern struct liquid_autotest_s poly_binomial_expand_pm_m6_k1_s;
extern struct liquid_autotest_s poly_expandbinomial_pm_m5_k2_s;
// ./src/math/tests/math_bessel_autotest.c
extern struct liquid_autotest_s lnbesselif_s;
extern struct liquid_autotest_s besselif_s;
extern struct liquid_autotest_s besseli0f_s;
extern struct liquid_autotest_s besseljf_s;
extern struct liquid_autotest_s besselj0f_s;
// ./src/math/tests/math_window_autotest.c
extern struct liquid_autotest_s window_hamming_s;
extern struct liquid_autotest_s window_hann_s;
extern struct liquid_autotest_s window_blackmanharris_s;
extern struct liquid_autotest_s window_blackmanharris7_s;
extern struct liquid_autotest_s window_kaiser_s;
extern struct liquid_autotest_s window_flattop_s;
extern struct liquid_autotest_s window_triangular_s;
extern struct liquid_autotest_s window_rcostaper_s;
extern struct liquid_autotest_s window_kbd_s;
extern struct liquid_autotest_s kbd_n16_s;
extern struct liquid_autotest_s kbd_n32_s;
extern struct liquid_autotest_s kbd_n48_s;
extern struct liquid_autotest_s window_config_s;
// ./src/quantization/tests/compand_autotest.c
extern struct liquid_autotest_s compand_float_s;
extern struct liquid_autotest_s compand_cfloat_s;
// ./src/quantization/tests/quantize_autotest.c
extern struct liquid_autotest_s quantize_float_n8_s;
// ./src/agc/tests/agc_crcf_autotest.c
extern struct liquid_autotest_s agc_crcf_dc_gain_control_s;
extern struct liquid_autotest_s agc_crcf_scale_s;
extern struct liquid_autotest_s agc_crcf_ac_gain_control_s;
extern struct liquid_autotest_s agc_crcf_rssi_sinusoid_s;
extern struct liquid_autotest_s agc_crcf_rssi_noise_s;
extern struct liquid_autotest_s agc_crcf_squelch_s;
extern struct liquid_autotest_s agc_crcf_lock_s;
extern struct liquid_autotest_s agc_crcf_config_s;
extern struct liquid_autotest_s agc_crcf_copy_s;
// ./src/audio/tests/cvsd_autotest.c
extern struct liquid_autotest_s cvsd_rmse_sine_s;
extern struct liquid_autotest_s cvsd_rmse_sine8_s;
extern struct liquid_autotest_s cvsd_config_s;
// ./src/nco/tests/nco_crcf_phase_autotest.c
extern struct liquid_autotest_s nco_crcf_phase_s;
extern struct liquid_autotest_s nco_basic_s;
extern struct liquid_autotest_s nco_mixing_s;
extern struct liquid_autotest_s nco_block_mixing_s;
// ./src/nco/tests/nco_crcf_pll_autotest.c
extern struct liquid_autotest_s nco_crcf_pll_phase_s;
extern struct liquid_autotest_s nco_crcf_pll_freq_s;
// ./src/nco/tests/unwrap_phase_autotest.c
extern struct liquid_autotest_s nco_unwrap_phase_s;
// ./src/nco/tests/nco_crcf_frequency_autotest.c
extern struct liquid_autotest_s nco_crcf_frequency_s;
// ./src/nco/tests/nco_crcf_autotest.c
extern struct liquid_autotest_s nco_crcf_constrain_s;
extern struct liquid_autotest_s nco_crcf_copy_s;
extern struct liquid_autotest_s nco_config_s;
// ./src/nco/tests/nco_crcf_spectrum_autotest.c
extern struct liquid_autotest_s nco_crcf_spectrum_nco_f00_s;
extern struct liquid_autotest_s nco_crcf_spectrum_nco_f01_s;
extern struct liquid_autotest_s nco_crcf_spectrum_nco_f02_s;
extern struct liquid_autotest_s nco_crcf_spectrum_nco_f03_s;
extern struct liquid_autotest_s nco_crcf_spectrum_nco_f04_s;
extern struct liquid_autotest_s nco_crcf_spectrum_vco_f00_s;
extern struct liquid_autotest_s nco_crcf_spectrum_vco_f01_s;
extern struct liquid_autotest_s nco_crcf_spectrum_vco_f02_s;
extern struct liquid_autotest_s nco_crcf_spectrum_vco_f03_s;
extern struct liquid_autotest_s nco_crcf_spectrum_vco_f04_s;
// ./src/nco/tests/nco_crcf_mix_autotest.c
extern struct liquid_autotest_s nco_crcf_mix_nco_0_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_1_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_2_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_3_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_4_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_5_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_6_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_7_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_8_s;
extern struct liquid_autotest_s nco_crcf_mix_nco_9_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_0_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_1_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_2_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_3_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_4_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_5_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_6_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_7_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_8_s;
extern struct liquid_autotest_s nco_crcf_mix_vco_9_s;
// ./src/modem/tests/cpfskmodem_autotest.c
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5000_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0250_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p1250_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0625_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5000_k4_m3_rcosfull_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0250_k4_m3_rcosfull_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p1250_k4_m3_rcosfull_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0625_k4_m3_rcosfull_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5000_k4_m3_rcospart_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0250_k4_m3_rcospart_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p1250_k4_m3_rcospart_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0625_k4_m3_rcospart_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5000_k4_m3_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0250_k4_m3_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p1250_k4_m3_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p0625_k4_m3_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps2_h0p0250_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps3_h0p1250_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps4_h0p0625_k4_m3_square_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5_k2_m7_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5_k4_m7_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5_k6_m7_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_bps1_h0p5_k8_m7_gmsk_s;
extern struct liquid_autotest_s cpfskmodem_spectrum_s;
extern struct liquid_autotest_s cpfskmodem_config_s;
// ./src/modem/tests/freqmodem_autotest.c
extern struct liquid_autotest_s freqmodem_kf_0_02_s;
extern struct liquid_autotest_s freqmodem_kf_0_04_s;
extern struct liquid_autotest_s freqmodem_kf_0_08_s;
// ./src/modem/tests/gmskmodem_autotest.c
extern struct liquid_autotest_s gmskmodem_k4_m3_b025_s;
extern struct liquid_autotest_s gmskmodem_k2_m3_b025_s;
extern struct liquid_autotest_s gmskmodem_k3_m3_b025_s;
extern struct liquid_autotest_s gmskmodem_k5_m3_b025_s;
extern struct liquid_autotest_s gmskmodem_k8_m3_b033_s;
extern struct liquid_autotest_s gmskmodem_k4_m1_b025_s;
extern struct liquid_autotest_s gmskmodem_k4_m2_b025_s;
extern struct liquid_autotest_s gmskmodem_k4_m8_b025_s;
extern struct liquid_autotest_s gmskmodem_k4_m3_b020_s;
extern struct liquid_autotest_s gmskmodem_k4_m3_b033_s;
extern struct liquid_autotest_s gmskmodem_k4_m3_b050_s;
extern struct liquid_autotest_s gmskmod_copy_s;
extern struct liquid_autotest_s gmskdem_copy_s;
// ./src/modem/tests/modem_config_autotest.c
extern struct liquid_autotest_s modem_copy_psk2_s;
extern struct liquid_autotest_s modem_copy_psk4_s;
extern struct liquid_autotest_s modem_copy_psk8_s;
extern struct liquid_autotest_s modem_copy_psk16_s;
extern struct liquid_autotest_s modem_copy_psk32_s;
extern struct liquid_autotest_s modem_copy_psk64_s;
extern struct liquid_autotest_s modem_copy_psk128_s;
extern struct liquid_autotest_s modem_copy_psk256_s;
extern struct liquid_autotest_s modem_copy_dpsk2_s;
extern struct liquid_autotest_s modem_copy_dpsk4_s;
extern struct liquid_autotest_s modem_copy_dpsk8_s;
extern struct liquid_autotest_s modem_copy_dpsk16_s;
extern struct liquid_autotest_s modem_copy_dpsk32_s;
extern struct liquid_autotest_s modem_copy_dpsk64_s;
extern struct liquid_autotest_s modem_copy_dpsk128_s;
extern struct liquid_autotest_s modem_copy_dpsk256_s;
extern struct liquid_autotest_s modem_copy_ask2_s;
extern struct liquid_autotest_s modem_copy_ask4_s;
extern struct liquid_autotest_s modem_copy_ask8_s;
extern struct liquid_autotest_s modem_copy_ask16_s;
extern struct liquid_autotest_s modem_copy_ask32_s;
extern struct liquid_autotest_s modem_copy_ask64_s;
extern struct liquid_autotest_s modem_copy_ask128_s;
extern struct liquid_autotest_s modem_copy_ask256_s;
extern struct liquid_autotest_s modem_copy_qam4_s;
extern struct liquid_autotest_s modem_copy_qam8_s;
extern struct liquid_autotest_s modem_copy_qam16_s;
extern struct liquid_autotest_s modem_copy_qam32_s;
extern struct liquid_autotest_s modem_copy_qam64_s;
extern struct liquid_autotest_s modem_copy_qam128_s;
extern struct liquid_autotest_s modem_copy_qam256_s;
extern struct liquid_autotest_s modem_copy_apsk4_s;
extern struct liquid_autotest_s modem_copy_apsk8_s;
extern struct liquid_autotest_s modem_copy_apsk16_s;
extern struct liquid_autotest_s modem_copy_apsk32_s;
extern struct liquid_autotest_s modem_copy_apsk64_s;
extern struct liquid_autotest_s modem_copy_apsk128_s;
extern struct liquid_autotest_s modem_copy_apsk256_s;
extern struct liquid_autotest_s modem_copy_bpsk_s;
extern struct liquid_autotest_s modem_copy_qpsk_s;
extern struct liquid_autotest_s modem_copy_ook_s;
extern struct liquid_autotest_s modem_copy_sqam32_s;
extern struct liquid_autotest_s modem_copy_sqam128_s;
extern struct liquid_autotest_s modem_copy_V29_s;
extern struct liquid_autotest_s modem_copy_arb16opt_s;
extern struct liquid_autotest_s modem_copy_arb32opt_s;
extern struct liquid_autotest_s modem_copy_arb64opt_s;
extern struct liquid_autotest_s modem_copy_arb128opt_s;
extern struct liquid_autotest_s modem_copy_arb256opt_s;
extern struct liquid_autotest_s modem_copy_arb64vt_s;
extern struct liquid_autotest_s modem_copy_pi4dqpsk_s;
extern struct liquid_autotest_s modem_config_s;
// ./src/modem/tests/ampmodem_autotest.c
extern struct liquid_autotest_s ampmodem_dsb_carrier_on_s;
extern struct liquid_autotest_s ampmodem_usb_carrier_on_s;
extern struct liquid_autotest_s ampmodem_lsb_carrier_on_s;
extern struct liquid_autotest_s ampmodem_dsb_carrier_off_s;
extern struct liquid_autotest_s ampmodem_usb_carrier_off_s;
extern struct liquid_autotest_s ampmodem_lsb_carrier_off_s;
// ./src/modem/tests/modem_autotest.c
extern struct liquid_autotest_s mod_demod_psk2_s;
extern struct liquid_autotest_s mod_demod_psk4_s;
extern struct liquid_autotest_s mod_demod_psk8_s;
extern struct liquid_autotest_s mod_demod_psk16_s;
extern struct liquid_autotest_s mod_demod_psk32_s;
extern struct liquid_autotest_s mod_demod_psk64_s;
extern struct liquid_autotest_s mod_demod_psk128_s;
extern struct liquid_autotest_s mod_demod_psk256_s;
extern struct liquid_autotest_s mod_demod_dpsk2_s;
extern struct liquid_autotest_s mod_demod_dpsk4_s;
extern struct liquid_autotest_s mod_demod_dpsk8_s;
extern struct liquid_autotest_s mod_demod_dpsk16_s;
extern struct liquid_autotest_s mod_demod_dpsk32_s;
extern struct liquid_autotest_s mod_demod_dpsk64_s;
extern struct liquid_autotest_s mod_demod_dpsk128_s;
extern struct liquid_autotest_s mod_demod_dpsk256_s;
extern struct liquid_autotest_s mod_demod_ask2_s;
extern struct liquid_autotest_s mod_demod_ask4_s;
extern struct liquid_autotest_s mod_demod_ask8_s;
extern struct liquid_autotest_s mod_demod_ask16_s;
extern struct liquid_autotest_s mod_demod_ask32_s;
extern struct liquid_autotest_s mod_demod_ask64_s;
extern struct liquid_autotest_s mod_demod_ask128_s;
extern struct liquid_autotest_s mod_demod_ask256_s;
extern struct liquid_autotest_s mod_demod_qam4_s;
extern struct liquid_autotest_s mod_demod_qam8_s;
extern struct liquid_autotest_s mod_demod_qam16_s;
extern struct liquid_autotest_s mod_demod_qam32_s;
extern struct liquid_autotest_s mod_demod_qam64_s;
extern struct liquid_autotest_s mod_demod_qam128_s;
extern struct liquid_autotest_s mod_demod_qam256_s;
extern struct liquid_autotest_s mod_demod_apsk4_s;
extern struct liquid_autotest_s mod_demod_apsk8_s;
extern struct liquid_autotest_s mod_demod_apsk16_s;
extern struct liquid_autotest_s mod_demod_apsk32_s;
extern struct liquid_autotest_s mod_demod_apsk64_s;
extern struct liquid_autotest_s mod_demod_apsk128_s;
extern struct liquid_autotest_s mod_demod_apsk256_s;
extern struct liquid_autotest_s mod_demod_bpsk_s;
extern struct liquid_autotest_s mod_demod_qpsk_s;
extern struct liquid_autotest_s mod_demod_ook_s;
extern struct liquid_autotest_s mod_demod_sqam32_s;
extern struct liquid_autotest_s mod_demod_sqam128_s;
extern struct liquid_autotest_s mod_demod_V29_s;
extern struct liquid_autotest_s mod_demod_arb16opt_s;
extern struct liquid_autotest_s mod_demod_arb32opt_s;
extern struct liquid_autotest_s mod_demod_arb64opt_s;
extern struct liquid_autotest_s mod_demod_arb128opt_s;
extern struct liquid_autotest_s mod_demod_arb256opt_s;
extern struct liquid_autotest_s mod_demod_arb64vt_s;
extern struct liquid_autotest_s mod_demod_pi4dqpsk_s;
// ./src/modem/tests/modem_demodsoft_autotest.c
extern struct liquid_autotest_s demodsoft_psk2_s;
extern struct liquid_autotest_s demodsoft_psk4_s;
extern struct liquid_autotest_s demodsoft_psk8_s;
extern struct liquid_autotest_s demodsoft_psk16_s;
extern struct liquid_autotest_s demodsoft_psk32_s;
extern struct liquid_autotest_s demodsoft_psk64_s;
extern struct liquid_autotest_s demodsoft_psk128_s;
extern struct liquid_autotest_s demodsoft_psk256_s;
extern struct liquid_autotest_s demodsoft_dpsk2_s;
extern struct liquid_autotest_s demodsoft_dpsk4_s;
extern struct liquid_autotest_s demodsoft_dpsk8_s;
extern struct liquid_autotest_s demodsoft_dpsk16_s;
extern struct liquid_autotest_s demodsoft_dpsk32_s;
extern struct liquid_autotest_s demodsoft_dpsk64_s;
extern struct liquid_autotest_s demodsoft_dpsk128_s;
extern struct liquid_autotest_s demodsoft_dpsk256_s;
extern struct liquid_autotest_s demodsoft_ask2_s;
extern struct liquid_autotest_s demodsoft_ask4_s;
extern struct liquid_autotest_s demodsoft_ask8_s;
extern struct liquid_autotest_s demodsoft_ask16_s;
extern struct liquid_autotest_s demodsoft_ask32_s;
extern struct liquid_autotest_s demodsoft_ask64_s;
extern struct liquid_autotest_s demodsoft_ask128_s;
extern struct liquid_autotest_s demodsoft_ask256_s;
extern struct liquid_autotest_s demodsoft_qam4_s;
extern struct liquid_autotest_s demodsoft_qam8_s;
extern struct liquid_autotest_s demodsoft_qam16_s;
extern struct liquid_autotest_s demodsoft_qam32_s;
extern struct liquid_autotest_s demodsoft_qam64_s;
extern struct liquid_autotest_s demodsoft_qam128_s;
extern struct liquid_autotest_s demodsoft_qam256_s;
extern struct liquid_autotest_s demodsoft_apsk4_s;
extern struct liquid_autotest_s demodsoft_apsk8_s;
extern struct liquid_autotest_s demodsoft_apsk16_s;
extern struct liquid_autotest_s demodsoft_apsk32_s;
extern struct liquid_autotest_s demodsoft_apsk64_s;
extern struct liquid_autotest_s demodsoft_apsk128_s;
extern struct liquid_autotest_s demodsoft_apsk256_s;
extern struct liquid_autotest_s demodsoft_bpsk_s;
extern struct liquid_autotest_s demodsoft_qpsk_s;
extern struct liquid_autotest_s demodsoft_ook_s;
extern struct liquid_autotest_s demodsoft_sqam32_s;
extern struct liquid_autotest_s demodsoft_sqam128_s;
extern struct liquid_autotest_s demodsoft_V29_s;
extern struct liquid_autotest_s demodsoft_arb16opt_s;
extern struct liquid_autotest_s demodsoft_arb32opt_s;
extern struct liquid_autotest_s demodsoft_arb64opt_s;
extern struct liquid_autotest_s demodsoft_arb128opt_s;
extern struct liquid_autotest_s demodsoft_arb256opt_s;
extern struct liquid_autotest_s demodsoft_arb64vt_s;
extern struct liquid_autotest_s demodsoft_pi4dqpsk_s;
// ./src/modem/tests/modem_demodstats_autotest.c
extern struct liquid_autotest_s demodstats_psk2_s;
extern struct liquid_autotest_s demodstats_psk4_s;
extern struct liquid_autotest_s demodstats_psk8_s;
extern struct liquid_autotest_s demodstats_psk16_s;
extern struct liquid_autotest_s demodstats_psk32_s;
extern struct liquid_autotest_s demodstats_psk64_s;
extern struct liquid_autotest_s demodstats_psk128_s;
extern struct liquid_autotest_s demodstats_psk256_s;
extern struct liquid_autotest_s demodstats_dpsk2_s;
extern struct liquid_autotest_s demodstats_dpsk4_s;
extern struct liquid_autotest_s demodstats_dpsk8_s;
extern struct liquid_autotest_s demodstats_dpsk16_s;
extern struct liquid_autotest_s demodstats_dpsk32_s;
extern struct liquid_autotest_s demodstats_dpsk64_s;
extern struct liquid_autotest_s demodstats_dpsk128_s;
extern struct liquid_autotest_s demodstats_dpsk256_s;
extern struct liquid_autotest_s demodstats_ask2_s;
extern struct liquid_autotest_s demodstats_ask4_s;
extern struct liquid_autotest_s demodstats_ask8_s;
extern struct liquid_autotest_s demodstats_ask16_s;
extern struct liquid_autotest_s demodstats_ask32_s;
extern struct liquid_autotest_s demodstats_ask64_s;
extern struct liquid_autotest_s demodstats_ask128_s;
extern struct liquid_autotest_s demodstats_ask256_s;
extern struct liquid_autotest_s demodstats_qam4_s;
extern struct liquid_autotest_s demodstats_qam8_s;
extern struct liquid_autotest_s demodstats_qam16_s;
extern struct liquid_autotest_s demodstats_qam32_s;
extern struct liquid_autotest_s demodstats_qam64_s;
extern struct liquid_autotest_s demodstats_qam128_s;
extern struct liquid_autotest_s demodstats_qam256_s;
extern struct liquid_autotest_s demodstats_apsk4_s;
extern struct liquid_autotest_s demodstats_apsk8_s;
extern struct liquid_autotest_s demodstats_apsk16_s;
extern struct liquid_autotest_s demodstats_apsk32_s;
extern struct liquid_autotest_s demodstats_apsk64_s;
extern struct liquid_autotest_s demodstats_apsk128_s;
extern struct liquid_autotest_s demodstats_apsk256_s;
extern struct liquid_autotest_s demodstats_bpsk_s;
extern struct liquid_autotest_s demodstats_qpsk_s;
extern struct liquid_autotest_s demodstats_ook_s;
extern struct liquid_autotest_s demodstats_sqam32_s;
extern struct liquid_autotest_s demodstats_sqam128_s;
extern struct liquid_autotest_s demodstats_V29_s;
extern struct liquid_autotest_s demodstats_arb16opt_s;
extern struct liquid_autotest_s demodstats_arb32opt_s;
extern struct liquid_autotest_s demodstats_arb64opt_s;
extern struct liquid_autotest_s demodstats_arb128opt_s;
extern struct liquid_autotest_s demodstats_arb256opt_s;
extern struct liquid_autotest_s demodstats_arb64vt_s;
// ./src/modem/tests/modem_utilities_autotest.c
extern struct liquid_autotest_s modemcf_print_schemes_s;
extern struct liquid_autotest_s modemcf_str2mod_s;
extern struct liquid_autotest_s modemcf_types_s;
// ./src/modem/tests/fskmodem_autotest.c
extern struct liquid_autotest_s fskmodem_norm_M2_s;
extern struct liquid_autotest_s fskmodem_norm_M4_s;
extern struct liquid_autotest_s fskmodem_norm_M8_s;
extern struct liquid_autotest_s fskmodem_norm_M16_s;
extern struct liquid_autotest_s fskmodem_norm_M32_s;
extern struct liquid_autotest_s fskmodem_norm_M64_s;
extern struct liquid_autotest_s fskmodem_norm_M128_s;
extern struct liquid_autotest_s fskmodem_norm_M256_s;
extern struct liquid_autotest_s fskmodem_norm_M512_s;
extern struct liquid_autotest_s fskmodem_norm_M1024_s;
extern struct liquid_autotest_s fskmodem_misc_M2_s;
extern struct liquid_autotest_s fskmodem_misc_M4_s;
extern struct liquid_autotest_s fskmodem_misc_M8_s;
extern struct liquid_autotest_s fskmodem_misc_M16_s;
extern struct liquid_autotest_s fskmodem_misc_M32_s;
extern struct liquid_autotest_s fskmodem_misc_M64_s;
extern struct liquid_autotest_s fskmodem_misc_M128_s;
extern struct liquid_autotest_s fskmodem_misc_M256_s;
extern struct liquid_autotest_s fskmodem_misc_M512_s;
extern struct liquid_autotest_s fskmodem_misc_M1024_s;
extern struct liquid_autotest_s fskmod_copy_s;
extern struct liquid_autotest_s fskdem_copy_s;
// ./src/fft/tests/spwaterfall_autotest.c
extern struct liquid_autotest_s spwaterfall_config_s;
extern struct liquid_autotest_s spwaterfallcf_noise_440_s;
extern struct liquid_autotest_s spwaterfallcf_noise_1024_s;
extern struct liquid_autotest_s spwaterfallcf_noise_1200_s;
extern struct liquid_autotest_s spwaterfall_operation_s;
extern struct liquid_autotest_s spwaterfall_copy_s;
extern struct liquid_autotest_s spwaterfall_gnuplot_s;
// ./src/fft/tests/fft_transforms_autotest.c
extern struct liquid_autotest_s fft_3_s;
extern struct liquid_autotest_s fft_5_s;
extern struct liquid_autotest_s fft_6_s;
extern struct liquid_autotest_s fft_7_s;
extern struct liquid_autotest_s fft_9_s;
extern struct liquid_autotest_s fft_2_s;
extern struct liquid_autotest_s fft_4_s;
extern struct liquid_autotest_s fft_8_s;
extern struct liquid_autotest_s fft_16_s;
extern struct liquid_autotest_s fft_32_s;
extern struct liquid_autotest_s fft_64_s;
extern struct liquid_autotest_s fft_10_s;
extern struct liquid_autotest_s fft_21_s;
extern struct liquid_autotest_s fft_22_s;
extern struct liquid_autotest_s fft_24_s;
extern struct liquid_autotest_s fft_26_s;
extern struct liquid_autotest_s fft_30_s;
extern struct liquid_autotest_s fft_35_s;
extern struct liquid_autotest_s fft_36_s;
extern struct liquid_autotest_s fft_48_s;
extern struct liquid_autotest_s fft_63_s;
extern struct liquid_autotest_s fft_92_s;
extern struct liquid_autotest_s fft_96_s;
extern struct liquid_autotest_s fft_120_s;
extern struct liquid_autotest_s fft_130_s;
extern struct liquid_autotest_s fft_192_s;
extern struct liquid_autotest_s fft_17_s;
extern struct liquid_autotest_s fft_43_s;
extern struct liquid_autotest_s fft_79_s;
extern struct liquid_autotest_s fft_157_s;
extern struct liquid_autotest_s fft_317_s;
extern struct liquid_autotest_s fft_509_s;
// ./src/fft/tests/fft_r2r_autotest.c
extern struct liquid_autotest_s fft_r2r_REDFT00_n8_s;
extern struct liquid_autotest_s fft_r2r_REDFT10_n8_s;
extern struct liquid_autotest_s fft_r2r_REDFT01_n8_s;
extern struct liquid_autotest_s fft_r2r_REDFT11_n8_s;
extern struct liquid_autotest_s fft_r2r_RODFT00_n8_s;
extern struct liquid_autotest_s fft_r2r_RODFT10_n8_s;
extern struct liquid_autotest_s fft_r2r_RODFT01_n8_s;
extern struct liquid_autotest_s fft_r2r_RODFT11_n8_s;
extern struct liquid_autotest_s fft_r2r_REDFT00_n27_s;
extern struct liquid_autotest_s fft_r2r_REDFT10_n27_s;
extern struct liquid_autotest_s fft_r2r_REDFT01_n27_s;
extern struct liquid_autotest_s fft_r2r_REDFT11_n27_s;
extern struct liquid_autotest_s fft_r2r_RODFT00_n27_s;
extern struct liquid_autotest_s fft_r2r_RODFT10_n27_s;
extern struct liquid_autotest_s fft_r2r_RODFT01_n27_s;
extern struct liquid_autotest_s fft_r2r_RODFT11_n27_s;
extern struct liquid_autotest_s fft_r2r_REDFT00_n32_s;
extern struct liquid_autotest_s fft_r2r_REDFT10_n32_s;
extern struct liquid_autotest_s fft_r2r_REDFT01_n32_s;
extern struct liquid_autotest_s fft_r2r_REDFT11_n32_s;
extern struct liquid_autotest_s fft_r2r_RODFT00_n32_s;
extern struct liquid_autotest_s fft_r2r_RODFT10_n32_s;
extern struct liquid_autotest_s fft_r2r_RODFT01_n32_s;
extern struct liquid_autotest_s fft_r2r_RODFT11_n32_s;
// ./src/fft/tests/fft_autotest.c
extern struct liquid_autotest_s fft_shift_4_s;
extern struct liquid_autotest_s fft_shift_8_s;
// ./src/fft/tests/asgram_autotest.c
extern struct liquid_autotest_s asgramcf_copy_s;
extern struct liquid_autotest_s asgramcf_config_s;
// ./src/fft/tests/spgram_autotest.c
extern struct liquid_autotest_s spgramcf_noise_440_s;
extern struct liquid_autotest_s spgramcf_noise_1024_s;
extern struct liquid_autotest_s spgramcf_noise_1200_s;
extern struct liquid_autotest_s spgramcf_noise_custom_0_s;
extern struct liquid_autotest_s spgramcf_noise_custom_1_s;
extern struct liquid_autotest_s spgramcf_noise_custom_2_s;
extern struct liquid_autotest_s spgramcf_noise_custom_3_s;
extern struct liquid_autotest_s spgramcf_noise_hamming_s;
extern struct liquid_autotest_s spgramcf_noise_hann_s;
extern struct liquid_autotest_s spgramcf_noise_blackmanharris_s;
extern struct liquid_autotest_s spgramcf_noise_blackmanharris7_s;
extern struct liquid_autotest_s spgramcf_noise_kaiser_s;
extern struct liquid_autotest_s spgramcf_noise_flattop_s;
extern struct liquid_autotest_s spgramcf_noise_triangular_s;
extern struct liquid_autotest_s spgramcf_noise_rcostaper_s;
extern struct liquid_autotest_s spgramcf_noise_kbd_s;
extern struct liquid_autotest_s spgramcf_signal_00_s;
extern struct liquid_autotest_s spgramcf_signal_01_s;
extern struct liquid_autotest_s spgramcf_signal_02_s;
extern struct liquid_autotest_s spgramcf_signal_03_s;
extern struct liquid_autotest_s spgramcf_signal_04_s;
extern struct liquid_autotest_s spgramcf_signal_05_s;
extern struct liquid_autotest_s spgramcf_counters_s;
extern struct liquid_autotest_s spgramcf_config_s;
extern struct liquid_autotest_s spgramcf_standalone_s;
extern struct liquid_autotest_s spgramcf_short_s;
extern struct liquid_autotest_s spgramcf_copy_s;
extern struct liquid_autotest_s spgramcf_null_s;
extern struct liquid_autotest_s spgram_gnuplot_s;
// ./src/filter/tests/iirfilt_autotest.c
extern struct liquid_autotest_s iirfilt_integrator_s;
extern struct liquid_autotest_s iirfilt_differentiator_s;
extern struct liquid_autotest_s iirfilt_dcblock_s;
extern struct liquid_autotest_s iirfilt_copy_tf_s;
extern struct liquid_autotest_s iirfilt_copy_sos_s;
extern struct liquid_autotest_s iirfilt_config_s;
// ./src/filter/tests/dds_cccf_autotest.c
extern struct liquid_autotest_s dds_cccf_0_s;
extern struct liquid_autotest_s dds_cccf_1_s;
extern struct liquid_autotest_s dds_cccf_2_s;
extern struct liquid_autotest_s dds_config_s;
extern struct liquid_autotest_s dds_copy_s;
// ./src/filter/tests/firpfb_autotest.c
extern struct liquid_autotest_s firpfb_impulse_response_s;
extern struct liquid_autotest_s firpfb_crcf_copy_s;
// ./src/filter/tests/resamp2_crcf_autotest.c
extern struct liquid_autotest_s resamp2_analysis_s;
extern struct liquid_autotest_s resamp2_synthesis_s;
extern struct liquid_autotest_s resamp2_crcf_filter_0_s;
extern struct liquid_autotest_s resamp2_crcf_filter_1_s;
extern struct liquid_autotest_s resamp2_crcf_filter_2_s;
extern struct liquid_autotest_s resamp2_crcf_filter_3_s;
extern struct liquid_autotest_s resamp2_crcf_filter_4_s;
extern struct liquid_autotest_s resamp2_crcf_filter_5_s;
extern struct liquid_autotest_s resamp2_config_s;
extern struct liquid_autotest_s resamp2_copy_s;
// ./src/filter/tests/firfilt_autotest.c
extern struct liquid_autotest_s firfilt_crcf_kaiser_s;
extern struct liquid_autotest_s firfilt_crcf_firdespm_s;
extern struct liquid_autotest_s firfilt_crcf_rect_s;
extern struct liquid_autotest_s firfilt_crcf_notch_s;
extern struct liquid_autotest_s firfilt_cccf_notch_s;
extern struct liquid_autotest_s firfilt_config_s;
extern struct liquid_autotest_s firfilt_recreate_s;
extern struct liquid_autotest_s firfilt_push_write_s;
// ./src/filter/tests/symsync_copy_autotest.c
extern struct liquid_autotest_s symsync_copy_s;
extern struct liquid_autotest_s symsync_config_s;
// ./src/filter/tests/rresamp_crcf_autotest.c
extern struct liquid_autotest_s rresamp_crcf_baseline_P1_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P2_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P6_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P8_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_baseline_P9_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P1_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P2_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P6_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P8_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_default_P9_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_arkaiser_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_arkaiser_P5_Q3_s;
extern struct liquid_autotest_s rresamp_crcf_rrcos_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_rrcos_P5_Q3_s;
extern struct liquid_autotest_s rresamp_copy_s;
extern struct liquid_autotest_s rresamp_config_s;
// ./src/filter/tests/msresamp_crcf_autotest.c
extern struct liquid_autotest_s msresamp_crcf_01_s;
extern struct liquid_autotest_s msresamp_crcf_02_s;
extern struct liquid_autotest_s msresamp_crcf_03_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_0_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_1_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_2_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_3_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_4_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_5_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_6_s;
extern struct liquid_autotest_s msresamp_crcf_num_output_7_s;
extern struct liquid_autotest_s msresamp_crcf_copy_s;
// ./src/filter/tests/firfilt_coefficients_autotest.c
extern struct liquid_autotest_s firfilt_cccf_coefficients_test_s;
// ./src/filter/tests/msresamp2_crcf_autotest.c
extern struct liquid_autotest_s msresamp2_crcf_interp_01_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_02_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_03_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_04_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_05_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_06_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_07_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_08_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_09_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_10_s;
extern struct liquid_autotest_s msresamp2_crcf_interp_11_s;
extern struct liquid_autotest_s msresamp2_copy_s;
// ./src/filter/tests/firdecim_autotest.c
extern struct liquid_autotest_s firdecim_config_s;
extern struct liquid_autotest_s firdecim_block_s;
extern struct liquid_autotest_s firdecim_copy_s;
// ./src/filter/tests/firfilt_copy_autotest.c
extern struct liquid_autotest_s firfilt_crcf_copy_s;
// ./src/filter/tests/firinterp_autotest.c
extern struct liquid_autotest_s firinterp_rrrf_common_s;
extern struct liquid_autotest_s firinterp_crcf_common_s;
extern struct liquid_autotest_s firinterp_rrrf_generic_s;
extern struct liquid_autotest_s firinterp_crcf_generic_s;
extern struct liquid_autotest_s firinterp_crcf_rnyquist_0_s;
extern struct liquid_autotest_s firinterp_crcf_rnyquist_1_s;
extern struct liquid_autotest_s firinterp_crcf_rnyquist_2_s;
extern struct liquid_autotest_s firinterp_crcf_rnyquist_3_s;
extern struct liquid_autotest_s firinterp_copy_s;
extern struct liquid_autotest_s firinterp_flush_s;
// ./src/filter/tests/iirfilt_xxxf_autotest.c
extern struct liquid_autotest_s iirfilt_rrrf_h3x64_s;
extern struct liquid_autotest_s iirfilt_rrrf_h5x64_s;
extern struct liquid_autotest_s iirfilt_rrrf_h7x64_s;
extern struct liquid_autotest_s iirfilt_crcf_h3x64_s;
extern struct liquid_autotest_s iirfilt_crcf_h5x64_s;
extern struct liquid_autotest_s iirfilt_crcf_h7x64_s;
extern struct liquid_autotest_s iirfilt_cccf_h3x64_s;
extern struct liquid_autotest_s iirfilt_cccf_h5x64_s;
extern struct liquid_autotest_s iirfilt_cccf_h7x64_s;
// ./src/filter/tests/iirdes_support_autotest.c
extern struct liquid_autotest_s iirdes_cplxpair_n6_s;
extern struct liquid_autotest_s iirdes_cplxpair_n20_s;
extern struct liquid_autotest_s iirdes_dzpk2sosf_s;
extern struct liquid_autotest_s iirdes_isstable_n2_yes_s;
extern struct liquid_autotest_s iirdes_isstable_n2_no_s;
// ./src/filter/tests/ordfilt_autotest.c
extern struct liquid_autotest_s ordfilt_copy_s;
// ./src/filter/tests/iirhilb_autotest.c
extern struct liquid_autotest_s iirhilbf_interp_decim_s;
extern struct liquid_autotest_s iirhilbf_filter_s;
extern struct liquid_autotest_s iirhilbf_config_s;
extern struct liquid_autotest_s iirhilbf_copy_interp_s;
extern struct liquid_autotest_s iirhilbf_copy_decim_s;
// ./src/filter/tests/symsync_crcf_autotest.c
extern struct liquid_autotest_s symsync_crcf_scenario_0_s;
extern struct liquid_autotest_s symsync_crcf_scenario_1_s;
extern struct liquid_autotest_s symsync_crcf_scenario_2_s;
extern struct liquid_autotest_s symsync_crcf_scenario_3_s;
extern struct liquid_autotest_s symsync_crcf_scenario_4_s;
extern struct liquid_autotest_s symsync_crcf_scenario_5_s;
extern struct liquid_autotest_s symsync_crcf_scenario_6_s;
extern struct liquid_autotest_s symsync_crcf_scenario_7_s;
// ./src/filter/tests/iirdes_autotest.c
extern struct liquid_autotest_s iirdes_butter_2_s;
extern struct liquid_autotest_s iirdes_ellip_lowpass_0_s;
extern struct liquid_autotest_s iirdes_ellip_lowpass_1_s;
extern struct liquid_autotest_s iirdes_ellip_lowpass_2_s;
extern struct liquid_autotest_s iirdes_ellip_lowpass_3_s;
extern struct liquid_autotest_s iirdes_ellip_lowpass_4_s;
extern struct liquid_autotest_s iirdes_cheby1_lowpass_0_s;
extern struct liquid_autotest_s iirdes_cheby1_lowpass_1_s;
extern struct liquid_autotest_s iirdes_cheby1_lowpass_2_s;
extern struct liquid_autotest_s iirdes_cheby1_lowpass_3_s;
extern struct liquid_autotest_s iirdes_cheby1_lowpass_4_s;
extern struct liquid_autotest_s iirdes_cheby2_lowpass_0_s;
extern struct liquid_autotest_s iirdes_cheby2_lowpass_1_s;
extern struct liquid_autotest_s iirdes_cheby2_lowpass_2_s;
extern struct liquid_autotest_s iirdes_cheby2_lowpass_3_s;
extern struct liquid_autotest_s iirdes_cheby2_lowpass_4_s;
extern struct liquid_autotest_s iirdes_butter_lowpass_0_s;
extern struct liquid_autotest_s iirdes_butter_lowpass_1_s;
extern struct liquid_autotest_s iirdes_butter_lowpass_2_s;
extern struct liquid_autotest_s iirdes_butter_lowpass_3_s;
extern struct liquid_autotest_s iirdes_butter_lowpass_4_s;
extern struct liquid_autotest_s iirdes_ellip_highpass_s;
extern struct liquid_autotest_s iirdes_ellip_bandpass_s;
extern struct liquid_autotest_s iirdes_ellip_bandstop_s;
extern struct liquid_autotest_s iirdes_bessel_s;
// ./src/filter/tests/firdes_autotest.c
extern struct liquid_autotest_s liquid_firdes_rcos_s;
extern struct liquid_autotest_s liquid_firdes_rrcos_s;
extern struct liquid_autotest_s firdes_rrcos_s;
extern struct liquid_autotest_s firdes_rkaiser_s;
extern struct liquid_autotest_s firdes_arkaiser_s;
extern struct liquid_autotest_s liquid_firdes_dcblock_s;
extern struct liquid_autotest_s liquid_firdes_notch_s;
extern struct liquid_autotest_s liquid_getopt_str2firfilt_s;
extern struct liquid_autotest_s liquid_firdes_config_s;
extern struct liquid_autotest_s liquid_firdes_estimate_s;
extern struct liquid_autotest_s firdes_prototype_kaiser_s;
extern struct liquid_autotest_s firdes_prototype_pm_s;
extern struct liquid_autotest_s firdes_prototype_rcos_s;
extern struct liquid_autotest_s firdes_prototype_fexp_s;
extern struct liquid_autotest_s firdes_prototype_fsech_s;
extern struct liquid_autotest_s firdes_prototype_farcsech_s;
extern struct liquid_autotest_s firdes_prototype_arkaiser_s;
extern struct liquid_autotest_s firdes_prototype_rkaiser_s;
extern struct liquid_autotest_s firdes_prototype_rrcos_s;
extern struct liquid_autotest_s firdes_prototype_hm3_s;
extern struct liquid_autotest_s firdes_prototype_rfexp_s;
extern struct liquid_autotest_s firdes_prototype_rfsech_s;
extern struct liquid_autotest_s firdes_prototype_rfarcsech_s;
extern struct liquid_autotest_s firdes_doppler_s;
extern struct liquid_autotest_s liquid_freqrespf_s;
extern struct liquid_autotest_s liquid_freqrespcf_s;
// ./src/filter/tests/firhilb_autotest.c
extern struct liquid_autotest_s firhilbf_decim_s;
extern struct liquid_autotest_s firhilbf_interp_s;
extern struct liquid_autotest_s firhilbf_psd_s;
extern struct liquid_autotest_s firhilbf_config_s;
extern struct liquid_autotest_s firhilbf_copy_interp_s;
extern struct liquid_autotest_s firhilbf_copy_decim_s;
// ./src/filter/tests/lpc_autotest.c
extern struct liquid_autotest_s lpc_p4_s;
extern struct liquid_autotest_s lpc_p6_s;
extern struct liquid_autotest_s lpc_p8_s;
extern struct liquid_autotest_s lpc_p10_s;
extern struct liquid_autotest_s lpc_p16_s;
extern struct liquid_autotest_s lpc_p32_s;
// ./src/filter/tests/rresamp_crcf_partition_autotest.c
extern struct liquid_autotest_s rresamp_crcf_part_P1_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_part_P2_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_part_P3_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_part_P6_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_part_P8_Q5_s;
extern struct liquid_autotest_s rresamp_crcf_part_P9_Q5_s;
// ./src/filter/tests/firdespm_autotest.c
extern struct liquid_autotest_s firdespm_bandpass_n24_s;
extern struct liquid_autotest_s firdespm_bandpass_n32_s;
extern struct liquid_autotest_s firdespm_lowpass_s;
extern struct liquid_autotest_s firdespm_callback_s;
extern struct liquid_autotest_s firdespm_halfband_m2_ft400_s;
extern struct liquid_autotest_s firdespm_halfband_m4_ft400_s;
extern struct liquid_autotest_s firdespm_halfband_m4_ft200_s;
extern struct liquid_autotest_s firdespm_halfband_m10_ft200_s;
extern struct liquid_autotest_s firdespm_halfband_m12_ft100_s;
extern struct liquid_autotest_s firdespm_halfband_m20_ft050_s;
extern struct liquid_autotest_s firdespm_halfband_m40_ft050_s;
extern struct liquid_autotest_s firdespm_halfband_m80_ft010_s;
extern struct liquid_autotest_s firdespm_copy_s;
extern struct liquid_autotest_s firdespm_config_s;
extern struct liquid_autotest_s firdespm_differentiator_s;
extern struct liquid_autotest_s firdespm_hilbert_s;
// ./src/filter/tests/fdelay_rrrf_autotest.c
extern struct liquid_autotest_s fdelay_rrrf_0_s;
extern struct liquid_autotest_s fdelay_rrrf_1_s;
extern struct liquid_autotest_s fdelay_rrrf_2_s;
extern struct liquid_autotest_s fdelay_rrrf_3_s;
extern struct liquid_autotest_s fdelay_rrrf_4_s;
extern struct liquid_autotest_s fdelay_rrrf_5_s;
extern struct liquid_autotest_s fdelay_rrrf_6_s;
extern struct liquid_autotest_s fdelay_rrrf_7_s;
extern struct liquid_autotest_s fdelay_rrrf_8_s;
extern struct liquid_autotest_s fdelay_rrrf_9_s;
extern struct liquid_autotest_s fdelay_rrrf_config_s;
extern struct liquid_autotest_s fdelay_rrrf_push_write_s;
// ./src/filter/tests/fftfilt_xxxf_autotest.c
extern struct liquid_autotest_s fftfilt_rrrf_data_h4x256_s;
extern struct liquid_autotest_s fftfilt_rrrf_data_h7x256_s;
extern struct liquid_autotest_s fftfilt_rrrf_data_h13x256_s;
extern struct liquid_autotest_s fftfilt_rrrf_data_h23x256_s;
extern struct liquid_autotest_s fftfilt_crcf_data_h4x256_s;
extern struct liquid_autotest_s fftfilt_crcf_data_h7x256_s;
extern struct liquid_autotest_s fftfilt_crcf_data_h13x256_s;
extern struct liquid_autotest_s fftfilt_crcf_data_h23x256_s;
extern struct liquid_autotest_s fftfilt_cccf_data_h4x256_s;
extern struct liquid_autotest_s fftfilt_cccf_data_h7x256_s;
extern struct liquid_autotest_s fftfilt_cccf_data_h13x256_s;
extern struct liquid_autotest_s fftfilt_cccf_data_h23x256_s;
extern struct liquid_autotest_s fftfilt_config_s;
extern struct liquid_autotest_s fftfilt_copy_s;
// ./src/filter/tests/firfilt_cccf_notch_autotest.c
extern struct liquid_autotest_s firfilt_cccf_notch_0_s;
extern struct liquid_autotest_s firfilt_cccf_notch_1_s;
extern struct liquid_autotest_s firfilt_cccf_notch_2_s;
extern struct liquid_autotest_s firfilt_cccf_notch_3_s;
extern struct liquid_autotest_s firfilt_cccf_notch_4_s;
extern struct liquid_autotest_s firfilt_cccf_notch_5_s;
// ./src/filter/tests/iirinterp_autotest.c
extern struct liquid_autotest_s iirinterp_crcf_M2_O9_s;
extern struct liquid_autotest_s iirinterp_crcf_M3_O9_s;
extern struct liquid_autotest_s iirinterp_crcf_M4_O9_s;
extern struct liquid_autotest_s iirinterp_copy_s;
// ./src/filter/tests/rkaiser_autotest.c
extern struct liquid_autotest_s liquid_rkaiser_config_s;
// ./src/filter/tests/symsync_rrrf_autotest.c
extern struct liquid_autotest_s symsync_rrrf_scenario_0_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_1_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_2_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_3_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_4_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_5_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_6_s;
extern struct liquid_autotest_s symsync_rrrf_scenario_7_s;
// ./src/filter/tests/firfilt_xxxf_autotest.c
extern struct liquid_autotest_s firfilt_rrrf_data_h4x8_s;
extern struct liquid_autotest_s firfilt_rrrf_data_h7x16_s;
extern struct liquid_autotest_s firfilt_rrrf_data_h13x32_s;
extern struct liquid_autotest_s firfilt_rrrf_data_h23x64_s;
extern struct liquid_autotest_s firfilt_crcf_data_h4x8_s;
extern struct liquid_autotest_s firfilt_crcf_data_h7x16_s;
extern struct liquid_autotest_s firfilt_crcf_data_h13x32_s;
extern struct liquid_autotest_s firfilt_crcf_data_h23x64_s;
extern struct liquid_autotest_s firfilt_cccf_data_h4x8_s;
extern struct liquid_autotest_s firfilt_cccf_data_h7x16_s;
extern struct liquid_autotest_s firfilt_cccf_data_h13x32_s;
extern struct liquid_autotest_s firfilt_cccf_data_h23x64_s;
// ./src/filter/tests/firfilt_rnyquist_autotest.c
extern struct liquid_autotest_s firfilt_rnyquist_baseline_arkaiser_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_rkaiser_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_rrc_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_hm3_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_gmsktxrx_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_rfexp_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_rfsech_s;
extern struct liquid_autotest_s firfilt_rnyquist_baseline_rfarcsech_s;
extern struct liquid_autotest_s firfilt_rnyquist_0_s;
extern struct liquid_autotest_s firfilt_rnyquist_1_s;
extern struct liquid_autotest_s firfilt_rnyquist_2_s;
extern struct liquid_autotest_s firfilt_rnyquist_3_s;
extern struct liquid_autotest_s firfilt_rnyquist_4_s;
extern struct liquid_autotest_s firfilt_rnyquist_5_s;
extern struct liquid_autotest_s firfilt_rnyquist_6_s;
extern struct liquid_autotest_s firfilt_rnyquist_7_s;
extern struct liquid_autotest_s firfilt_rnyquist_8_s;
extern struct liquid_autotest_s firfilt_rnyquist_9_s;
// ./src/filter/tests/filter_crosscorr_autotest.c
extern struct liquid_autotest_s filter_crosscorr_rrrf_s;
// ./src/filter/tests/iirdecim_autotest.c
extern struct liquid_autotest_s iirdecim_copy_s;
// ./src/filter/tests/resamp_crcf_autotest.c
extern struct liquid_autotest_s resamp_crcf_00_s;
extern struct liquid_autotest_s resamp_crcf_01_s;
extern struct liquid_autotest_s resamp_crcf_02_s;
extern struct liquid_autotest_s resamp_crcf_03_s;
extern struct liquid_autotest_s resamp_crcf_10_s;
extern struct liquid_autotest_s resamp_crcf_11_s;
extern struct liquid_autotest_s resamp_crcf_12_s;
extern struct liquid_autotest_s resamp_crcf_13_s;
extern struct liquid_autotest_s resamp_crcf_num_output_0_s;
extern struct liquid_autotest_s resamp_crcf_num_output_1_s;
extern struct liquid_autotest_s resamp_crcf_num_output_2_s;
extern struct liquid_autotest_s resamp_crcf_num_output_3_s;
extern struct liquid_autotest_s resamp_crcf_num_output_4_s;
extern struct liquid_autotest_s resamp_crcf_num_output_5_s;
extern struct liquid_autotest_s resamp_crcf_num_output_6_s;
extern struct liquid_autotest_s resamp_crcf_num_output_7_s;
extern struct liquid_autotest_s resamp_crcf_copy_s;
// ./src/filter/tests/firdecim_xxxf_autotest.c
extern struct liquid_autotest_s firdecim_rrrf_common_s;
extern struct liquid_autotest_s firdecim_crcf_common_s;
extern struct liquid_autotest_s firdecim_rrrf_data_M2h4x20_s;
extern struct liquid_autotest_s firdecim_rrrf_data_M3h7x30_s;
extern struct liquid_autotest_s firdecim_rrrf_data_M4h13x40_s;
extern struct liquid_autotest_s firdecim_rrrf_data_M5h23x50_s;
extern struct liquid_autotest_s firdecim_crcf_data_M2h4x20_s;
extern struct liquid_autotest_s firdecim_crcf_data_M3h7x30_s;
extern struct liquid_autotest_s firdecim_crcf_data_M4h13x40_s;
extern struct liquid_autotest_s firdecim_crcf_data_M5h23x50_s;
extern struct liquid_autotest_s firdecim_cccf_data_M2h4x20_s;
extern struct liquid_autotest_s firdecim_cccf_data_M3h7x30_s;
extern struct liquid_autotest_s firdecim_cccf_data_M4h13x40_s;
extern struct liquid_autotest_s firdecim_cccf_data_M5h23x50_s;
// ./src/filter/tests/iirfiltsos_autotest.c
extern struct liquid_autotest_s iirfiltsos_impulse_n2_s;
extern struct liquid_autotest_s iirfiltsos_step_n2_s;
extern struct liquid_autotest_s iirfiltsos_copy_s;
extern struct liquid_autotest_s iirfiltsos_config_s;
// ./src/filter/tests/groupdelay_autotest.c
extern struct liquid_autotest_s fir_groupdelay_n3_s;
extern struct liquid_autotest_s iir_groupdelay_n3_s;
extern struct liquid_autotest_s iir_groupdelay_n8_s;
extern struct liquid_autotest_s iir_groupdelay_sos_n8_s;
// ./src/framing/tests/qdetector_cccf_autotest.c
extern struct liquid_autotest_s qdetector_cccf_linear_n64_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n83_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n128_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n167_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n256_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n335_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n512_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n671_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n1024_s;
extern struct liquid_autotest_s qdetector_cccf_linear_n1341_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n64_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n83_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n128_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n167_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n256_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n335_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n512_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n671_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n1024_s;
extern struct liquid_autotest_s qdetector_cccf_gmsk_n1341_s;
// ./src/framing/tests/qpilotsync_autotest.c
extern struct liquid_autotest_s qpilotsync_100_16_s;
extern struct liquid_autotest_s qpilotsync_200_20_s;
extern struct liquid_autotest_s qpilotsync_300_24_s;
extern struct liquid_autotest_s qpilotsync_400_28_s;
extern struct liquid_autotest_s qpilotsync_500_32_s;
extern struct liquid_autotest_s qpilotgen_config_s;
extern struct liquid_autotest_s qpilotsync_config_s;
// ./src/framing/tests/symstreamcf_delay_autotest.c
extern struct liquid_autotest_s symstreamcf_delay_00_s;
extern struct liquid_autotest_s symstreamcf_delay_01_s;
extern struct liquid_autotest_s symstreamcf_delay_02_s;
extern struct liquid_autotest_s symstreamcf_delay_03_s;
extern struct liquid_autotest_s symstreamcf_delay_04_s;
extern struct liquid_autotest_s symstreamcf_delay_05_s;
extern struct liquid_autotest_s symstreamcf_delay_06_s;
extern struct liquid_autotest_s symstreamcf_delay_07_s;
extern struct liquid_autotest_s symstreamcf_delay_08_s;
extern struct liquid_autotest_s symstreamcf_delay_09_s;
extern struct liquid_autotest_s symstreamcf_delay_10_s;
extern struct liquid_autotest_s symstreamcf_delay_11_s;
extern struct liquid_autotest_s symstreamcf_delay_12_s;
extern struct liquid_autotest_s symstreamcf_delay_13_s;
extern struct liquid_autotest_s symstreamcf_delay_14_s;
extern struct liquid_autotest_s symstreamcf_delay_15_s;
extern struct liquid_autotest_s symstreamcf_delay_16_s;
extern struct liquid_autotest_s symstreamcf_delay_17_s;
extern struct liquid_autotest_s symstreamcf_delay_18_s;
extern struct liquid_autotest_s symstreamcf_delay_19_s;
// ./src/framing/tests/qdetector_cccf_copy_autotest.c
extern struct liquid_autotest_s qdetector_cccf_copy_s;
// ./src/framing/tests/detector_autotest.c
extern struct liquid_autotest_s detector_cccf_n64_s;
extern struct liquid_autotest_s detector_cccf_n83_s;
extern struct liquid_autotest_s detector_cccf_n128_s;
extern struct liquid_autotest_s detector_cccf_n167_s;
extern struct liquid_autotest_s detector_cccf_n256_s;
extern struct liquid_autotest_s detector_cccf_n335_s;
extern struct liquid_autotest_s detector_cccf_n512_s;
extern struct liquid_autotest_s detector_cccf_n671_s;
extern struct liquid_autotest_s detector_cccf_n1024_s;
extern struct liquid_autotest_s detector_cccf_n1341_s;
// ./src/framing/tests/dsssframesync_autotest.c
extern struct liquid_autotest_s dsssframesync_s;
// ./src/framing/tests/bpacketsync_autotest.c
extern struct liquid_autotest_s bpacketsync_s;
// ./src/framing/tests/bsync_autotest.c
extern struct liquid_autotest_s bsync_rrrf_15_s;
extern struct liquid_autotest_s bsync_crcf_15_s;
extern struct liquid_autotest_s bsync_crcf_phase_15_s;
// ./src/framing/tests/qsource_autotest.c
extern struct liquid_autotest_s qsourcecf_config_s;
// ./src/framing/tests/dsssframe64_autotest.c
extern struct liquid_autotest_s dsssframe64sync_s;
extern struct liquid_autotest_s dsssframe64_config_s;
extern struct liquid_autotest_s dsssframe64gen_copy_s;
extern struct liquid_autotest_s dsssframe64sync_copy_s;
// ./src/framing/tests/qpacketmodem_autotest.c
extern struct liquid_autotest_s qpacketmodem_bpsk_s;
extern struct liquid_autotest_s qpacketmodem_qpsk_s;
extern struct liquid_autotest_s qpacketmodem_psk8_s;
extern struct liquid_autotest_s qpacketmodem_qam16_s;
extern struct liquid_autotest_s qpacketmodem_sqam32_s;
extern struct liquid_autotest_s qpacketmodem_qam64_s;
extern struct liquid_autotest_s qpacketmodem_sqam128_s;
extern struct liquid_autotest_s qpacketmodem_qam256_s;
extern struct liquid_autotest_s qpacketmodem_evm_s;
extern struct liquid_autotest_s qpacketmodem_unmod_bpsk_s;
extern struct liquid_autotest_s qpacketmodem_unmod_qpsk_s;
extern struct liquid_autotest_s qpacketmodem_unmod_psk8_s;
extern struct liquid_autotest_s qpacketmodem_unmod_qam16_s;
extern struct liquid_autotest_s qpacketmodem_unmod_sqam32_s;
extern struct liquid_autotest_s qpacketmodem_unmod_qam64_s;
extern struct liquid_autotest_s qpacketmodem_unmod_sqam128_s;
extern struct liquid_autotest_s qpacketmodem_unmod_qam256_s;
extern struct liquid_autotest_s qpacketmodem_copy_s;
// ./src/framing/tests/symstreamrcf_delay_autotest.c
extern struct liquid_autotest_s symstreamrcf_delay_00_s;
extern struct liquid_autotest_s symstreamrcf_delay_01_s;
extern struct liquid_autotest_s symstreamrcf_delay_02_s;
extern struct liquid_autotest_s symstreamrcf_delay_03_s;
extern struct liquid_autotest_s symstreamrcf_delay_04_s;
extern struct liquid_autotest_s symstreamrcf_delay_05_s;
extern struct liquid_autotest_s symstreamrcf_delay_06_s;
extern struct liquid_autotest_s symstreamrcf_delay_07_s;
extern struct liquid_autotest_s symstreamrcf_delay_08_s;
extern struct liquid_autotest_s symstreamrcf_delay_09_s;
extern struct liquid_autotest_s symstreamrcf_delay_10_s;
extern struct liquid_autotest_s symstreamrcf_delay_11_s;
extern struct liquid_autotest_s symstreamrcf_delay_12_s;
extern struct liquid_autotest_s symstreamrcf_delay_13_s;
extern struct liquid_autotest_s symstreamrcf_delay_14_s;
extern struct liquid_autotest_s symstreamrcf_delay_15_s;
extern struct liquid_autotest_s symstreamrcf_delay_16_s;
extern struct liquid_autotest_s symstreamrcf_delay_17_s;
extern struct liquid_autotest_s symstreamrcf_delay_18_s;
extern struct liquid_autotest_s symstreamrcf_delay_19_s;
// ./src/framing/tests/qdsync_cccf_autotest.c
extern struct liquid_autotest_s qdsync_cccf_k2_s;
extern struct liquid_autotest_s qdsync_cccf_k3_s;
extern struct liquid_autotest_s qdsync_cccf_k4_s;
extern struct liquid_autotest_s qdsync_set_buf_len_s;
extern struct liquid_autotest_s qdsync_cccf_copy_s;
extern struct liquid_autotest_s qdsync_cccf_config_s;
// ./src/framing/tests/symtrack_cccf_autotest.c
extern struct liquid_autotest_s symtrack_cccf_bpsk_s;
extern struct liquid_autotest_s symtrack_cccf_qpsk_s;
extern struct liquid_autotest_s symtrack_cccf_config_invalid_s;
extern struct liquid_autotest_s symtrack_cccf_config_valid_s;
// ./src/framing/tests/symstreamcf_autotest.c
extern struct liquid_autotest_s symstreamcf_psd_k2_m12_b030_s;
extern struct liquid_autotest_s symstreamcf_psd_k4_m12_b030_s;
extern struct liquid_autotest_s symstreamcf_psd_k4_m25_b020_s;
extern struct liquid_autotest_s symstreamcf_psd_k7_m11_b035_s;
extern struct liquid_autotest_s symstreamcf_copy_s;
// ./src/framing/tests/ofdmflexframe_autotest.c
extern struct liquid_autotest_s ofdmflexframe_00_s;
extern struct liquid_autotest_s ofdmflexframe_01_s;
extern struct liquid_autotest_s ofdmflexframe_02_s;
extern struct liquid_autotest_s ofdmflexframe_03_s;
extern struct liquid_autotest_s ofdmflexframe_04_s;
extern struct liquid_autotest_s ofdmflexframe_05_s;
extern struct liquid_autotest_s ofdmflexframe_06_s;
extern struct liquid_autotest_s ofdmflexframe_07_s;
extern struct liquid_autotest_s ofdmflexframe_08_s;
extern struct liquid_autotest_s ofdmflexframe_09_s;
extern struct liquid_autotest_s ofdmflexframegen_config_s;
extern struct liquid_autotest_s ofdmflexframesync_config_s;
// ./src/framing/tests/fskframesync_autotest.c
extern struct liquid_autotest_s fskframesync_s;
// ./src/framing/tests/symstreamrcf_autotest.c
extern struct liquid_autotest_s symstreamrcf_psd_bw200_m12_b030_s;
extern struct liquid_autotest_s symstreamrcf_psd_bw400_m12_b030_s;
extern struct liquid_autotest_s symstreamrcf_psd_bw400_m25_b020_s;
extern struct liquid_autotest_s symstreamrcf_psd_bw700_m11_b035_s;
extern struct liquid_autotest_s symstreamrcf_copy_s;
// ./src/framing/tests/framesync64_autotest.c
extern struct liquid_autotest_s framesync64_s;
extern struct liquid_autotest_s framegen64_copy_s;
extern struct liquid_autotest_s framesync64_copy_s;
extern struct liquid_autotest_s framesync64_config_s;
extern struct liquid_autotest_s framesync64_debug_none_s;
extern struct liquid_autotest_s framesync64_debug_user_s;
extern struct liquid_autotest_s framesync64_debug_ndet_s;
extern struct liquid_autotest_s framesync64_debug_head_s;
extern struct liquid_autotest_s framesync64_debug_rand_s;
extern struct liquid_autotest_s framesync64_estimation_s;
// ./src/framing/tests/flexframesync_autotest.c
extern struct liquid_autotest_s flexframesync_s;
// ./src/framing/tests/gmskframe_autotest.c
extern struct liquid_autotest_s gmskframesync_process_s;
extern struct liquid_autotest_s gmskframesync_multiple_s;
extern struct liquid_autotest_s gmskframesync_k02_m05_bt20_s;
extern struct liquid_autotest_s gmskframesync_k02_m05_bt30_s;
extern struct liquid_autotest_s gmskframesync_k02_m05_bt40_s;
extern struct liquid_autotest_s gmskframesync_k04_m05_bt20_s;
extern struct liquid_autotest_s gmskframesync_k04_m05_bt30_s;
extern struct liquid_autotest_s gmskframesync_k04_m05_bt40_s;
extern struct liquid_autotest_s gmskframesync_k03_m07_bt20_s;
extern struct liquid_autotest_s gmskframesync_k08_m20_bt15_s;
extern struct liquid_autotest_s gmskframesync_k15_m02_bt40_s;
// ./src/framing/tests/msource_autotest.c
extern struct liquid_autotest_s msourcecf_tone_s;
extern struct liquid_autotest_s msourcecf_chirp_s;
extern struct liquid_autotest_s msourcecf_aggregate_s;
extern struct liquid_autotest_s msourcecf_config_s;
extern struct liquid_autotest_s msourcecf_accessor_s;
extern struct liquid_autotest_s msourcecf_copy_s;
// ./src/equalization/tests/eqlms_cccf_autotest.c
extern struct liquid_autotest_s eqlms_00_s;
extern struct liquid_autotest_s eqlms_01_s;
extern struct liquid_autotest_s eqlms_02_s;
extern struct liquid_autotest_s eqlms_03_s;
extern struct liquid_autotest_s eqlms_04_s;
extern struct liquid_autotest_s eqlms_05_s;
extern struct liquid_autotest_s eqlms_06_s;
extern struct liquid_autotest_s eqlms_07_s;
extern struct liquid_autotest_s eqlms_08_s;
extern struct liquid_autotest_s eqlms_09_s;
extern struct liquid_autotest_s eqlms_10_s;
extern struct liquid_autotest_s eqlms_11_s;
extern struct liquid_autotest_s eqlms_config_s;
extern struct liquid_autotest_s eqlms_cccf_copy_s;
// ./src/equalization/tests/eqrls_rrrf_autotest.c
extern struct liquid_autotest_s eqrls_rrrf_01_s;
extern struct liquid_autotest_s eqrls_rrrf_copy_s;
// ./src/matrix/tests/smatrixb_autotest.c
extern struct liquid_autotest_s smatrixb_vmul_s;
extern struct liquid_autotest_s smatrixb_mul_s;
extern struct liquid_autotest_s smatrixb_mulf_s;
extern struct liquid_autotest_s smatrixb_vmulf_s;
// ./src/matrix/tests/matrixcf_autotest.c
extern struct liquid_autotest_s matrixcf_add_s;
extern struct liquid_autotest_s matrixcf_aug_s;
extern struct liquid_autotest_s matrixcf_chol_s;
extern struct liquid_autotest_s matrixcf_inv_s;
extern struct liquid_autotest_s matrixcf_linsolve_s;
extern struct liquid_autotest_s matrixcf_ludecomp_crout_s;
extern struct liquid_autotest_s matrixcf_ludecomp_doolittle_s;
extern struct liquid_autotest_s matrixcf_mul_s;
extern struct liquid_autotest_s matrixcf_qrdecomp_s;
extern struct liquid_autotest_s matrixcf_transmul_s;
// ./src/matrix/tests/matrixf_autotest.c
extern struct liquid_autotest_s matrixf_add_s;
extern struct liquid_autotest_s matrixf_aug_s;
extern struct liquid_autotest_s matrixf_cgsolve_s;
extern struct liquid_autotest_s matrixf_chol_s;
extern struct liquid_autotest_s matrixf_gramschmidt_s;
extern struct liquid_autotest_s matrixf_inv_s;
extern struct liquid_autotest_s matrixf_linsolve_s;
extern struct liquid_autotest_s matrixf_ludecomp_crout_s;
extern struct liquid_autotest_s matrixf_ludecomp_doolittle_s;
extern struct liquid_autotest_s matrixf_mul_s;
extern struct liquid_autotest_s matrixf_qrdecomp_s;
extern struct liquid_autotest_s matrixf_transmul_s;
// ./src/matrix/tests/smatrixf_autotest.c
extern struct liquid_autotest_s smatrixf_vmul_s;
extern struct liquid_autotest_s smatrixf_mul_s;
// ./src/matrix/tests/smatrixi_autotest.c
extern struct liquid_autotest_s smatrixi_vmul_s;
extern struct liquid_autotest_s smatrixi_mul_s;
// ./src/buffer/tests/buffer_window_autotest.c
extern struct liquid_autotest_s window_config_errors_s;
extern struct liquid_autotest_s windowf_s;
extern struct liquid_autotest_s window_copy_s;
// ./src/buffer/tests/wdelay_autotest.c
extern struct liquid_autotest_s wdelayf_s;
extern struct liquid_autotest_s wdelay_copy_s;
// ./src/buffer/tests/cbuffer_autotest.c
extern struct liquid_autotest_s cbufferf_s;
extern struct liquid_autotest_s cbuffercf_s;
extern struct liquid_autotest_s cbufferf_flow_s;
extern struct liquid_autotest_s cbufferf_config_s;
extern struct liquid_autotest_s cbuffer_copy_s;

// compile test registry
liquid_autotest liquid_autotest_registry[] =
{
    &null_s,
    &libliquid_version_s,
    &hamming128_codec_s,
    &hamming128_codec_soft_s,
    &secded2216_codec_e0_s,
    &secded2216_codec_e1_s,
    &secded2216_codec_e2_s,
    &hamming3126_codec_s,
    &packetizer_n16_0_0_s,
    &packetizer_n16_0_1_s,
    &packetizer_n16_0_2_s,
    &fec_copy_r3_s,
    &fec_copy_r5_s,
    &fec_copy_h74_s,
    &fec_copy_h84_s,
    &fec_copy_h128_s,
    &fec_copy_g2412_s,
    &fec_copy_secded2216_s,
    &fec_copy_secded3932_s,
    &fec_copy_secded7264_s,
    &fec_copy_v27_s,
    &fec_copy_v29_s,
    &fec_copy_v39_s,
    &fec_copy_v615_s,
    &fec_copy_v27p23_s,
    &fec_copy_v27p34_s,
    &fec_copy_v27p45_s,
    &fec_copy_v27p56_s,
    &fec_copy_v27p67_s,
    &fec_copy_v27p78_s,
    &fec_copy_v29p23_s,
    &fec_copy_v29p34_s,
    &fec_copy_v29p45_s,
    &fec_copy_v29p56_s,
    &fec_copy_v29p67_s,
    &fec_copy_v29p78_s,
    &fec_copy_rs8_s,
    &hamming1511_codec_s,
    &rep3_codec_s,
    &reedsolomon_223_255_s,
    &secded7264_codec_e0_s,
    &secded7264_codec_e1_s,
    &secded7264_codec_e2_s,
    &hamming84_codec_s,
    &hamming84_codec_soft_s,
    &packetizer_copy_s,
    &fec_r3_s,
    &fec_r5_s,
    &fec_h74_s,
    &fec_h84_s,
    &fec_h128_s,
    &fec_g2412_s,
    &fec_secded2216_s,
    &fec_secded3932_s,
    &fec_secded7264_s,
    &fec_v27_s,
    &fec_v29_s,
    &fec_v39_s,
    &fec_v615_s,
    &fec_v27p23_s,
    &fec_v27p34_s,
    &fec_v27p45_s,
    &fec_v27p56_s,
    &fec_v27p67_s,
    &fec_v27p78_s,
    &fec_v29p23_s,
    &fec_v29p34_s,
    &fec_v29p45_s,
    &fec_v29p56_s,
    &fec_v29p67_s,
    &fec_v29p78_s,
    &fec_rs8_s,
    &golay2412_codec_s,
    &fecsoft_r3_s,
    &fecsoft_r5_s,
    &fecsoft_h74_s,
    &fecsoft_h84_s,
    &fecsoft_h128_s,
    &fecsoft_v27_s,
    &fecsoft_v29_s,
    &fecsoft_v39_s,
    &fecsoft_v615_s,
    &fecsoft_v27p23_s,
    &fecsoft_v27p34_s,
    &fecsoft_v27p45_s,
    &fecsoft_v27p56_s,
    &fecsoft_v27p67_s,
    &fecsoft_v27p78_s,
    &fecsoft_v29p23_s,
    &fecsoft_v29p34_s,
    &fecsoft_v29p45_s,
    &fecsoft_v29p56_s,
    &fecsoft_v29p67_s,
    &fecsoft_v29p78_s,
    &fecsoft_rs8_s,
    &fec_config_s,
    &fec_str2fec_s,
    &fec_is_convolutional_s,
    &fec_is_punctured_s,
    &fec_is_reedsolomon_s,
    &fec_is_hamming_s,
    &hamming74_codec_s,
    &hamming74_codec_soft_s,
    &reverse_byte_s,
    &reverse_uint16_s,
    &reverse_uint32_s,
    &checksum_s,
    &crc8_s,
    &crc16_s,
    &crc24_s,
    &crc32_s,
    &crc_config_s,
    &rep5_codec_s,
    &secded3932_codec_e0_s,
    &secded3932_codec_e1_s,
    &secded3932_codec_e2_s,
    &interleaver_hard_8_s,
    &interleaver_hard_16_s,
    &interleaver_hard_64_s,
    &interleaver_hard_256_s,
    &interleaver_soft_8_s,
    &interleaver_soft_16_s,
    &interleaver_soft_64_s,
    &interleaver_soft_256_s,
    &firpfbchr_crcf_s,
    &firpfbchr_crcf_config_s,
    &firpfbch_crcf_analysis_s,
    &ofdmframesync_acquire_n64_s,
    &ofdmframesync_acquire_n128_s,
    &ofdmframesync_acquire_n256_s,
    &ofdmframesync_acquire_n512_s,
    &ofdmframe_common_config_s,
    &ofdmframegen_config_s,
    &ofdmframesync_config_s,
    &firpfbch2_crcf_n8_s,
    &firpfbch2_crcf_n16_s,
    &firpfbch2_crcf_n32_s,
    &firpfbch2_crcf_n64_s,
    &firpfbch2_crcf_copy_s,
    &firpfbch2_crcf_config_s,
    &firpfbch_crcf_config_s,
    &firpfbch_crcf_synthesis_s,
    &gasearch_peak_s,
    &chromosome_config_s,
    &gasearch_config_s,
    &qs1dsearch_min_01_s,
    &qs1dsearch_min_02_s,
    &qs1dsearch_min_03_s,
    &qs1dsearch_min_05_s,
    &qs1dsearch_min_06_s,
    &qs1dsearch_min_07_s,
    &qs1dsearch_min_08_s,
    &qs1dsearch_min_10_s,
    &qs1dsearch_min_11_s,
    &qs1dsearch_min_12_s,
    &qs1dsearch_min_13_s,
    &qs1dsearch_max_01_s,
    &qs1dsearch_max_02_s,
    &qs1dsearch_max_03_s,
    &qs1dsearch_max_05_s,
    &qs1dsearch_max_06_s,
    &qs1dsearch_max_07_s,
    &qs1dsearch_max_08_s,
    &qs1dsearch_max_10_s,
    &qs1dsearch_max_11_s,
    &qs1dsearch_max_12_s,
    &qs1dsearch_max_13_s,
    &qs1dsearch_config_s,
    &gradsearch_rosenbrock_s,
    &gradsearch_maxutility_s,
    &optim_rosenbrock_s,
    &qnsearch_rosenbrock_s,
    &qnsearch_config_s,
    &dotprod_crcf_rand01_s,
    &dotprod_crcf_rand02_s,
    &dotprod_crcf_struct_vs_ordinal_s,
    &sumsqcf_3_s,
    &sumsqcf_4_s,
    &sumsqcf_7_s,
    &sumsqcf_8_s,
    &sumsqcf_15_s,
    &sumsqcf_16_s,
    &sumsqf_3_s,
    &sumsqf_4_s,
    &sumsqf_7_s,
    &sumsqf_8_s,
    &sumsqf_15_s,
    &sumsqf_16_s,
    &dotprod_cccf_rand16_s,
    &dotprod_cccf_struct_lengths_s,
    &dotprod_cccf_struct_vs_ordinal_s,
    &dotprod_rrrf_basic_s,
    &dotprod_rrrf_uneven_s,
    &dotprod_rrrf_struct_s,
    &dotprod_rrrf_struct_align_s,
    &dotprod_rrrf_rand01_s,
    &dotprod_rrrf_rand02_s,
    &dotprod_rrrf_struct_lengths_s,
    &dotprod_rrrf_struct_vs_ordinal_s,
    &prime_small_s,
    &factors_s,
    &totient_s,
    &gamma_s,
    &lngamma_s,
    &uppergamma_s,
    &factorial_s,
    &nchoosek_s,
    &Q_s,
    &MarcumQf_s,
    &MarcumQ1f_s,
    &sincf_s,
    &nextpow2_s,
    &math_config_s,
    &polyf_findroots_real_s,
    &polyf_findroots_complex_s,
    &polyf_findroots_mix_s,
    &polyf_findroots_mix2_s,
    &polyf_findroots_rand_s,
    &cexpf_s,
    &clogf_s,
    &csqrtf_s,
    &casinf_s,
    &cacosf_s,
    &catanf_s,
    &cargf_s,
    &gcd_one_s,
    &gcd_edge_cases_s,
    &gcd_base_s,
    &polyf_fit_q3n3_s,
    &polyf_lagrange_issue165_s,
    &polyf_lagrange_s,
    &polyf_expandbinomial_4_s,
    &polyf_expandroots_4_s,
    &polyf_expandroots_11_s,
    &polycf_expandroots_4_s,
    &polyf_expandroots2_3_s,
    &polyf_mul_2_3_s,
    &poly_expandbinomial_n6_s,
    &poly_binomial_expand_pm_m6_k1_s,
    &poly_expandbinomial_pm_m5_k2_s,
    &lnbesselif_s,
    &besselif_s,
    &besseli0f_s,
    &besseljf_s,
    &besselj0f_s,
    &window_hamming_s,
    &window_hann_s,
    &window_blackmanharris_s,
    &window_blackmanharris7_s,
    &window_kaiser_s,
    &window_flattop_s,
    &window_triangular_s,
    &window_rcostaper_s,
    &window_kbd_s,
    &kbd_n16_s,
    &kbd_n32_s,
    &kbd_n48_s,
    &window_config_s,
    &compand_float_s,
    &compand_cfloat_s,
    &quantize_float_n8_s,
    &agc_crcf_dc_gain_control_s,
    &agc_crcf_scale_s,
    &agc_crcf_ac_gain_control_s,
    &agc_crcf_rssi_sinusoid_s,
    &agc_crcf_rssi_noise_s,
    &agc_crcf_squelch_s,
    &agc_crcf_lock_s,
    &agc_crcf_config_s,
    &agc_crcf_copy_s,
    &cvsd_rmse_sine_s,
    &cvsd_rmse_sine8_s,
    &cvsd_config_s,
    &nco_crcf_phase_s,
    &nco_basic_s,
    &nco_mixing_s,
    &nco_block_mixing_s,
    &nco_crcf_pll_phase_s,
    &nco_crcf_pll_freq_s,
    &nco_unwrap_phase_s,
    &nco_crcf_frequency_s,
    &nco_crcf_constrain_s,
    &nco_crcf_copy_s,
    &nco_config_s,
    &nco_crcf_spectrum_nco_f00_s,
    &nco_crcf_spectrum_nco_f01_s,
    &nco_crcf_spectrum_nco_f02_s,
    &nco_crcf_spectrum_nco_f03_s,
    &nco_crcf_spectrum_nco_f04_s,
    &nco_crcf_spectrum_vco_f00_s,
    &nco_crcf_spectrum_vco_f01_s,
    &nco_crcf_spectrum_vco_f02_s,
    &nco_crcf_spectrum_vco_f03_s,
    &nco_crcf_spectrum_vco_f04_s,
    &nco_crcf_mix_nco_0_s,
    &nco_crcf_mix_nco_1_s,
    &nco_crcf_mix_nco_2_s,
    &nco_crcf_mix_nco_3_s,
    &nco_crcf_mix_nco_4_s,
    &nco_crcf_mix_nco_5_s,
    &nco_crcf_mix_nco_6_s,
    &nco_crcf_mix_nco_7_s,
    &nco_crcf_mix_nco_8_s,
    &nco_crcf_mix_nco_9_s,
    &nco_crcf_mix_vco_0_s,
    &nco_crcf_mix_vco_1_s,
    &nco_crcf_mix_vco_2_s,
    &nco_crcf_mix_vco_3_s,
    &nco_crcf_mix_vco_4_s,
    &nco_crcf_mix_vco_5_s,
    &nco_crcf_mix_vco_6_s,
    &nco_crcf_mix_vco_7_s,
    &nco_crcf_mix_vco_8_s,
    &nco_crcf_mix_vco_9_s,
    &cpfskmodem_bps1_h0p5000_k4_m3_square_s,
    &cpfskmodem_bps1_h0p0250_k4_m3_square_s,
    &cpfskmodem_bps1_h0p1250_k4_m3_square_s,
    &cpfskmodem_bps1_h0p0625_k4_m3_square_s,
    &cpfskmodem_bps1_h0p5000_k4_m3_rcosfull_s,
    &cpfskmodem_bps1_h0p0250_k4_m3_rcosfull_s,
    &cpfskmodem_bps1_h0p1250_k4_m3_rcosfull_s,
    &cpfskmodem_bps1_h0p0625_k4_m3_rcosfull_s,
    &cpfskmodem_bps1_h0p5000_k4_m3_rcospart_s,
    &cpfskmodem_bps1_h0p0250_k4_m3_rcospart_s,
    &cpfskmodem_bps1_h0p1250_k4_m3_rcospart_s,
    &cpfskmodem_bps1_h0p0625_k4_m3_rcospart_s,
    &cpfskmodem_bps1_h0p5000_k4_m3_gmsk_s,
    &cpfskmodem_bps1_h0p0250_k4_m3_gmsk_s,
    &cpfskmodem_bps1_h0p1250_k4_m3_gmsk_s,
    &cpfskmodem_bps1_h0p0625_k4_m3_gmsk_s,
    &cpfskmodem_bps2_h0p0250_k4_m3_square_s,
    &cpfskmodem_bps3_h0p1250_k4_m3_square_s,
    &cpfskmodem_bps4_h0p0625_k4_m3_square_s,
    &cpfskmodem_bps1_h0p5_k2_m7_gmsk_s,
    &cpfskmodem_bps1_h0p5_k4_m7_gmsk_s,
    &cpfskmodem_bps1_h0p5_k6_m7_gmsk_s,
    &cpfskmodem_bps1_h0p5_k8_m7_gmsk_s,
    &cpfskmodem_spectrum_s,
    &cpfskmodem_config_s,
    &freqmodem_kf_0_02_s,
    &freqmodem_kf_0_04_s,
    &freqmodem_kf_0_08_s,
    &gmskmodem_k4_m3_b025_s,
    &gmskmodem_k2_m3_b025_s,
    &gmskmodem_k3_m3_b025_s,
    &gmskmodem_k5_m3_b025_s,
    &gmskmodem_k8_m3_b033_s,
    &gmskmodem_k4_m1_b025_s,
    &gmskmodem_k4_m2_b025_s,
    &gmskmodem_k4_m8_b025_s,
    &gmskmodem_k4_m3_b020_s,
    &gmskmodem_k4_m3_b033_s,
    &gmskmodem_k4_m3_b050_s,
    &gmskmod_copy_s,
    &gmskdem_copy_s,
    &modem_copy_psk2_s,
    &modem_copy_psk4_s,
    &modem_copy_psk8_s,
    &modem_copy_psk16_s,
    &modem_copy_psk32_s,
    &modem_copy_psk64_s,
    &modem_copy_psk128_s,
    &modem_copy_psk256_s,
    &modem_copy_dpsk2_s,
    &modem_copy_dpsk4_s,
    &modem_copy_dpsk8_s,
    &modem_copy_dpsk16_s,
    &modem_copy_dpsk32_s,
    &modem_copy_dpsk64_s,
    &modem_copy_dpsk128_s,
    &modem_copy_dpsk256_s,
    &modem_copy_ask2_s,
    &modem_copy_ask4_s,
    &modem_copy_ask8_s,
    &modem_copy_ask16_s,
    &modem_copy_ask32_s,
    &modem_copy_ask64_s,
    &modem_copy_ask128_s,
    &modem_copy_ask256_s,
    &modem_copy_qam4_s,
    &modem_copy_qam8_s,
    &modem_copy_qam16_s,
    &modem_copy_qam32_s,
    &modem_copy_qam64_s,
    &modem_copy_qam128_s,
    &modem_copy_qam256_s,
    &modem_copy_apsk4_s,
    &modem_copy_apsk8_s,
    &modem_copy_apsk16_s,
    &modem_copy_apsk32_s,
    &modem_copy_apsk64_s,
    &modem_copy_apsk128_s,
    &modem_copy_apsk256_s,
    &modem_copy_bpsk_s,
    &modem_copy_qpsk_s,
    &modem_copy_ook_s,
    &modem_copy_sqam32_s,
    &modem_copy_sqam128_s,
    &modem_copy_V29_s,
    &modem_copy_arb16opt_s,
    &modem_copy_arb32opt_s,
    &modem_copy_arb64opt_s,
    &modem_copy_arb128opt_s,
    &modem_copy_arb256opt_s,
    &modem_copy_arb64vt_s,
    &modem_copy_pi4dqpsk_s,
    &modem_config_s,
    &ampmodem_dsb_carrier_on_s,
    &ampmodem_usb_carrier_on_s,
    &ampmodem_lsb_carrier_on_s,
    &ampmodem_dsb_carrier_off_s,
    &ampmodem_usb_carrier_off_s,
    &ampmodem_lsb_carrier_off_s,
    &mod_demod_psk2_s,
    &mod_demod_psk4_s,
    &mod_demod_psk8_s,
    &mod_demod_psk16_s,
    &mod_demod_psk32_s,
    &mod_demod_psk64_s,
    &mod_demod_psk128_s,
    &mod_demod_psk256_s,
    &mod_demod_dpsk2_s,
    &mod_demod_dpsk4_s,
    &mod_demod_dpsk8_s,
    &mod_demod_dpsk16_s,
    &mod_demod_dpsk32_s,
    &mod_demod_dpsk64_s,
    &mod_demod_dpsk128_s,
    &mod_demod_dpsk256_s,
    &mod_demod_ask2_s,
    &mod_demod_ask4_s,
    &mod_demod_ask8_s,
    &mod_demod_ask16_s,
    &mod_demod_ask32_s,
    &mod_demod_ask64_s,
    &mod_demod_ask128_s,
    &mod_demod_ask256_s,
    &mod_demod_qam4_s,
    &mod_demod_qam8_s,
    &mod_demod_qam16_s,
    &mod_demod_qam32_s,
    &mod_demod_qam64_s,
    &mod_demod_qam128_s,
    &mod_demod_qam256_s,
    &mod_demod_apsk4_s,
    &mod_demod_apsk8_s,
    &mod_demod_apsk16_s,
    &mod_demod_apsk32_s,
    &mod_demod_apsk64_s,
    &mod_demod_apsk128_s,
    &mod_demod_apsk256_s,
    &mod_demod_bpsk_s,
    &mod_demod_qpsk_s,
    &mod_demod_ook_s,
    &mod_demod_sqam32_s,
    &mod_demod_sqam128_s,
    &mod_demod_V29_s,
    &mod_demod_arb16opt_s,
    &mod_demod_arb32opt_s,
    &mod_demod_arb64opt_s,
    &mod_demod_arb128opt_s,
    &mod_demod_arb256opt_s,
    &mod_demod_arb64vt_s,
    &mod_demod_pi4dqpsk_s,
    &demodsoft_psk2_s,
    &demodsoft_psk4_s,
    &demodsoft_psk8_s,
    &demodsoft_psk16_s,
    &demodsoft_psk32_s,
    &demodsoft_psk64_s,
    &demodsoft_psk128_s,
    &demodsoft_psk256_s,
    &demodsoft_dpsk2_s,
    &demodsoft_dpsk4_s,
    &demodsoft_dpsk8_s,
    &demodsoft_dpsk16_s,
    &demodsoft_dpsk32_s,
    &demodsoft_dpsk64_s,
    &demodsoft_dpsk128_s,
    &demodsoft_dpsk256_s,
    &demodsoft_ask2_s,
    &demodsoft_ask4_s,
    &demodsoft_ask8_s,
    &demodsoft_ask16_s,
    &demodsoft_ask32_s,
    &demodsoft_ask64_s,
    &demodsoft_ask128_s,
    &demodsoft_ask256_s,
    &demodsoft_qam4_s,
    &demodsoft_qam8_s,
    &demodsoft_qam16_s,
    &demodsoft_qam32_s,
    &demodsoft_qam64_s,
    &demodsoft_qam128_s,
    &demodsoft_qam256_s,
    &demodsoft_apsk4_s,
    &demodsoft_apsk8_s,
    &demodsoft_apsk16_s,
    &demodsoft_apsk32_s,
    &demodsoft_apsk64_s,
    &demodsoft_apsk128_s,
    &demodsoft_apsk256_s,
    &demodsoft_bpsk_s,
    &demodsoft_qpsk_s,
    &demodsoft_ook_s,
    &demodsoft_sqam32_s,
    &demodsoft_sqam128_s,
    &demodsoft_V29_s,
    &demodsoft_arb16opt_s,
    &demodsoft_arb32opt_s,
    &demodsoft_arb64opt_s,
    &demodsoft_arb128opt_s,
    &demodsoft_arb256opt_s,
    &demodsoft_arb64vt_s,
    &demodsoft_pi4dqpsk_s,
    &demodstats_psk2_s,
    &demodstats_psk4_s,
    &demodstats_psk8_s,
    &demodstats_psk16_s,
    &demodstats_psk32_s,
    &demodstats_psk64_s,
    &demodstats_psk128_s,
    &demodstats_psk256_s,
    &demodstats_dpsk2_s,
    &demodstats_dpsk4_s,
    &demodstats_dpsk8_s,
    &demodstats_dpsk16_s,
    &demodstats_dpsk32_s,
    &demodstats_dpsk64_s,
    &demodstats_dpsk128_s,
    &demodstats_dpsk256_s,
    &demodstats_ask2_s,
    &demodstats_ask4_s,
    &demodstats_ask8_s,
    &demodstats_ask16_s,
    &demodstats_ask32_s,
    &demodstats_ask64_s,
    &demodstats_ask128_s,
    &demodstats_ask256_s,
    &demodstats_qam4_s,
    &demodstats_qam8_s,
    &demodstats_qam16_s,
    &demodstats_qam32_s,
    &demodstats_qam64_s,
    &demodstats_qam128_s,
    &demodstats_qam256_s,
    &demodstats_apsk4_s,
    &demodstats_apsk8_s,
    &demodstats_apsk16_s,
    &demodstats_apsk32_s,
    &demodstats_apsk64_s,
    &demodstats_apsk128_s,
    &demodstats_apsk256_s,
    &demodstats_bpsk_s,
    &demodstats_qpsk_s,
    &demodstats_ook_s,
    &demodstats_sqam32_s,
    &demodstats_sqam128_s,
    &demodstats_V29_s,
    &demodstats_arb16opt_s,
    &demodstats_arb32opt_s,
    &demodstats_arb64opt_s,
    &demodstats_arb128opt_s,
    &demodstats_arb256opt_s,
    &demodstats_arb64vt_s,
    &modemcf_print_schemes_s,
    &modemcf_str2mod_s,
    &modemcf_types_s,
    &fskmodem_norm_M2_s,
    &fskmodem_norm_M4_s,
    &fskmodem_norm_M8_s,
    &fskmodem_norm_M16_s,
    &fskmodem_norm_M32_s,
    &fskmodem_norm_M64_s,
    &fskmodem_norm_M128_s,
    &fskmodem_norm_M256_s,
    &fskmodem_norm_M512_s,
    &fskmodem_norm_M1024_s,
    &fskmodem_misc_M2_s,
    &fskmodem_misc_M4_s,
    &fskmodem_misc_M8_s,
    &fskmodem_misc_M16_s,
    &fskmodem_misc_M32_s,
    &fskmodem_misc_M64_s,
    &fskmodem_misc_M128_s,
    &fskmodem_misc_M256_s,
    &fskmodem_misc_M512_s,
    &fskmodem_misc_M1024_s,
    &fskmod_copy_s,
    &fskdem_copy_s,
    &spwaterfall_config_s,
    &spwaterfallcf_noise_440_s,
    &spwaterfallcf_noise_1024_s,
    &spwaterfallcf_noise_1200_s,
    &spwaterfall_operation_s,
    &spwaterfall_copy_s,
    &spwaterfall_gnuplot_s,
    &fft_3_s,
    &fft_5_s,
    &fft_6_s,
    &fft_7_s,
    &fft_9_s,
    &fft_2_s,
    &fft_4_s,
    &fft_8_s,
    &fft_16_s,
    &fft_32_s,
    &fft_64_s,
    &fft_10_s,
    &fft_21_s,
    &fft_22_s,
    &fft_24_s,
    &fft_26_s,
    &fft_30_s,
    &fft_35_s,
    &fft_36_s,
    &fft_48_s,
    &fft_63_s,
    &fft_92_s,
    &fft_96_s,
    &fft_120_s,
    &fft_130_s,
    &fft_192_s,
    &fft_17_s,
    &fft_43_s,
    &fft_79_s,
    &fft_157_s,
    &fft_317_s,
    &fft_509_s,
    &fft_r2r_REDFT00_n8_s,
    &fft_r2r_REDFT10_n8_s,
    &fft_r2r_REDFT01_n8_s,
    &fft_r2r_REDFT11_n8_s,
    &fft_r2r_RODFT00_n8_s,
    &fft_r2r_RODFT10_n8_s,
    &fft_r2r_RODFT01_n8_s,
    &fft_r2r_RODFT11_n8_s,
    &fft_r2r_REDFT00_n27_s,
    &fft_r2r_REDFT10_n27_s,
    &fft_r2r_REDFT01_n27_s,
    &fft_r2r_REDFT11_n27_s,
    &fft_r2r_RODFT00_n27_s,
    &fft_r2r_RODFT10_n27_s,
    &fft_r2r_RODFT01_n27_s,
    &fft_r2r_RODFT11_n27_s,
    &fft_r2r_REDFT00_n32_s,
    &fft_r2r_REDFT10_n32_s,
    &fft_r2r_REDFT01_n32_s,
    &fft_r2r_REDFT11_n32_s,
    &fft_r2r_RODFT00_n32_s,
    &fft_r2r_RODFT10_n32_s,
    &fft_r2r_RODFT01_n32_s,
    &fft_r2r_RODFT11_n32_s,
    &fft_shift_4_s,
    &fft_shift_8_s,
    &asgramcf_copy_s,
    &asgramcf_config_s,
    &spgramcf_noise_440_s,
    &spgramcf_noise_1024_s,
    &spgramcf_noise_1200_s,
    &spgramcf_noise_custom_0_s,
    &spgramcf_noise_custom_1_s,
    &spgramcf_noise_custom_2_s,
    &spgramcf_noise_custom_3_s,
    &spgramcf_noise_hamming_s,
    &spgramcf_noise_hann_s,
    &spgramcf_noise_blackmanharris_s,
    &spgramcf_noise_blackmanharris7_s,
    &spgramcf_noise_kaiser_s,
    &spgramcf_noise_flattop_s,
    &spgramcf_noise_triangular_s,
    &spgramcf_noise_rcostaper_s,
    &spgramcf_noise_kbd_s,
    &spgramcf_signal_00_s,
    &spgramcf_signal_01_s,
    &spgramcf_signal_02_s,
    &spgramcf_signal_03_s,
    &spgramcf_signal_04_s,
    &spgramcf_signal_05_s,
    &spgramcf_counters_s,
    &spgramcf_config_s,
    &spgramcf_standalone_s,
    &spgramcf_short_s,
    &spgramcf_copy_s,
    &spgramcf_null_s,
    &spgram_gnuplot_s,
    &iirfilt_integrator_s,
    &iirfilt_differentiator_s,
    &iirfilt_dcblock_s,
    &iirfilt_copy_tf_s,
    &iirfilt_copy_sos_s,
    &iirfilt_config_s,
    &dds_cccf_0_s,
    &dds_cccf_1_s,
    &dds_cccf_2_s,
    &dds_config_s,
    &dds_copy_s,
    &firpfb_impulse_response_s,
    &firpfb_crcf_copy_s,
    &resamp2_analysis_s,
    &resamp2_synthesis_s,
    &resamp2_crcf_filter_0_s,
    &resamp2_crcf_filter_1_s,
    &resamp2_crcf_filter_2_s,
    &resamp2_crcf_filter_3_s,
    &resamp2_crcf_filter_4_s,
    &resamp2_crcf_filter_5_s,
    &resamp2_config_s,
    &resamp2_copy_s,
    &firfilt_crcf_kaiser_s,
    &firfilt_crcf_firdespm_s,
    &firfilt_crcf_rect_s,
    &firfilt_crcf_notch_s,
    &firfilt_cccf_notch_s,
    &firfilt_config_s,
    &firfilt_recreate_s,
    &firfilt_push_write_s,
    &symsync_copy_s,
    &symsync_config_s,
    &rresamp_crcf_baseline_P1_Q5_s,
    &rresamp_crcf_baseline_P2_Q5_s,
    &rresamp_crcf_baseline_P3_Q5_s,
    &rresamp_crcf_baseline_P6_Q5_s,
    &rresamp_crcf_baseline_P8_Q5_s,
    &rresamp_crcf_baseline_P9_Q5_s,
    &rresamp_crcf_default_P1_Q5_s,
    &rresamp_crcf_default_P2_Q5_s,
    &rresamp_crcf_default_P3_Q5_s,
    &rresamp_crcf_default_P6_Q5_s,
    &rresamp_crcf_default_P8_Q5_s,
    &rresamp_crcf_default_P9_Q5_s,
    &rresamp_crcf_arkaiser_P3_Q5_s,
    &rresamp_crcf_arkaiser_P5_Q3_s,
    &rresamp_crcf_rrcos_P3_Q5_s,
    &rresamp_crcf_rrcos_P5_Q3_s,
    &rresamp_copy_s,
    &rresamp_config_s,
    &msresamp_crcf_01_s,
    &msresamp_crcf_02_s,
    &msresamp_crcf_03_s,
    &msresamp_crcf_num_output_0_s,
    &msresamp_crcf_num_output_1_s,
    &msresamp_crcf_num_output_2_s,
    &msresamp_crcf_num_output_3_s,
    &msresamp_crcf_num_output_4_s,
    &msresamp_crcf_num_output_5_s,
    &msresamp_crcf_num_output_6_s,
    &msresamp_crcf_num_output_7_s,
    &msresamp_crcf_copy_s,
    &firfilt_cccf_coefficients_test_s,
    &msresamp2_crcf_interp_01_s,
    &msresamp2_crcf_interp_02_s,
    &msresamp2_crcf_interp_03_s,
    &msresamp2_crcf_interp_04_s,
    &msresamp2_crcf_interp_05_s,
    &msresamp2_crcf_interp_06_s,
    &msresamp2_crcf_interp_07_s,
    &msresamp2_crcf_interp_08_s,
    &msresamp2_crcf_interp_09_s,
    &msresamp2_crcf_interp_10_s,
    &msresamp2_crcf_interp_11_s,
    &msresamp2_copy_s,
    &firdecim_config_s,
    &firdecim_block_s,
    &firdecim_copy_s,
    &firfilt_crcf_copy_s,
    &firinterp_rrrf_common_s,
    &firinterp_crcf_common_s,
    &firinterp_rrrf_generic_s,
    &firinterp_crcf_generic_s,
    &firinterp_crcf_rnyquist_0_s,
    &firinterp_crcf_rnyquist_1_s,
    &firinterp_crcf_rnyquist_2_s,
    &firinterp_crcf_rnyquist_3_s,
    &firinterp_copy_s,
    &firinterp_flush_s,
    &iirfilt_rrrf_h3x64_s,
    &iirfilt_rrrf_h5x64_s,
    &iirfilt_rrrf_h7x64_s,
    &iirfilt_crcf_h3x64_s,
    &iirfilt_crcf_h5x64_s,
    &iirfilt_crcf_h7x64_s,
    &iirfilt_cccf_h3x64_s,
    &iirfilt_cccf_h5x64_s,
    &iirfilt_cccf_h7x64_s,
    &iirdes_cplxpair_n6_s,
    &iirdes_cplxpair_n20_s,
    &iirdes_dzpk2sosf_s,
    &iirdes_isstable_n2_yes_s,
    &iirdes_isstable_n2_no_s,
    &ordfilt_copy_s,
    &iirhilbf_interp_decim_s,
    &iirhilbf_filter_s,
    &iirhilbf_config_s,
    &iirhilbf_copy_interp_s,
    &iirhilbf_copy_decim_s,
    &symsync_crcf_scenario_0_s,
    &symsync_crcf_scenario_1_s,
    &symsync_crcf_scenario_2_s,
    &symsync_crcf_scenario_3_s,
    &symsync_crcf_scenario_4_s,
    &symsync_crcf_scenario_5_s,
    &symsync_crcf_scenario_6_s,
    &symsync_crcf_scenario_7_s,
    &iirdes_butter_2_s,
    &iirdes_ellip_lowpass_0_s,
    &iirdes_ellip_lowpass_1_s,
    &iirdes_ellip_lowpass_2_s,
    &iirdes_ellip_lowpass_3_s,
    &iirdes_ellip_lowpass_4_s,
    &iirdes_cheby1_lowpass_0_s,
    &iirdes_cheby1_lowpass_1_s,
    &iirdes_cheby1_lowpass_2_s,
    &iirdes_cheby1_lowpass_3_s,
    &iirdes_cheby1_lowpass_4_s,
    &iirdes_cheby2_lowpass_0_s,
    &iirdes_cheby2_lowpass_1_s,
    &iirdes_cheby2_lowpass_2_s,
    &iirdes_cheby2_lowpass_3_s,
    &iirdes_cheby2_lowpass_4_s,
    &iirdes_butter_lowpass_0_s,
    &iirdes_butter_lowpass_1_s,
    &iirdes_butter_lowpass_2_s,
    &iirdes_butter_lowpass_3_s,
    &iirdes_butter_lowpass_4_s,
    &iirdes_ellip_highpass_s,
    &iirdes_ellip_bandpass_s,
    &iirdes_ellip_bandstop_s,
    &iirdes_bessel_s,
    &liquid_firdes_rcos_s,
    &liquid_firdes_rrcos_s,
    &firdes_rrcos_s,
    &firdes_rkaiser_s,
    &firdes_arkaiser_s,
    &liquid_firdes_dcblock_s,
    &liquid_firdes_notch_s,
    &liquid_getopt_str2firfilt_s,
    &liquid_firdes_config_s,
    &liquid_firdes_estimate_s,
    &firdes_prototype_kaiser_s,
    &firdes_prototype_pm_s,
    &firdes_prototype_rcos_s,
    &firdes_prototype_fexp_s,
    &firdes_prototype_fsech_s,
    &firdes_prototype_farcsech_s,
    &firdes_prototype_arkaiser_s,
    &firdes_prototype_rkaiser_s,
    &firdes_prototype_rrcos_s,
    &firdes_prototype_hm3_s,
    &firdes_prototype_rfexp_s,
    &firdes_prototype_rfsech_s,
    &firdes_prototype_rfarcsech_s,
    &firdes_doppler_s,
    &liquid_freqrespf_s,
    &liquid_freqrespcf_s,
    &firhilbf_decim_s,
    &firhilbf_interp_s,
    &firhilbf_psd_s,
    &firhilbf_config_s,
    &firhilbf_copy_interp_s,
    &firhilbf_copy_decim_s,
    &lpc_p4_s,
    &lpc_p6_s,
    &lpc_p8_s,
    &lpc_p10_s,
    &lpc_p16_s,
    &lpc_p32_s,
    &rresamp_crcf_part_P1_Q5_s,
    &rresamp_crcf_part_P2_Q5_s,
    &rresamp_crcf_part_P3_Q5_s,
    &rresamp_crcf_part_P6_Q5_s,
    &rresamp_crcf_part_P8_Q5_s,
    &rresamp_crcf_part_P9_Q5_s,
    &firdespm_bandpass_n24_s,
    &firdespm_bandpass_n32_s,
    &firdespm_lowpass_s,
    &firdespm_callback_s,
    &firdespm_halfband_m2_ft400_s,
    &firdespm_halfband_m4_ft400_s,
    &firdespm_halfband_m4_ft200_s,
    &firdespm_halfband_m10_ft200_s,
    &firdespm_halfband_m12_ft100_s,
    &firdespm_halfband_m20_ft050_s,
    &firdespm_halfband_m40_ft050_s,
    &firdespm_halfband_m80_ft010_s,
    &firdespm_copy_s,
    &firdespm_config_s,
    &firdespm_differentiator_s,
    &firdespm_hilbert_s,
    &fdelay_rrrf_0_s,
    &fdelay_rrrf_1_s,
    &fdelay_rrrf_2_s,
    &fdelay_rrrf_3_s,
    &fdelay_rrrf_4_s,
    &fdelay_rrrf_5_s,
    &fdelay_rrrf_6_s,
    &fdelay_rrrf_7_s,
    &fdelay_rrrf_8_s,
    &fdelay_rrrf_9_s,
    &fdelay_rrrf_config_s,
    &fdelay_rrrf_push_write_s,
    &fftfilt_rrrf_data_h4x256_s,
    &fftfilt_rrrf_data_h7x256_s,
    &fftfilt_rrrf_data_h13x256_s,
    &fftfilt_rrrf_data_h23x256_s,
    &fftfilt_crcf_data_h4x256_s,
    &fftfilt_crcf_data_h7x256_s,
    &fftfilt_crcf_data_h13x256_s,
    &fftfilt_crcf_data_h23x256_s,
    &fftfilt_cccf_data_h4x256_s,
    &fftfilt_cccf_data_h7x256_s,
    &fftfilt_cccf_data_h13x256_s,
    &fftfilt_cccf_data_h23x256_s,
    &fftfilt_config_s,
    &fftfilt_copy_s,
    &firfilt_cccf_notch_0_s,
    &firfilt_cccf_notch_1_s,
    &firfilt_cccf_notch_2_s,
    &firfilt_cccf_notch_3_s,
    &firfilt_cccf_notch_4_s,
    &firfilt_cccf_notch_5_s,
    &iirinterp_crcf_M2_O9_s,
    &iirinterp_crcf_M3_O9_s,
    &iirinterp_crcf_M4_O9_s,
    &iirinterp_copy_s,
    &liquid_rkaiser_config_s,
    &symsync_rrrf_scenario_0_s,
    &symsync_rrrf_scenario_1_s,
    &symsync_rrrf_scenario_2_s,
    &symsync_rrrf_scenario_3_s,
    &symsync_rrrf_scenario_4_s,
    &symsync_rrrf_scenario_5_s,
    &symsync_rrrf_scenario_6_s,
    &symsync_rrrf_scenario_7_s,
    &firfilt_rrrf_data_h4x8_s,
    &firfilt_rrrf_data_h7x16_s,
    &firfilt_rrrf_data_h13x32_s,
    &firfilt_rrrf_data_h23x64_s,
    &firfilt_crcf_data_h4x8_s,
    &firfilt_crcf_data_h7x16_s,
    &firfilt_crcf_data_h13x32_s,
    &firfilt_crcf_data_h23x64_s,
    &firfilt_cccf_data_h4x8_s,
    &firfilt_cccf_data_h7x16_s,
    &firfilt_cccf_data_h13x32_s,
    &firfilt_cccf_data_h23x64_s,
    &firfilt_rnyquist_baseline_arkaiser_s,
    &firfilt_rnyquist_baseline_rkaiser_s,
    &firfilt_rnyquist_baseline_rrc_s,
    &firfilt_rnyquist_baseline_hm3_s,
    &firfilt_rnyquist_baseline_gmsktxrx_s,
    &firfilt_rnyquist_baseline_rfexp_s,
    &firfilt_rnyquist_baseline_rfsech_s,
    &firfilt_rnyquist_baseline_rfarcsech_s,
    &firfilt_rnyquist_0_s,
    &firfilt_rnyquist_1_s,
    &firfilt_rnyquist_2_s,
    &firfilt_rnyquist_3_s,
    &firfilt_rnyquist_4_s,
    &firfilt_rnyquist_5_s,
    &firfilt_rnyquist_6_s,
    &firfilt_rnyquist_7_s,
    &firfilt_rnyquist_8_s,
    &firfilt_rnyquist_9_s,
    &filter_crosscorr_rrrf_s,
    &iirdecim_copy_s,
    &resamp_crcf_00_s,
    &resamp_crcf_01_s,
    &resamp_crcf_02_s,
    &resamp_crcf_03_s,
    &resamp_crcf_10_s,
    &resamp_crcf_11_s,
    &resamp_crcf_12_s,
    &resamp_crcf_13_s,
    &resamp_crcf_num_output_0_s,
    &resamp_crcf_num_output_1_s,
    &resamp_crcf_num_output_2_s,
    &resamp_crcf_num_output_3_s,
    &resamp_crcf_num_output_4_s,
    &resamp_crcf_num_output_5_s,
    &resamp_crcf_num_output_6_s,
    &resamp_crcf_num_output_7_s,
    &resamp_crcf_copy_s,
    &firdecim_rrrf_common_s,
    &firdecim_crcf_common_s,
    &firdecim_rrrf_data_M2h4x20_s,
    &firdecim_rrrf_data_M3h7x30_s,
    &firdecim_rrrf_data_M4h13x40_s,
    &firdecim_rrrf_data_M5h23x50_s,
    &firdecim_crcf_data_M2h4x20_s,
    &firdecim_crcf_data_M3h7x30_s,
    &firdecim_crcf_data_M4h13x40_s,
    &firdecim_crcf_data_M5h23x50_s,
    &firdecim_cccf_data_M2h4x20_s,
    &firdecim_cccf_data_M3h7x30_s,
    &firdecim_cccf_data_M4h13x40_s,
    &firdecim_cccf_data_M5h23x50_s,
    &iirfiltsos_impulse_n2_s,
    &iirfiltsos_step_n2_s,
    &iirfiltsos_copy_s,
    &iirfiltsos_config_s,
    &fir_groupdelay_n3_s,
    &iir_groupdelay_n3_s,
    &iir_groupdelay_n8_s,
    &iir_groupdelay_sos_n8_s,
    &qdetector_cccf_linear_n64_s,
    &qdetector_cccf_linear_n83_s,
    &qdetector_cccf_linear_n128_s,
    &qdetector_cccf_linear_n167_s,
    &qdetector_cccf_linear_n256_s,
    &qdetector_cccf_linear_n335_s,
    &qdetector_cccf_linear_n512_s,
    &qdetector_cccf_linear_n671_s,
    &qdetector_cccf_linear_n1024_s,
    &qdetector_cccf_linear_n1341_s,
    &qdetector_cccf_gmsk_n64_s,
    &qdetector_cccf_gmsk_n83_s,
    &qdetector_cccf_gmsk_n128_s,
    &qdetector_cccf_gmsk_n167_s,
    &qdetector_cccf_gmsk_n256_s,
    &qdetector_cccf_gmsk_n335_s,
    &qdetector_cccf_gmsk_n512_s,
    &qdetector_cccf_gmsk_n671_s,
    &qdetector_cccf_gmsk_n1024_s,
    &qdetector_cccf_gmsk_n1341_s,
    &qpilotsync_100_16_s,
    &qpilotsync_200_20_s,
    &qpilotsync_300_24_s,
    &qpilotsync_400_28_s,
    &qpilotsync_500_32_s,
    &qpilotgen_config_s,
    &qpilotsync_config_s,
    &symstreamcf_delay_00_s,
    &symstreamcf_delay_01_s,
    &symstreamcf_delay_02_s,
    &symstreamcf_delay_03_s,
    &symstreamcf_delay_04_s,
    &symstreamcf_delay_05_s,
    &symstreamcf_delay_06_s,
    &symstreamcf_delay_07_s,
    &symstreamcf_delay_08_s,
    &symstreamcf_delay_09_s,
    &symstreamcf_delay_10_s,
    &symstreamcf_delay_11_s,
    &symstreamcf_delay_12_s,
    &symstreamcf_delay_13_s,
    &symstreamcf_delay_14_s,
    &symstreamcf_delay_15_s,
    &symstreamcf_delay_16_s,
    &symstreamcf_delay_17_s,
    &symstreamcf_delay_18_s,
    &symstreamcf_delay_19_s,
    &qdetector_cccf_copy_s,
    &detector_cccf_n64_s,
    &detector_cccf_n83_s,
    &detector_cccf_n128_s,
    &detector_cccf_n167_s,
    &detector_cccf_n256_s,
    &detector_cccf_n335_s,
    &detector_cccf_n512_s,
    &detector_cccf_n671_s,
    &detector_cccf_n1024_s,
    &detector_cccf_n1341_s,
    &dsssframesync_s,
    &bpacketsync_s,
    &bsync_rrrf_15_s,
    &bsync_crcf_15_s,
    &bsync_crcf_phase_15_s,
    &qsourcecf_config_s,
    &dsssframe64sync_s,
    &dsssframe64_config_s,
    &dsssframe64gen_copy_s,
    &dsssframe64sync_copy_s,
    &qpacketmodem_bpsk_s,
    &qpacketmodem_qpsk_s,
    &qpacketmodem_psk8_s,
    &qpacketmodem_qam16_s,
    &qpacketmodem_sqam32_s,
    &qpacketmodem_qam64_s,
    &qpacketmodem_sqam128_s,
    &qpacketmodem_qam256_s,
    &qpacketmodem_evm_s,
    &qpacketmodem_unmod_bpsk_s,
    &qpacketmodem_unmod_qpsk_s,
    &qpacketmodem_unmod_psk8_s,
    &qpacketmodem_unmod_qam16_s,
    &qpacketmodem_unmod_sqam32_s,
    &qpacketmodem_unmod_qam64_s,
    &qpacketmodem_unmod_sqam128_s,
    &qpacketmodem_unmod_qam256_s,
    &qpacketmodem_copy_s,
    &symstreamrcf_delay_00_s,
    &symstreamrcf_delay_01_s,
    &symstreamrcf_delay_02_s,
    &symstreamrcf_delay_03_s,
    &symstreamrcf_delay_04_s,
    &symstreamrcf_delay_05_s,
    &symstreamrcf_delay_06_s,
    &symstreamrcf_delay_07_s,
    &symstreamrcf_delay_08_s,
    &symstreamrcf_delay_09_s,
    &symstreamrcf_delay_10_s,
    &symstreamrcf_delay_11_s,
    &symstreamrcf_delay_12_s,
    &symstreamrcf_delay_13_s,
    &symstreamrcf_delay_14_s,
    &symstreamrcf_delay_15_s,
    &symstreamrcf_delay_16_s,
    &symstreamrcf_delay_17_s,
    &symstreamrcf_delay_18_s,
    &symstreamrcf_delay_19_s,
    &qdsync_cccf_k2_s,
    &qdsync_cccf_k3_s,
    &qdsync_cccf_k4_s,
    &qdsync_set_buf_len_s,
    &qdsync_cccf_copy_s,
    &qdsync_cccf_config_s,
    &symtrack_cccf_bpsk_s,
    &symtrack_cccf_qpsk_s,
    &symtrack_cccf_config_invalid_s,
    &symtrack_cccf_config_valid_s,
    &symstreamcf_psd_k2_m12_b030_s,
    &symstreamcf_psd_k4_m12_b030_s,
    &symstreamcf_psd_k4_m25_b020_s,
    &symstreamcf_psd_k7_m11_b035_s,
    &symstreamcf_copy_s,
    &ofdmflexframe_00_s,
    &ofdmflexframe_01_s,
    &ofdmflexframe_02_s,
    &ofdmflexframe_03_s,
    &ofdmflexframe_04_s,
    &ofdmflexframe_05_s,
    &ofdmflexframe_06_s,
    &ofdmflexframe_07_s,
    &ofdmflexframe_08_s,
    &ofdmflexframe_09_s,
    &ofdmflexframegen_config_s,
    &ofdmflexframesync_config_s,
    &fskframesync_s,
    &symstreamrcf_psd_bw200_m12_b030_s,
    &symstreamrcf_psd_bw400_m12_b030_s,
    &symstreamrcf_psd_bw400_m25_b020_s,
    &symstreamrcf_psd_bw700_m11_b035_s,
    &symstreamrcf_copy_s,
    &framesync64_s,
    &framegen64_copy_s,
    &framesync64_copy_s,
    &framesync64_config_s,
    &framesync64_debug_none_s,
    &framesync64_debug_user_s,
    &framesync64_debug_ndet_s,
    &framesync64_debug_head_s,
    &framesync64_debug_rand_s,
    &framesync64_estimation_s,
    &flexframesync_s,
    &gmskframesync_process_s,
    &gmskframesync_multiple_s,
    &gmskframesync_k02_m05_bt20_s,
    &gmskframesync_k02_m05_bt30_s,
    &gmskframesync_k02_m05_bt40_s,
    &gmskframesync_k04_m05_bt20_s,
    &gmskframesync_k04_m05_bt30_s,
    &gmskframesync_k04_m05_bt40_s,
    &gmskframesync_k03_m07_bt20_s,
    &gmskframesync_k08_m20_bt15_s,
    &gmskframesync_k15_m02_bt40_s,
    &msourcecf_tone_s,
    &msourcecf_chirp_s,
    &msourcecf_aggregate_s,
    &msourcecf_config_s,
    &msourcecf_accessor_s,
    &msourcecf_copy_s,
    &eqlms_00_s,
    &eqlms_01_s,
    &eqlms_02_s,
    &eqlms_03_s,
    &eqlms_04_s,
    &eqlms_05_s,
    &eqlms_06_s,
    &eqlms_07_s,
    &eqlms_08_s,
    &eqlms_09_s,
    &eqlms_10_s,
    &eqlms_11_s,
    &eqlms_config_s,
    &eqlms_cccf_copy_s,
    &eqrls_rrrf_01_s,
    &eqrls_rrrf_copy_s,
    &smatrixb_vmul_s,
    &smatrixb_mul_s,
    &smatrixb_mulf_s,
    &smatrixb_vmulf_s,
    &matrixcf_add_s,
    &matrixcf_aug_s,
    &matrixcf_chol_s,
    &matrixcf_inv_s,
    &matrixcf_linsolve_s,
    &matrixcf_ludecomp_crout_s,
    &matrixcf_ludecomp_doolittle_s,
    &matrixcf_mul_s,
    &matrixcf_qrdecomp_s,
    &matrixcf_transmul_s,
    &matrixf_add_s,
    &matrixf_aug_s,
    &matrixf_cgsolve_s,
    &matrixf_chol_s,
    &matrixf_gramschmidt_s,
    &matrixf_inv_s,
    &matrixf_linsolve_s,
    &matrixf_ludecomp_crout_s,
    &matrixf_ludecomp_doolittle_s,
    &matrixf_mul_s,
    &matrixf_qrdecomp_s,
    &matrixf_transmul_s,
    &smatrixf_vmul_s,
    &smatrixf_mul_s,
    &smatrixi_vmul_s,
    &smatrixi_mul_s,
    &window_config_errors_s,
    &windowf_s,
    &window_copy_s,
    &wdelayf_s,
    &wdelay_copy_s,
    &cbufferf_s,
    &cbuffercf_s,
    &cbufferf_flow_s,
    &cbufferf_config_s,
    &cbuffer_copy_s,
    NULL
};

#endif // __LIQUID_AUTOTEST_REGISTRY_H__


#ifndef __LIQUID_AUTOTEST_REGISTRY_H__
#define __LIQUID_AUTOTEST_REGISTRY_H__

#include "liquid.autotest.h"

// ./autotest/null_autotest.c
extern struct liquid_autotest_s null_s;
// ./autotest/libliquid_autotest.c
extern struct liquid_autotest_s libliquid_version_s;
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

// compile test registry
liquid_autotest liquid_autotest_registry[] =
{
    &null_s,
    &libliquid_version_s,
    &agc_crcf_dc_gain_control_s,
    &agc_crcf_scale_s,
    &agc_crcf_ac_gain_control_s,
    &agc_crcf_rssi_sinusoid_s,
    &agc_crcf_rssi_noise_s,
    &agc_crcf_squelch_s,
    &agc_crcf_lock_s,
    &agc_crcf_config_s,
    &agc_crcf_copy_s,
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
    NULL
};

#endif // __LIQUID_AUTOTEST_REGISTRY_H__


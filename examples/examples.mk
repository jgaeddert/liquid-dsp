# 
# Examples
#

local_dir	:= examples

local_src	:=			\
	agc_example.c			\
	agc_qpsk_example.c		\
	agc_squelch_example.c		\
	ampmodem_example.c		\
	asgram_example.c		\
	autocorr_cccf_example.c		\
	bpacketsync_example.c		\
	bsequence_example.c		\
	bufferf_example.c		\
	chromosome_example.c		\
	compand_example.c		\
	compand_cf_example.c		\
	complementary_codes_example.c	\
	crc_example.c			\
	cvsd_example.c			\
	decim_crcf_example.c		\
	decim_rrrf_example.c		\
	dotprod_rrrf_example.c		\
	dotprod_cccf_example.c		\
	eqlms_cccf_example.c		\
	eqrls_cccf_example.c		\
	fec_example.c			\
	fct_example.c			\
	fft_example.c			\
	firfarrow_rrrf_example.c	\
	firfarrow_rrrf_sine_example.c	\
	firfilt_rrrf_example.c		\
	firfilt_rrrf_recreate_example.c	\
	firdes_kaiser_example.c		\
	firdespm_example.c		\
	firhilb_example.c		\
	firhilb_decim_example.c		\
	firhilb_interp_example.c	\
	firpfb_rrrf_example.c		\
	firpfbch_example.c		\
	firpfbch_analysis_example.c	\
	firpfbch_synthesis_example.c	\
	flexframesync_example.c		\
	flexframesync_reconfig_example.c		\
	framesync64_example.c		\
	freqmodem_example.c		\
	ga_search_example.c		\
	ga_search_knapsack_example.c	\
	gmskmodem_example.c		\
	gradient_search_datafit_example.c	\
	gradient_search_example.c	\
	interleaver_example.c		\
	interleaver_scatterplot_example.c	\
	iirdes_example.c		\
	iirdes_analog_example.c		\
	iirdes_pll_example.c		\
	iirfilt_crcf_example.c		\
	interp_crcf_example.c		\
	kbd_window_example.c		\
	lpc_example.c			\
	matched_filter_example.c	\
	matrix_example.c		\
	math_lngamma_example.c		\
	mdct_example.c			\
	modem_arb_example.c		\
	modem_example.c			\
	msequence_example.c		\
	nco_example.c			\
	nco_pll_example.c		\
	nco_pll_modem_example.c		\
	ofdmframe64gen_example.c	\
	ofdmframe64sync_example.c	\
	ofdmframesync_example.c		\
	ofdmoqam_example.c		\
	ofdmoqam_firpfbch_example.c	\
	ofdmoqamframesync_example.c	\
	packetizer_example.c		\
	pll_example.c			\
	polyfit_example.c		\
	polyfit_lagrange_example.c	\
	poly_findroots_example.c	\
	quantize_example.c		\
	quasinewton_search_example.c	\
	random_example.c		\
	random_histogram_example.c	\
	repack_bytes_example.c		\
	resamp_crcf_example.c		\
	resamp2_crcf_example.c		\
	resamp2_crcf_decim_example.c	\
	resamp2_crcf_interp_example.c	\
	resamp2_crcf_interp_recreate_example.c	\
	scramble_example.c		\
	symsync_crcf_example.c		\
	symsync_rrrf_example.c		\
	wdelayf_example.c		\
	windowf_example.c

#	metadata_example.c
#	ofdmframegen_example.c
#	ofdmoqamframe64gen_example.c
#	ofdmoqamframe64sync_example.c
#	gmskframe_example.c
#	fading_generator_example.c
#	ricek_channel_example.c

# experimental examples
# TODO : build when configured with '--enable-experimental' option
local_src_experimental :=		\
	ann_bitpattern_example.c	\
	ann_example.c			\
	ann_layer_example.c		\
	ann_maxnet_example.c		\
	ann_node_example.c		\
	ann_xor_example.c		\
	dds_cccf_example.c		\
	fbasc_example.c			\
	gport_dma_example.c		\
	gport_dma_threaded_example.c	\
	gport_ima_example.c		\
	gport_ima_threaded_example.c	\
	gport_mma_threaded_example.c	\
	iirqmfb_crcf_example.c		\
	itqmfb_crcf_example.c		\
	itqmfb_rrrf_example.c		\
	kmeans_example.c		\
	patternset_example.c		\
	prqmfb_crcf_example.c		\
	qmfb_crcf_analysis_example.c	\
	qmfb_crcf_synthesis_example.c	\
	symsync2_crcf_example.c		\
	symsynclp_crcf_example.c

local_examples	:= $(addprefix $(local_dir)/, $(local_src))

example_src	= $(local_examples)


# 
# Examples
#

local_dir	:= examples
local_src	:=			\
	bit_alloc_test.c		\
	resamp_crcf_example.c		\
	resamp2_crcf_decim_example.c	\
	resamp2_crcf_interp_example.c	\
	resamp2_crcf_interp_recreate_example.c	\
	symsync2_crcf_example.c		\
	symsync_crcf_example.c		\
	decim_rrrf_example.c		\
	fir_farrow_rrrf_example.c	\
	fir_filter_rrrf_example.c	\
	fir_filter_rrrf_recreate_example.c	\
	firdes_kaiser_example.c		\
	firpfb_rrrf_example.c		\
	hilbert_decim_example.c		\
	hilbert_interp_example.c	\
	iir_filter_rrrf_example.c	\
	interp_rrrf_example.c		\
	matched_filter_example.c	\
	symsync_rrrf_example.c		\
					\
	agc_example.c			\
	agc_qpsk_example.c		\
	ann_example.c			\
	asgram_example.c		\
	buffer_example.c		\
	cfbuffer_example.c		\
	checksum_example.c		\
	compand_example.c		\
	compand_cf_example.c		\
	crc_example.c			\
	cvsd_sine_example.c		\
	dotprod_rrrf_example.c		\
	dotprod_cccf_example.c		\
	eqlms_cccf_example.c		\
	eqrls_cccf_example.c		\
	fbuffer_example.c		\
	fwindow_example.c		\
	fading_channel_example.c	\
	fading_generator_example.c	\
	fbasc_example.c			\
	fec_example.c			\
	fft_example.c			\
	firpfbch_example.c		\
	firpfbch_analysis_example.c	\
	firpfbch_synthesis_example.c	\
	flexframesync_example.c		\
	framegen64_example.c		\
	framesync64_example.c		\
	gport2_example.c		\
	gport_example.c			\
	gport_threaded_example.c	\
	gport2_threaded_example.c	\
	interleaver_example.c		\
	itqmfb_crcf_example.c		\
	itqmfb_rrrf_example.c		\
	kmeans_example.c		\
	matrix_example.c		\
	modem_example.c			\
	nco_example.c			\
	ofdmframegen_example.c		\
	ofdmframesync_example.c		\
	ofdmoqam_example.c		\
	packetizer_example.c		\
	pll_example.c			\
	polyfit_example.c		\
	pnsync_crcf_example.c		\
	pnsync_rrrf_example.c		\
	prqmfb_crcf_example.c		\
	qmfb_crcf_analysis_example.c	\
	qmfb_crcf_synthesis_example.c	\
	quantize_example.c		\
	random_example.c		\
	ricek_channel_example.c		\
	scramble_example.c
#	metadata_example.c		\

local_examples	:= $(addprefix $(local_dir)/, $(local_src))

example_src	= $(local_examples)


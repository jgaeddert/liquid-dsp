# 
# Examples
#

local_dir	:= examples
local_src	:=			\
        cresamp2_decim_example.c        \
        cresamp2_interp_example.c       \
        csymsync2_example.c             \
        csymsync_example.c              \
        decim_example.c                 \
        fir_filter_example.c            \
        firdes_kaiser_example.c         \
        firpfb_example.c                \
        hilbert_decim_example.c         \
        hilbert_interp_example.c        \
        iir_filter_example.c            \
        interp_example.c                \
        matched_filter_example.c        \
        symsync_example.c		\
					\
	agc_example.c			\
	asgram_example.c		\
	buffer_example.c		\
	cfbuffer_example.c		\
	checksum_example.c		\
	crc_example.c			\
	dotprod_rrrf_example.c		\
	dotprod_cccf_example.c		\
	fbuffer_example.c		\
	fwindow_example.c		\
	fading_channel_example.c	\
	fading_generator_example.c	\
	fec_hamming74_example.c		\
	fec_hamming84_example.c		\
	fec_rep3_example.c		\
	fft_example.c			\
	gport2_example.c		\
	gport_example.c			\
	gport_threaded_example.c	\
	gport2_threaded_example.c	\
	modem_example.c			\
	ricek_channel_example.c
#	metadata_example.c		\

local_examples	:= $(addprefix $(local_dir)/, $(local_src))

examples	= $(local_examples)


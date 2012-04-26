#
# Makefile for generating liquid documentation figures
#



##
## PGF
## 

# local pgf targets
local_pgffiles :=					\
	figures.pgf/agc_squelch.pdf			\
	figures.pgf/agc_transfer_function.pdf		\
	figures.pgf/bpacket_structure.pdf		\
	figures.pgf/fft_spgram_diagram.pdf		\
	figures.pgf/framing_structure.pdf		\
	figures.pgf/msresamp_decim_diagram.pdf		\
	figures.pgf/msresamp_interp_diagram.pdf		\
	figures.pgf/nco_pll_diagram.pdf			\
	figures.pgf/ofdmflexframe_spectrum.pdf		\
	figures.pgf/ofdmflexframe_structure.pdf		\
	figures.pgf/window.pdf				\

$(local_pgffiles) : %.pdf : %.tex
	$(TEX) -interaction=batchmode -output-directory=figures.pgf $<
	#$(TEX) -output-directory=figures.pgf $<


##
## TARGET PDF FILES (default)
## 

# local targets
local_pdffiles :=					\
	figures.gen/agc_transient.pdf			\
	figures.gen/audio_cvsd.pdf			\
	figures.gen/eqlms_vs_eqrls_const.pdf		\
	figures.gen/eqlms_vs_eqrls_mse.pdf		\
	figures.gen/eqlms_vs_eqrls_freq.pdf		\
	figures.gen/eqlms_vs_eqrls_taps.pdf		\
							\
	figures.gen/eqlms_cccf_blind_const.pdf		\
	figures.gen/eqlms_cccf_blind_mse.pdf		\
	figures.gen/eqlms_cccf_blind_freq.pdf		\
	figures.gen/eqlms_cccf_blind_time.pdf		\
							\
	figures.gen/fec_ber_esn0_block.pdf		\
	figures.gen/fec_ber_ebn0_block.pdf		\
	figures.gen/fec_ber_esn0_conv.pdf		\
	figures.gen/fec_ber_ebn0_conv.pdf		\
	figures.gen/fec_ber_esn0_convpunc.pdf		\
	figures.gen/fec_ber_ebn0_convpunc.pdf		\
	figures.gen/fec_ber_ebn0_hardsoft.pdf			\
							\
	figures.gen/fft_example_time.pdf		\
	figures.gen/fft_example_freq.pdf		\
							\
	figures.gen/fft_spgram_time.pdf			\
	figures.gen/fft_spgram_freq.pdf			\
							\
	figures.gen/filter_rnyquist.pdf			\
	figures.gen/filter_butter_psd.pdf		\
	figures.gen/filter_butter_zpk.pdf		\
							\
	figures.gen/filter_cheby1_psd.pdf		\
	figures.gen/filter_cheby1_zpk.pdf		\
							\
	figures.gen/filter_cheby2_psd.pdf		\
	figures.gen/filter_cheby2_zpk.pdf		\
							\
	figures.gen/filter_ellip_psd.pdf		\
	figures.gen/filter_ellip_zpk.pdf		\
							\
	figures.gen/filter_bessel_psd.pdf		\
	figures.gen/filter_bessel_zpk.pdf		\
							\
	figures.gen/filter_firdespm.pdf			\
							\
	figures.gen/filter_firfarrow_groupdelay.pdf	\
							\
	figures.gen/filter_firfilt_crcf_time.pdf	\
							\
	figures.gen/filter_firhilb_decim_crcf_time.pdf	\
	figures.gen/filter_firhilb_decim_crcf_freq.pdf	\
							\
	figures.gen/filter_iirfilt_crcf_time.pdf	\
							\
	figures.gen/filter_iirdes_example.pdf		\
							\
	figures.gen/filter_decim_crcf.pdf		\
	figures.gen/filter_interp_crcf.pdf		\
	figures.gen/filter_kaiser_time.pdf		\
	figures.gen/filter_kaiser_freq.pdf		\
	figures.gen/filter_firdes_gmskrx_time.pdf	\
	figures.gen/filter_firdes_gmskrx_freq.pdf	\
	figures.gen/filter_msresamp_crcf_time.pdf	\
	figures.gen/filter_msresamp_crcf_freq.pdf	\
	figures.gen/filter_resamp_crcf_time.pdf		\
	figures.gen/filter_resamp_crcf_freq.pdf		\
	figures.gen/filter_resamp2_crcf_interp_time.pdf	\
	figures.gen/filter_resamp2_crcf_interp_freq.pdf	\
							\
	figures.gen/filter_symsync_crcf_const.pdf	\
	figures.gen/filter_symsync_crcf_time.pdf	\
							\
	figures.gen/ofdmflexframesync_performance.pdf	\
							\
	figures.gen/interleaver_scatterplot_i0.pdf	\
	figures.gen/interleaver_scatterplot_i1.pdf	\
	figures.gen/interleaver_scatterplot_i2.pdf	\
	figures.gen/interleaver_scatterplot_i3.pdf	\
	figures.gen/interleaver_scatterplot_i4.pdf	\
							\
	figures.gen/interleaver_ber_ebn0.pdf		\
							\
	figures.gen/math_polyfit_lagrange.pdf		\
	figures.gen/math_window_kaiser_time.pdf		\
	figures.gen/math_window_kaiser_freq.pdf		\
	figures.gen/math_window_hamming_time.pdf	\
	figures.gen/math_window_hamming_freq.pdf	\
	figures.gen/math_window_hann_time.pdf		\
	figures.gen/math_window_hann_freq.pdf		\
	figures.gen/math_window_blackmanharris_time.pdf	\
	figures.gen/math_window_blackmanharris_freq.pdf	\
	figures.gen/math_window_kbd_time.pdf		\
	figures.gen/math_window_kbd_freq.pdf		\
							\
	figures.gen/modem_freqmodem_time.pdf		\
	figures.gen/modem_freqmodem_freq.pdf		\
							\
	figures.gen/modem_ampmodem_time.pdf		\
	figures.gen/modem_ampmodem_freq.pdf		\
							\
	figures.gen/modem_bpsk.pdf			\
	figures.gen/modem_qpsk.pdf			\
	figures.gen/modem_ook.pdf			\
	figures.gen/modem_sqam32.pdf			\
	figures.gen/modem_sqam128.pdf			\
	figures.gen/modem_psk2.pdf			\
	figures.gen/modem_psk4.pdf			\
	figures.gen/modem_psk8.pdf			\
	figures.gen/modem_psk16.pdf			\
	figures.gen/modem_psk32.pdf			\
	figures.gen/modem_psk64.pdf			\
							\
	figures.gen/modem_apsk4.pdf			\
	figures.gen/modem_apsk8.pdf			\
	figures.gen/modem_apsk16.pdf			\
	figures.gen/modem_apsk32.pdf			\
	figures.gen/modem_apsk64.pdf			\
	figures.gen/modem_apsk128.pdf			\
	figures.gen/modem_apsk256.pdf			\
							\
	figures.gen/modem_ask2.pdf			\
	figures.gen/modem_ask4.pdf			\
	figures.gen/modem_ask8.pdf			\
	figures.gen/modem_ask16.pdf			\
							\
	figures.gen/modem_qam8.pdf			\
	figures.gen/modem_qam16.pdf			\
	figures.gen/modem_qam32.pdf			\
	figures.gen/modem_qam64.pdf			\
	figures.gen/modem_qam128.pdf			\
	figures.gen/modem_qam256.pdf			\
							\
	figures.gen/modem_V29.pdf			\
	figures.gen/modem_arb16opt.pdf			\
	figures.gen/modem_arb32opt.pdf			\
	figures.gen/modem_arb64opt.pdf			\
	figures.gen/modem_arb128opt.pdf			\
	figures.gen/modem_arb256opt.pdf			\
	figures.gen/modem_arb64vt.pdf			\
							\
	figures.gen/modem_demodsoft_b0.pdf		\
	figures.gen/modem_demodsoft_b1.pdf		\
	figures.gen/modem_demodsoft_b2.pdf		\
	figures.gen/modem_demodsoft_b3.pdf		\
							\
	figures.gen/modem_ber_ebn0_psk.pdf		\
	figures.gen/modem_ber_ebn0_dpsk.pdf		\
	figures.gen/modem_ber_ebn0_apsk.pdf		\
	figures.gen/modem_ber_ebn0_ask.pdf		\
	figures.gen/modem_ber_ebn0_qam.pdf		\
	figures.gen/modem_ber_ebn0_optqam.pdf		\
	figures.gen/modem_ber_ebn0_arb.pdf		\
							\
	figures.gen/modem_ber_ebn0_M2.pdf		\
	figures.gen/modem_ber_ebn0_M4.pdf		\
	figures.gen/modem_ber_ebn0_M8.pdf		\
	figures.gen/modem_ber_ebn0_M16.pdf		\
	figures.gen/modem_ber_ebn0_M32.pdf		\
	figures.gen/modem_ber_ebn0_M64.pdf		\
	figures.gen/modem_ber_ebn0_M128.pdf		\
	figures.gen/modem_ber_ebn0_M256.pdf		\
							\
	figures.gen/modem_phase_error_qam.pdf		\
	figures.gen/modem_phase_error_M4.pdf		\
	figures.gen/modem_phase_error_M8.pdf		\
	figures.gen/modem_phase_error_M16.pdf		\
	figures.gen/modem_phase_error_M32.pdf		\
	figures.gen/modem_phase_error_M64.pdf		\
	figures.gen/modem_phase_error_M128.pdf		\
	figures.gen/modem_phase_error_M256.pdf		\
							\
	figures.gen/nco_pll_sincos.pdf			\
	figures.gen/nco_pll_error.pdf			\
							\
	figures.gen/optim_gradsearch.pdf		\
	figures.gen/optim_gradsearch_utility.pdf	\
							\
	figures.gen/quantization_adc_b4.pdf		\
	figures.gen/quantization_adc_b5.pdf		\
	figures.gen/quantization_compander.pdf		\
							\
	figures.gen/random_histogram_uniform.pdf	\
	figures.gen/random_histogram_normal.pdf		\
	figures.gen/random_histogram_exp.pdf		\
	figures.gen/random_histogram_weib.pdf		\
	figures.gen/random_histogram_gamma.pdf		\
	figures.gen/random_histogram_nak.pdf		\
	figures.gen/random_histogram_rice.pdf		\
							\
	figures.gen/sequence_ccodes.pdf			\
	figures.gen/sequence_msequence.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

##
## PROGRAMS
##

local_progs :=						\
	src/agc_transient				\
	src/audio_cvsd					\
	src/eqlms_vs_eqrls				\
	src/eqlms_cccf_blind				\
	src/estimate_snr_fec				\
	src/estimate_snr_modem				\
	src/estimate_snr_test				\
	src/fft_example					\
	src/fft_spgram					\
	src/filter_iirdes				\
	src/filter_iirdes_example			\
	src/filter_firdes_gmskrx_time			\
	src/filter_firdes_gmskrx_freq			\
	src/filter_firdespm				\
	src/filter_firfarrow_crcf			\
	src/filter_firfilt_crcf				\
	src/filter_firhilb_decim_crcf			\
	src/filter_iirfilt_crcf				\
	src/filter_decim_crcf				\
	src/filter_interp_crcf				\
	src/filter_msresamp_crcf			\
	src/filter_resamp_crcf				\
	src/filter_resamp2_crcf_interp			\
	src/filter_kaiser				\
	src/filter_rnyquist				\
	src/filter_symsync_crcf				\
	src/interleaver_scatterplot			\
	src/math_polyfit_lagrange			\
	src/math_poly_examples				\
	src/math_window					\
	src/modem_demodsoft				\
	src/modem_freqmodem				\
	src/modem_ampmodem				\
	src/modem.genplot				\
	src/modem.gendata				\
	src/modem_phase_error				\
	src/nco_pll					\
	src/optim_gradsearch				\
	src/quantization_adc				\
	src/quantization_compander			\
	src/random_histogram				\
	src/sequence_ccodes				\
	src/sequence_msequence				\
							\
	src/simulate_ber				\
	src/simulate_per_test				\

$(local_progs) : % : %.c $(lib_objects) libliquid.a

programs : $(local_progs)


##
## MODULE : agc
##

# agc_transient
figures.gen/agc_transient.gnu : src/agc_transient ; ./$<


##
## MODULE : audio
##

# audio_cvsd
figures.gen/audio_cvsd.gnu : src/audio_cvsd ; ./$<


##
## MODULE : buffer
##

##
## MODULE : equalization
##


# eqlms_vs_eqrls
figures.gen/eqlms_vs_eqrls_const.gnu	\
figures.gen/eqlms_vs_eqrls_mse.gnu	\
figures.gen/eqlms_vs_eqrls_freq.gnu	\
figures.gen/eqlms_vs_eqrls_taps.gnu	: src/eqlms_vs_eqrls
	./$< -n512 -c6 -p12 -s40

# eqlms_cccf_blind
figures.gen/eqlms_cccf_blind_const.gnu		\
figures.gen/eqlms_cccf_blind_mse.gnu		\
figures.gen/eqlms_cccf_blind_freq.gnu		\
figures.gen/eqlms_cccf_blind_time.gnu : src/eqlms_cccf_blind
	./$< -n1000 -c8 -p8

##
## MODULE : fec
##

# simulation options
#   -b : minimum BER to reach before stopping
#   -e : minimum number of errors for each step
#   -n : minimum number of bit trials for each step
#   -t : maximum number of bit trials before stopping
#   -s : starting SNR value
#   -x : maximum SNR value before stopping
#   -d : SNR step size
ber-fec-opts      := -b 1e-5 -e 1000 -n4000000 -t400000000 -s-9 -d0.5 -x40

# 
# hard-decision decoding
#
data_ber_fec_hard :=				\
	data/fec-ber/hard/ber_none.dat		\
	data/fec-ber/hard/ber_rep3.dat		\
	data/fec-ber/hard/ber_rep5.dat		\
	data/fec-ber/hard/ber_h128.dat		\
	data/fec-ber/hard/ber_h74.dat		\
	data/fec-ber/hard/ber_h84.dat		\
	data/fec-ber/hard/ber_secded2216.dat	\
	data/fec-ber/hard/ber_secded3932.dat	\
	data/fec-ber/hard/ber_secded7264.dat	\
	data/fec-ber/hard/ber_g2412.dat		\
	data/fec-ber/hard/ber_v27.dat		\
	data/fec-ber/hard/ber_v29.dat		\
	data/fec-ber/hard/ber_v39.dat		\
	data/fec-ber/hard/ber_v615.dat		\
	data/fec-ber/hard/ber_v27p23.dat	\
	data/fec-ber/hard/ber_v27p34.dat	\
	data/fec-ber/hard/ber_v27p45.dat	\
	data/fec-ber/hard/ber_v27p56.dat	\
	data/fec-ber/hard/ber_v27p67.dat	\
	data/fec-ber/hard/ber_v27p78.dat	\
	data/fec-ber/hard/ber_rs8.dat		\

# re-simulate BER data
resimulate-data-fec-ber-hard : src/simulate_ber
	@echo "re-simulating ber data (hard decision decoding)..."
	./src/simulate_ber -H -c none   $(ber-fec-opts) -o data/fec-ber/hard/ber_none.dat
	./src/simulate_ber -H -c rep3   $(ber-fec-opts) -o data/fec-ber/hard/ber_rep3.dat
	./src/simulate_ber -H -c rep5   $(ber-fec-opts) -o data/fec-ber/hard/ber_rep5.dat
	./src/simulate_ber -H -c h128   $(ber-fec-opts) -o data/fec-ber/hard/ber_h128.dat
	./src/simulate_ber -H -c h84    $(ber-fec-opts) -o data/fec-ber/hard/ber_h84.dat
	./src/simulate_ber -H -c h74    $(ber-fec-opts) -o data/fec-ber/hard/ber_h74.dat
	./src/simulate_ber -H -c secded2216 $(ber-fec-opts) -o data/fec-ber/hard/ber_secded2216.dat
	./src/simulate_ber -H -c secded3932 $(ber-fec-opts) -o data/fec-ber/hard/ber_secded3932.dat
	./src/simulate_ber -H -c secded7264 $(ber-fec-opts) -o data/fec-ber/hard/ber_secded7264.dat
	./src/simulate_ber -H -c g2412  $(ber-fec-opts) -o data/fec-ber/hard/ber_g2412.dat
	./src/simulate_ber -H -c v27    $(ber-fec-opts) -o data/fec-ber/hard/ber_v27.dat
	./src/simulate_ber -H -c v29    $(ber-fec-opts) -o data/fec-ber/hard/ber_v29.dat
	./src/simulate_ber -H -c v39    $(ber-fec-opts) -o data/fec-ber/hard/ber_v39.dat
	./src/simulate_ber -H -c v27p23 $(ber-fec-opts) -o data/fec-ber/hard/ber_v27p23.dat
	./src/simulate_ber -H -c v27p34 $(ber-fec-opts) -o data/fec-ber/hard/ber_v27p34.dat
	./src/simulate_ber -H -c v27p45 $(ber-fec-opts) -o data/fec-ber/hard/ber_v27p45.dat
	./src/simulate_ber -H -c v27p56 $(ber-fec-opts) -o data/fec-ber/hard/ber_v27p56.dat
	./src/simulate_ber -H -c v27p67 $(ber-fec-opts) -o data/fec-ber/hard/ber_v27p67.dat
	./src/simulate_ber -H -c v27p78 $(ber-fec-opts) -o data/fec-ber/hard/ber_v27p78.dat
	./src/simulate_ber -H -c v615 -s-9.75 -d0.25 -e200 -n200000 -t4000000 -o data/fec-ber/hard/ber_v615.dat
	./src/simulate_ber -H -c rs8    $(ber-fec-opts) -o data/fec-ber/hard/ber_rs8.dat


# 
# soft-decision decoding
#
data_ber_fec_soft :=				\
	data/fec-ber/soft/ber_none.dat		\
	data/fec-ber/soft/ber_rep3.dat		\
	data/fec-ber/soft/ber_rep5.dat		\
	data/fec-ber/soft/ber_h128.dat		\
	data/fec-ber/soft/ber_h74.dat		\
	data/fec-ber/soft/ber_h84.dat		\
	data/fec-ber/soft/ber_secded2216.dat	\
	data/fec-ber/soft/ber_secded3932.dat	\
	data/fec-ber/soft/ber_secded7264.dat	\
	data/fec-ber/soft/ber_g2412.dat		\
	data/fec-ber/soft/ber_v27.dat		\
	data/fec-ber/soft/ber_v29.dat		\
	data/fec-ber/soft/ber_v39.dat		\
	data/fec-ber/soft/ber_v615.dat		\
	data/fec-ber/soft/ber_v27p23.dat	\
	data/fec-ber/soft/ber_v27p34.dat	\
	data/fec-ber/soft/ber_v27p45.dat	\
	data/fec-ber/soft/ber_v27p56.dat	\
	data/fec-ber/soft/ber_v27p67.dat	\
	data/fec-ber/soft/ber_v27p78.dat	\
	data/fec-ber/soft/ber_rs8.dat		\

# re-simulate BER data
resimulate-data-fec-ber-soft : src/simulate_ber
	@echo "re-simulating ber data (soft decision decoding)..."
	./src/simulate_ber -S -c none   $(ber-fec-opts) -o data/fec-ber/soft/ber_none.dat
	./src/simulate_ber -S -c rep3   $(ber-fec-opts) -o data/fec-ber/soft/ber_rep3.dat
	./src/simulate_ber -S -c rep5   $(ber-fec-opts) -o data/fec-ber/soft/ber_rep5.dat
	./src/simulate_ber -S -c h128   $(ber-fec-opts) -o data/fec-ber/soft/ber_h128.dat
	./src/simulate_ber -S -c h84    $(ber-fec-opts) -o data/fec-ber/soft/ber_h84.dat
	./src/simulate_ber -S -c h74    $(ber-fec-opts) -o data/fec-ber/soft/ber_h74.dat
	./src/simulate_ber -H -c secded2216 $(ber-fec-opts) -o data/fec-ber/soft/ber_secded2216.dat
	./src/simulate_ber -H -c secded3932 $(ber-fec-opts) -o data/fec-ber/soft/ber_secded3932.dat
	./src/simulate_ber -H -c secded7264 $(ber-fec-opts) -o data/fec-ber/soft/ber_secded7264.dat
	./src/simulate_ber -H -c g2412  $(ber-fec-opts) -o data/fec-ber/soft/ber_g2412.dat
	./src/simulate_ber -S -c v27    $(ber-fec-opts) -o data/fec-ber/soft/ber_v27.dat
	./src/simulate_ber -S -c v29    $(ber-fec-opts) -o data/fec-ber/soft/ber_v29.dat
	./src/simulate_ber -S -c v39    $(ber-fec-opts) -o data/fec-ber/soft/ber_v39.dat
	./src/simulate_ber -S -c v27p23 $(ber-fec-opts) -o data/fec-ber/soft/ber_v27p23.dat
	./src/simulate_ber -S -c v27p34 $(ber-fec-opts) -o data/fec-ber/soft/ber_v27p34.dat
	./src/simulate_ber -S -c v27p45 $(ber-fec-opts) -o data/fec-ber/soft/ber_v27p45.dat
	./src/simulate_ber -S -c v27p56 $(ber-fec-opts) -o data/fec-ber/soft/ber_v27p56.dat
	./src/simulate_ber -S -c v27p67 $(ber-fec-opts) -o data/fec-ber/soft/ber_v27p67.dat
	./src/simulate_ber -S -c v27p78 $(ber-fec-opts) -o data/fec-ber/soft/ber_v27p78.dat
	./src/simulate_ber -S -c v615 -s-9.75 -d0.25 -e200 -n200000 -t4000000 -o data/fec-ber/soft/ber_v615.dat
	./src/simulate_ber -S -c rs8    $(ber-fec-opts) -o data/fec-ber/soft/ber_rs8.dat

# copy gnuplot file
figures.gen/fec_ber_esn0_block.gnu \
figures.gen/fec_ber_ebn0_block.gnu \
figures.gen/fec_ber_esn0_conv.gnu \
figures.gen/fec_ber_ebn0_conv.gnu \
figures.gen/fec_ber_esn0_convpunc.gnu \
figures.gen/fec_ber_ebn0_convpunc.gnu \
figures.gen/fec_ber_ebn0_hardsoft.gnu : figures.gen/%.gnu : data/fec-ber/%.gnu
	cp $< $@

# add ber simulation data files as dependencies
figures.gen/fec_ber_esn0_block.eps \
figures.gen/fec_ber_ebn0_block.eps \
figures.gen/fec_ber_esn0_conv.eps \
figures.gen/fec_ber_ebn0_conv.eps \
figures.gen/fec_ber_esn0_convpunc.eps \
figures.gen/fec_ber_ebn0_convpunc.eps \
figures.gen/fec_ber_ebn0_hardsoft.eps : %.eps : %.gnu $(data_ber_fec_hard)

# 
# Required Eb/N0, SNR for 10^-5
#

data/fec-ber/hard/fec_snr_req.dat : src/estimate_snr_fec
	./$< -o$@ -B -H -E1e-5 -n1024 -x4000000

data/fec-ber/soft/fec_snr_req.dat : src/estimate_snr_fec
	./$< -o$@ -B -S -E1e-5 -n1024 -x4000000


##
## MODULE : filter
##

#
# iir filter design
#

# filter design options
fc	:= 0.3
order	:= 7
ripple	:= 1.0
slsl	:= 60.0
nfft	:= 1024
filter_iirdes_opts := -f $(fc) -n $(order) -w $(nfft) -r $(ripple) -s $(slsl)

figures.gen/filter_butter_psd.gnu	\
figures.gen/filter_butter_zpk.gnu	: src/filter_iirdes
	./$< $(filter_iirdes_opts) -t butter

figures.gen/filter_cheby1_psd.gnu	\
figures.gen/filter_cheby1_zpk.gnu	: src/filter_iirdes
	./$< $(filter_iirdes_opts) -t cheby1

figures.gen/filter_cheby2_psd.gnu	\
figures.gen/filter_cheby2_zpk.gnu	: src/filter_iirdes
	./$< $(filter_iirdes_opts) -t cheby2

figures.gen/filter_ellip_psd.gnu	\
figures.gen/filter_ellip_zpk.gnu	: src/filter_iirdes
	./$< $(filter_iirdes_opts) -t ellip

figures.gen/filter_bessel_psd.gnu	\
figures.gen/filter_bessel_zpk.gnu	: src/filter_iirdes
	./$< $(filter_iirdes_opts) -t bessel

# iirdes example
figures.gen/filter_iirdes_example.gnu : src/filter_iirdes_example ; ./$<

#
# firdespm
# 
figures.gen/filter_firdespm.gnu : src/filter_firdespm
	./$<


# 
# firfarrow
#
figures.gen/filter_firfarrow_groupdelay.gnu : src/filter_firfarrow_crcf
	./$<

# 
# firfilt
#
figures.gen/filter_firfilt_crcf_time.gnu : src/filter_firfilt_crcf
	./$<

#
# firhilb decimator
# 
figures.gen/filter_firhilb_decim_crcf_time.gnu \
figures.gen/filter_firhilb_decim_crcf_freq.gnu : src/filter_firhilb_decim_crcf
	./$<

# 
# iirfilt
#
figures.gen/filter_iirfilt_crcf_time.gnu : src/filter_iirfilt_crcf
	./$<

# 
# decimator
#
figures.gen/filter_decim_crcf.gnu : src/filter_decim_crcf
	./$<

# 
# interpolator
#
figures.gen/filter_interp_crcf.gnu : src/filter_interp_crcf
	./$<

# 
# kaiser window filter design
#
figures.gen/filter_kaiser_time.gnu \
figures.gen/filter_kaiser_freq.gnu : src/filter_kaiser
	./$<

# 
# GMSK receive filter
#
figures.gen/filter_firdes_gmskrx_time.gnu : src/filter_firdes_gmskrx_time ; ./$<
figures.gen/filter_firdes_gmskrx_freq.gnu : src/filter_firdes_gmskrx_freq ; ./$<

# 
# msresamp_crcf
#
figures.gen/filter_msresamp_crcf_time.gnu \
figures.gen/filter_msresamp_crcf_freq.gnu: src/filter_msresamp_crcf
	./$<

# 
# resamp_crcf
#
figures.gen/filter_resamp_crcf_time.gnu \
figures.gen/filter_resamp_crcf_freq.gnu: src/filter_resamp_crcf
	./$<

# 
# resamp2_crcf_interp
#
figures.gen/filter_resamp2_crcf_interp_time.gnu \
figures.gen/filter_resamp2_crcf_interp_freq.gnu: src/filter_resamp2_crcf_interp
	./$<

# 
# rnyquist
#
figures.gen/filter_rnyquist.gnu : src/filter_rnyquist
	./$<

# 
# filter_symsync_crcf
#
figures.gen/filter_symsync_crcf_const.gnu \
figures.gen/filter_symsync_crcf_time.gnu : src/filter_symsync_crcf
	./$<


##
## MODULE : fft
##

# 
# fft_example
#
figures.gen/fft_example_time.gnu figures.gen/fft_example_freq.gnu : src/fft_example
	./$<

# 
# fft_spgram
#
figures.gen/fft_spgram_time.dat figures.gen/fft_spgram_freq.dat: src/fft_spgram
	./$<

figures.gen/fft_spgram_time.gnu : figures.gen/% : data/% ; cp $< $@
figures.gen/fft_spgram_freq.gnu : figures.gen/% : data/% ; cp $< $@

figures.gen/fft_spgram_time.eps : %.eps : %.gnu %.dat
figures.gen/fft_spgram_freq.eps : %.eps : %.gnu %.dat

##
## MODULE : framing
##

# 
# ofdmflexframesync_performance
#

figures.gen/ofdmflexframesync_performance.dat \
figures.gen/ofdmflexframesync_performance.gnu : figures.gen/% : data/ofdmflexframe/%
	cp $< $@

figures.gen/ofdmflexframesync_performance.eps : %.eps : %.gnu %.dat

##
## MODULE : interleaver
##

#
# interleaver scatterplot
# 

figures.gen/interleaver_scatterplot_i0.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i0.gnu -n64 -d0

figures.gen/interleaver_scatterplot_i1.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i1.gnu -n64 -d1

figures.gen/interleaver_scatterplot_i2.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i2.gnu -n64 -d2

figures.gen/interleaver_scatterplot_i3.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i3.gnu -n64 -d3

figures.gen/interleaver_scatterplot_i4.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i4.gnu -n64 -d4


#
# interleaver ber
# 

# simulation options
interleaver_ber_opts := -f1024 -ch84 -knone -mqam64 -b1e-5 -e 200 -n320000 -t64000000 -s-9 -d1.0 -x60 -S

# simulation data
resimulate-interleaver-ber-data : src/simulate_ber
	./$<    $(interleaver_ber_opts) -o data/interleaver-ber/interleaver_ber_ebn0_i0.dat
	./$< -i $(interleaver_ber_opts) -o data/interleaver-ber/interleaver_ber_ebn0_i4.dat

# copy gnuplot file
figures.gen/interleaver_ber_ebn0.gnu : figures.gen/% : data/interleaver-ber/%
	cp $< $@

# add .dat files as dependencies
figures.gen/interleaver_ber_ebn0.eps : %.eps : %.gnu data/interleaver-ber/interleaver_ber_ebn0_i0.dat data/interleaver-ber/interleaver_ber_ebn0_i4.dat

##
## MODULE : math
##


#
# window
# 

figures.gen/math_window_kaiser_time.gnu \
figures.gen/math_window_kaiser_freq.gnu : src/math_window
	./$< -w kaiser -b12.0

figures.gen/math_window_hamming_time.gnu \
figures.gen/math_window_hamming_freq.gnu : src/math_window
	./$< -w hamming

figures.gen/math_window_hann_time.gnu \
figures.gen/math_window_hann_freq.gnu : src/math_window
	./$< -w hann

figures.gen/math_window_blackmanharris_time.gnu \
figures.gen/math_window_blackmanharris_freq.gnu : src/math_window
	./$< -w blackmanharris

figures.gen/math_window_kbd_time.gnu \
figures.gen/math_window_kbd_freq.gnu : src/math_window
	./$< -w kbd -b12.0 -n 50

#
# math_polyfit_lagrange
# 
figures.gen/math_polyfit_lagrange.gnu latex.gen/math_polyfit_lagrange.tex : src/math_polyfit_lagrange
	./$<



##
## MODULE : modem
##

# freqmodem
figures.gen/modem_freqmodem_time.gnu figures.gen/modem_freqmodem_freq.gnu : src/modem_freqmodem ; ./$<
figures.gen/modem_freqmodem_time.eps figures.gen/modem_freqmodem_freq.eps : %.eps : %.gnu

# ampmodem
figures.gen/modem_ampmodem_time.gnu figures.gen/modem_ampmodem_freq.gnu : src/modem_ampmodem ; ./$<
figures.gen/modem_ampmodem_time.eps figures.gen/modem_ampmodem_freq.eps : %.eps : %.gnu

# constellation plots: .eps files
modem_constellation_pdffiles :=		\
	figures.gen/modem_psk2.pdf	\
	figures.gen/modem_psk4.pdf	\
	figures.gen/modem_psk8.pdf	\
	figures.gen/modem_psk16.pdf	\
	figures.gen/modem_psk32.pdf	\
	figures.gen/modem_psk64.pdf	\
					\
	figures.gen/modem_apsk4.pdf	\
	figures.gen/modem_apsk8.pdf	\
	figures.gen/modem_apsk16.pdf	\
	figures.gen/modem_apsk32.pdf	\
	figures.gen/modem_apsk64.pdf	\
	figures.gen/modem_apsk128.pdf	\
	figures.gen/modem_apsk256.pdf	\
					\
	figures.gen/modem_ask2.pdf	\
	figures.gen/modem_ask4.pdf	\
	figures.gen/modem_ask8.pdf	\
	figures.gen/modem_ask16.pdf	\
					\
	figures.gen/modem_qam8.pdf	\
	figures.gen/modem_qam16.pdf	\
	figures.gen/modem_qam32.pdf	\
	figures.gen/modem_qam64.pdf	\
	figures.gen/modem_qam128.pdf	\
	figures.gen/modem_qam256.pdf	\
					\
	figures.gen/modem_bpsk.pdf	\
	figures.gen/modem_qpsk.pdf	\
	figures.gen/modem_ook.pdf	\
	figures.gen/modem_sqam32.pdf	\
	figures.gen/modem_sqam128.pdf	\
	figures.gen/modem_V29.pdf	\
	figures.gen/modem_arb16opt.pdf	\
	figures.gen/modem_arb32opt.pdf	\
	figures.gen/modem_arb64opt.pdf	\
	figures.gen/modem_arb128opt.pdf	\
	figures.gen/modem_arb256opt.pdf	\
	figures.gen/modem_arb64vt.pdf	\

modem_constellation_epsfiles := $(patsubst %.pdf,%.eps,$(modem_constellation_pdffiles))
modem_constellation_gnufiles := $(patsubst %.pdf,%.gnu,$(modem_constellation_pdffiles))
modem_constellation_datfiles := $(patsubst %.pdf,%.dat,$(modem_constellation_pdffiles))

# .dat file targets
$(modem_constellation_datfiles) : figures.gen/modem_%.dat : src/modem.gendata
	./$< -f $@ -m$*

# .gnu file targets
$(modem_constellation_gnufiles) : figures.gen/modem_%.gnu : src/modem.genplot
	./$< -f $@ -m$* -t eps -d figures.gen/modem_$*.dat -g $(GNUPLOT_VERSION)

# .eps file targets
$(modem_constellation_epsfiles) : %.eps : %.gnu %.dat

# 
# soft demodulation example
#

modem_demodsoft_gnufiles :=			\
	figures.gen/modem_demodsoft_b0.gnu	\
	figures.gen/modem_demodsoft_b1.gnu	\
	figures.gen/modem_demodsoft_b2.gnu	\
	figures.gen/modem_demodsoft_b3.gnu	\

modem_demodsoft_epsfiles := $(patsubst %.gnu,%.eps,$(modem_demodsoft_gnufiles))

$(modem_demodsoft_gnufiles) figures.gen/modem_demodsoft_labels.dat : src/modem_demodsoft
	./$<

$(modem_demodsoft_epsfiles) : %.eps : %.gnu figures.gen/modem_demodsoft_labels.dat

#
# modem BER data
#

modem_ber_gnufiles :=				\
	figures.gen/modem_ber_ebn0_psk.gnu	\
	figures.gen/modem_ber_ebn0_dpsk.gnu	\
	figures.gen/modem_ber_ebn0_apsk.gnu	\
	figures.gen/modem_ber_ebn0_ask.gnu	\
	figures.gen/modem_ber_ebn0_qam.gnu	\
	figures.gen/modem_ber_ebn0_optqam.gnu	\
	figures.gen/modem_ber_ebn0_arb.gnu	\
	figures.gen/modem_ber_ebn0_M2.gnu	\
	figures.gen/modem_ber_ebn0_M4.gnu	\
	figures.gen/modem_ber_ebn0_M8.gnu	\
	figures.gen/modem_ber_ebn0_M16.gnu	\
	figures.gen/modem_ber_ebn0_M32.gnu	\
	figures.gen/modem_ber_ebn0_M64.gnu	\
	figures.gen/modem_ber_ebn0_M128.gnu	\
	figures.gen/modem_ber_ebn0_M256.gnu	\

modem_ber_epsfiles := $(patsubst %.gnu,%.eps,$(modem_ber_gnufiles))

modem_ber_data :=			\
	data/modem-ber/ber_bpsk.dat	\
	data/modem-ber/ber_qpsk.dat	\
	data/modem-ber/ber_psk8.dat	\
	data/modem-ber/ber_psk16.dat	\
	data/modem-ber/ber_psk32.dat	\
	data/modem-ber/ber_psk64.dat	\
	data/modem-ber/ber_dpsk2.dat	\
	data/modem-ber/ber_dpsk4.dat	\
	data/modem-ber/ber_dpsk8.dat	\
	data/modem-ber/ber_dpsk16.dat	\
	data/modem-ber/ber_dpsk32.dat	\
	data/modem-ber/ber_dpsk64.dat	\
	data/modem-ber/ber_apsk4.dat	\
	data/modem-ber/ber_apsk8.dat	\
	data/modem-ber/ber_apsk16.dat	\
	data/modem-ber/ber_apsk32.dat	\
	data/modem-ber/ber_apsk64.dat	\
	data/modem-ber/ber_apsk128.dat	\
	data/modem-ber/ber_apsk256.dat	\
	data/modem-ber/ber_ask2.dat	\
	data/modem-ber/ber_ask4.dat	\
	data/modem-ber/ber_ask8.dat	\
	data/modem-ber/ber_ask16.dat	\
	data/modem-ber/ber_qam4.dat	\
	data/modem-ber/ber_qam8.dat	\
	data/modem-ber/ber_qam16.dat	\
	data/modem-ber/ber_qam32.dat	\
	data/modem-ber/ber_qam64.dat	\
	data/modem-ber/ber_qam128.dat	\
	data/modem-ber/ber_qam256.dat	\
	data/modem-ber/ber_ook.dat	\
	data/modem-ber/ber_V29.dat	\
	data/modem-ber/ber_sqam32.dat	\
	data/modem-ber/ber_sqam128.dat	\
	data/modem-ber/ber_arb16opt.dat	\
	data/modem-ber/ber_arb32opt.dat	\
	data/modem-ber/ber_arb64opt.dat	\
	data/modem-ber/ber_arb128opt.dat	\
	data/modem-ber/ber_arb256opt.dat	\
	data/modem-ber/ber_arb64vt.dat	\

# re-simulate modem BER data
modem_ber_opts      := -cnone -knone -b1e-5 -e 700 -n320000 -t640000000 -s-9 -d1.0 -x60

$(modem_ber_data) : data/modem-ber/ber_%.dat : src/simulate_ber
	./src/simulate_ber -m $* $(modem_ber_opts) -o $@

# re-generate all data targets
resimulate-modem-ber-data : $(modem_ber_data)

# copy data files
modem_ber_datfiles = $(patsubst data/modem-ber/%,figures.gen/%,$(modem_ber_data))

# copy data, do not give dependencies
$(modem_ber_datfiles) :
	cp data/modem-ber/*.dat figures.gen/

# copy gnuplot file
$(modem_ber_gnufiles) : figures.gen/%.gnu : data/modem-ber/%.gnu
	cp $< $@

# add ber simulation data files as dependencies
$(modem_ber_epsfiles) : %.eps : %.gnu $(modem_ber_datfiles)


# 
# Required Eb/N0, SNR for 10^-5
#

data/modem-ber/modem_snr_req.dat : src/estimate_snr_modem
	./$< -o$@ -B -E1e-5 -n1024 -x8000000

#
# modem phase error plots
#

# phase error figures
modem_phase_error_gnufiles :=			\
	figures.gen/modem_phase_error_qam.gnu	\
	figures.gen/modem_phase_error_M4.gnu	\
	figures.gen/modem_phase_error_M8.gnu	\
	figures.gen/modem_phase_error_M16.gnu	\
	figures.gen/modem_phase_error_M32.gnu	\
	figures.gen/modem_phase_error_M64.gnu	\
	figures.gen/modem_phase_error_M128.gnu	\
	figures.gen/modem_phase_error_M256.gnu	\

modem_phase_error_data :=			\
	data/modem-phase-error/bpsk.dat		\
	data/modem-phase-error/qpsk.dat		\
	data/modem-phase-error/psk8.dat		\
	data/modem-phase-error/psk16.dat	\
	data/modem-phase-error/psk32.dat	\
	data/modem-phase-error/psk64.dat	\
	data/modem-phase-error/apsk4.dat	\
	data/modem-phase-error/apsk8.dat	\
	data/modem-phase-error/apsk16.dat	\
	data/modem-phase-error/apsk32.dat	\
	data/modem-phase-error/apsk64.dat	\
	data/modem-phase-error/apsk128.dat	\
	data/modem-phase-error/apsk256.dat	\
	data/modem-phase-error/ask2.dat		\
	data/modem-phase-error/ask4.dat		\
	data/modem-phase-error/ask8.dat		\
	data/modem-phase-error/ask16.dat	\
	data/modem-phase-error/qam4.dat		\
	data/modem-phase-error/qam8.dat		\
	data/modem-phase-error/qam16.dat	\
	data/modem-phase-error/qam32.dat	\
	data/modem-phase-error/qam64.dat	\
	data/modem-phase-error/qam128.dat	\
	data/modem-phase-error/qam256.dat	\
	data/modem-phase-error/ook.dat		\
	data/modem-phase-error/V29.dat		\
	data/modem-phase-error/sqam32.dat	\
	data/modem-phase-error/sqam128.dat	\
	data/modem-phase-error/arb16opt.dat	\
	data/modem-phase-error/arb32opt.dat	\
	data/modem-phase-error/arb64opt.dat	\
	data/modem-phase-error/arb128opt.dat	\
	data/modem-phase-error/arb256opt.dat	\
	data/modem-phase-error/arb64vt.dat	\


# re-simulate phase error data
modem-phase-error-opts      := -n50 -t100000 -q -s18

#$(modem_phase_error_data) : data/modem-phase-error/%.dat : ./src/modem_phase_error
#	./$< $(modem-phase-error-opts) -m$* -o $@

resimulate-modem-phase-error : $(modem_phase_error_data)

modem_phase_error_epsfiles := $(patsubst %.gnu,%.eps,$(modem_phase_error_gnufiles))

# copy .gnu files to figures.gen directory
$(modem_phase_error_gnufiles) : figures.gen/% : data/modem-phase-error/% ; cp $< $@

# add simulation data files as dependencies
$(modem_phase_error_epsfiles) : %.eps : %.gnu $(modem_phase_error_data)


##
## MODULE : nco
##


# 
# nco_pll
#
figures.gen/nco_pll_sincos.gnu \
figures.gen/nco_pll_error.gnu : src/nco_pll
	./$< -n 400 -p 0.8 -f 0.3 -b 0.01

#figures.gen/nco_pll_sincos.eps : figures.gen/nco_pll_sincos.gnu
#	$(GNUPLOT) $< > $@

#figures.gen/nco_pll_sincos.pdf : figures.gen/nco_pll_sincos.eps
#	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

#figures.gen/nco_pll_error.eps : figures.gen/nco_pll_error.gnu
#	$(GNUPLOT) $< > $@

#figures.gen/nco_pll_error.pdf : figures.gen/nco_pll_error.eps
#	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@


##
## MODULE : optim
##

# 
# optim_gradsearch
#
figures.gen/optim_gradsearch.dat \
figures.gen/optim_gradsearch.gnu \
figures.gen/optim_gradsearch_utility.gnu : src/optim_gradsearch
	./$<


##
## MODULE : quantizer
##

# 
# quantization_adc
#
figures.gen/quantization_adc_b4.gnu : src/quantization_adc ; ./$< -b4 -o$@
figures.gen/quantization_adc_b5.gnu : src/quantization_adc ; ./$< -b5 -o$@

# 
# quantization_compander
#
figures.gen/quantization_compander.gnu : src/quantization_compander ; ./$< -m255 -o$@



##
## MODULE : random
##

# 
# random_histogram
#

random_histogram_gnufiles =				\
	figures.gen/random_histogram_uniform.gnu	\
	figures.gen/random_histogram_normal.gnu		\
	figures.gen/random_histogram_exp.gnu		\
	figures.gen/random_histogram_weib.gnu		\
	figures.gen/random_histogram_gamma.gnu		\
	figures.gen/random_histogram_nak.gnu		\
	figures.gen/random_histogram_rice.gnu		\

$(random_histogram_gnufiles) : figures.gen/random_histogram_%.gnu : src/random_histogram
	./$< -d $* -f $@

##
## MODULE : sequence
##


#
# ccodes
# 

figures.gen/sequence_ccodes.gnu : src/sequence_ccodes
	./$< -n 64 -f $@

#
# msequence
# 
figures.gen/sequence_msequence.gnu : src/sequence_msequence
	./$< -m 6 -f $@

##
## SANDBOX
## 

##
## TARGET COLLECTION
## 

# accumulate targets
figures_generated +=			\
	$(local_pgffiles)		\
	$(local_pdffiles)

# additional auto-generated targets
figures_extra_gen +=

figures_extra_clean +=			\
	$(local_progs)			\
	$(figures_extra_gen)


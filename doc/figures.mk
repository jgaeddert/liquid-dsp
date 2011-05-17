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
	figures.pgf/window.pdf				\
	figures.pgf/framing_structure.pdf		\
	figures.pgf/nco_pll_diagram.pdf

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
	figures.gen/equalizer_example1_const.pdf	\
	figures.gen/equalizer_example1_mse.pdf		\
	figures.gen/equalizer_example1_psd.pdf		\
	figures.gen/equalizer_example1_taps.pdf		\
							\
	figures.gen/equalizer_example2_const.pdf	\
	figures.gen/equalizer_example2_mse.pdf		\
	figures.gen/equalizer_example2_psd.pdf		\
	figures.gen/equalizer_example2_taps.pdf		\
							\
	figures.gen/fec_ber_esn0_hamming.pdf		\
	figures.gen/fec_ber_ebn0_hamming.pdf		\
	figures.gen/fec_ber_esn0_conv.pdf		\
	figures.gen/fec_ber_ebn0_conv.pdf		\
	figures.gen/fec_ber_esn0_convpunc.pdf		\
	figures.gen/fec_ber_ebn0_convpunc.pdf		\
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
	figures.gen/filter_decim_crcf.pdf		\
	figures.gen/filter_interp_crcf.pdf		\
	figures.gen/filter_kaiser_time.pdf		\
	figures.gen/filter_kaiser_freq.pdf		\
	figures.gen/filter_resamp_crcf.pdf		\
	figures.gen/filter_resamp_crcf_psd.pdf		\
	figures.gen/interleaver_scatterplot_i0.pdf	\
	figures.gen/interleaver_scatterplot_i1.pdf	\
	figures.gen/interleaver_scatterplot_i2.pdf	\
	figures.gen/interleaver_scatterplot_i3.pdf	\
	figures.gen/math_window.pdf			\
							\
	figures.gen/modem_bpsk.pdf			\
	figures.gen/modem_qpsk.pdf			\
	figures.gen/modem_ook.pdf			\
	figures.gen/modem_sqam32.pdf			\
	figures.gen/modem_sqam128.pdf			\
	figures.gen/modem_2psk.pdf			\
	figures.gen/modem_4psk.pdf			\
	figures.gen/modem_8psk.pdf			\
	figures.gen/modem_16psk.pdf			\
	figures.gen/modem_32psk.pdf			\
	figures.gen/modem_64psk.pdf			\
							\
	figures.gen/modem_4apsk.pdf			\
	figures.gen/modem_8apsk.pdf			\
	figures.gen/modem_16apsk.pdf			\
	figures.gen/modem_32apsk.pdf			\
	figures.gen/modem_64apsk.pdf			\
	figures.gen/modem_128apsk.pdf			\
	figures.gen/modem_256apsk.pdf			\
							\
	figures.gen/modem_2ask.pdf			\
	figures.gen/modem_4ask.pdf			\
	figures.gen/modem_8ask.pdf			\
	figures.gen/modem_16ask.pdf			\
							\
	figures.gen/modem_8qam.pdf			\
	figures.gen/modem_16qam.pdf			\
	figures.gen/modem_32qam.pdf			\
	figures.gen/modem_64qam.pdf			\
	figures.gen/modem_128qam.pdf			\
	figures.gen/modem_256qam.pdf			\
							\
	figures.gen/modem_V29.pdf			\
	figures.gen/modem_arb16opt.pdf			\
	figures.gen/modem_arb32opt.pdf			\
	figures.gen/modem_arb64vt.pdf			\
							\
	figures.gen/modem_ber_ebn0_psk.pdf		\
	figures.gen/modem_ber_ebn0_apsk.pdf		\
	figures.gen/modem_ber_ebn0_ask.pdf		\
	figures.gen/modem_ber_ebn0_qam.pdf		\
	figures.gen/modem_ber_ebn0_arb.pdf		\
							\
	figures.gen/modem_ber_ebn0_M8.pdf		\
	figures.gen/modem_ber_ebn0_M16.pdf		\
	figures.gen/modem_ber_ebn0_M32.pdf		\
	figures.gen/modem_ber_ebn0_M64.pdf		\
	figures.gen/modem_ber_ebn0_M128.pdf		\
							\
	figures.gen/nco_pll_sincos.pdf			\
	figures.gen/nco_pll_error.pdf			\
							\
	figures.gen/optim_gradsearch.pdf		\
	figures.gen/optim_gradsearch_utility.pdf	\
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
	src/equalizer_cccf				\
	src/filter_iirdes				\
	src/filter_firdespm				\
	src/filter_firfarrow_crcf			\
	src/filter_firfilt_crcf				\
	src/filter_firhilb_decim_crcf			\
	src/filter_iirfilt_crcf				\
	src/filter_decim_crcf				\
	src/filter_interp_crcf				\
	src/filter_resamp_crcf				\
	src/filter_kaiser				\
	src/filter_rnyquist				\
	src/interleaver_scatterplot			\
	src/math_poly_examples				\
	src/math_window					\
	src/modem.genplot				\
	src/modem.gendata				\
	src/nco_pll					\
	src/optim_gradsearch				\
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


# equalizer_example1
figures.gen/equalizer_example1_const.gnu	\
figures.gen/equalizer_example1_mse.gnu		\
figures.gen/equalizer_example1_psd.gnu		\
figures.gen/equalizer_example1_taps.gnu	: src/equalizer_cccf
	./$< -f figures.gen/equalizer_example1 -n512 -c6 -p12 -s40

# equalizer_example2
figures.gen/equalizer_example2_const.gnu	\
figures.gen/equalizer_example2_mse.gnu		\
figures.gen/equalizer_example2_psd.gnu		\
figures.gen/equalizer_example2_taps.gnu	: src/equalizer_cccf
	./$< -f figures.gen/equalizer_example2 -n512 -c8 -p18 -s14

##
## MODULE : fec
##

fec_ber_data :=				\
	data/ber/ber_none.dat		\
	data/ber/ber_r3.dat		\
	data/ber/ber_r5.dat		\
	data/ber/ber_h128.dat		\
	data/ber/ber_h74.dat		\
	data/ber/ber_h84.dat		\
	data/ber/ber_v27.dat		\
	data/ber/ber_v29.dat		\
	data/ber/ber_v39.dat		\
	data/ber/ber_v615.dat		\
	data/ber/ber_v27p23.dat		\
	data/ber/ber_v27p34.dat		\
	data/ber/ber_v27p45.dat		\
	data/ber/ber_v27p56.dat		\
	data/ber/ber_v27p67.dat		\
	data/ber/ber_v27p78.dat		\
#	data/ber/ber_rs8.dat

# re-simulate BER data
fecber_opts      := -b1e-5 -e 500 -n80000 -t200000000 -s-9 -d0.5 -x40
resimulate-ber-data : src/simulate_ber
	@echo "re-simulating ber data..."
	./src/simulate_ber -c none   $(fecber_opts) -o data/ber/ber_none.dat
	./src/simulate_ber -c r3     $(fecber_opts) -o data/ber/ber_r3.dat
	./src/simulate_ber -c r5     $(fecber_opts) -o data/ber/ber_r5.dat
	./src/simulate_ber -c h128   $(fecber_opts) -o data/ber/ber_h128.dat
	./src/simulate_ber -c h84    $(fecber_opts) -o data/ber/ber_h84.dat
	./src/simulate_ber -c h74    $(fecber_opts) -o data/ber/ber_h74.dat
	./src/simulate_ber -c v27    $(fecber_opts) -o data/ber/ber_v27.dat
	./src/simulate_ber -c v29    $(fecber_opts) -o data/ber/ber_v29.dat
	./src/simulate_ber -c v39    $(fecber_opts) -o data/ber/ber_v39.dat
	./src/simulate_ber -c v27p23 $(fecber_opts) -o data/ber/ber_v27p23.dat
	./src/simulate_ber -c v27p34 $(fecber_opts) -o data/ber/ber_v27p34.dat
	./src/simulate_ber -c v27p45 $(fecber_opts) -o data/ber/ber_v27p45.dat
	./src/simulate_ber -c v27p56 $(fecber_opts) -o data/ber/ber_v27p56.dat
	./src/simulate_ber -c v27p67 $(fecber_opts) -o data/ber/ber_v27p67.dat
	./src/simulate_ber -c v27p78 $(fecber_opts) -o data/ber/ber_v27p78.dat
	./src/simulate_ber -c v615 -s-9.75 -d0.25 -e50 -n50000 -t3000000 -o data/ber/ber_v615.dat

#	./src/simulate_ber -c rs8    $(fecber_opts) -o data/ber/ber_rs8.dat

# copy gnuplot file
figures.gen/fec_ber_esn0_hamming.gnu \
figures.gen/fec_ber_ebn0_hamming.gnu \
figures.gen/fec_ber_esn0_conv.gnu \
figures.gen/fec_ber_ebn0_conv.gnu \
figures.gen/fec_ber_esn0_convpunc.gnu \
figures.gen/fec_ber_ebn0_convpunc.gnu : figures.gen/%.gnu : data/%.gnu
	cp $< $@

# add ber simulation data files as dependencies
figures.gen/fec_ber_esn0_hamming.eps \
figures.gen/fec_ber_ebn0_hamming.eps \
figures.gen/fec_ber_esn0_conv.eps \
figures.gen/fec_ber_ebn0_conv.eps \
figures.gen/fec_ber_esn0_convpunc.eps \
figures.gen/fec_ber_ebn0_convpunc.eps : %.eps : %.gnu $(fec_ber_data)



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
# resamp_crcf
#
figures.gen/filter_resamp_crcf.gnu \
figures.gen/filter_resamp_crcf_psd.gnu: src/filter_resamp_crcf
	./$<

# 
# rnyquist
#
figures.gen/filter_rnyquist.gnu : src/filter_rnyquist
	./$<


##
## MODULE : framing
##

##
## MODULE : interleaver
##

#
# interleaver scatterplot
# 

scatterplot_opts := -n 64 -t block

figures.gen/interleaver_scatterplot_i0.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i0.gnu $(scatterplot_opts) -i0

figures.gen/interleaver_scatterplot_i1.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i1.gnu $(scatterplot_opts) -i1

figures.gen/interleaver_scatterplot_i2.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i2.gnu $(scatterplot_opts) -i2

figures.gen/interleaver_scatterplot_i3.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_scatterplot_i3.gnu $(scatterplot_opts) -i3

##
## MODULE : math
##


#
# window
# 
figures.gen/math_window.gnu : src/math_window
	./$<



##
## MODULE : modem
##


# 
# psk
#
figures.gen/modem_2psk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 1
figures.gen/modem_2psk.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m psk -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_4psk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 2
figures.gen/modem_4psk.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m psk -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8psk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 3
figures.gen/modem_8psk.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m psk -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16psk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 4
figures.gen/modem_16psk.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m psk -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_32psk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 5
figures.gen/modem_32psk.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m psk -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_64psk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 6
figures.gen/modem_64psk.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m psk -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

# 
# apsk
#
figures.gen/modem_4apsk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 2
figures.gen/modem_4apsk.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8apsk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 3
figures.gen/modem_8apsk.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16apsk.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 4
figures.gen/modem_16apsk.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_32apsk.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 5
figures.gen/modem_32apsk.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_64apsk.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 6
figures.gen/modem_64apsk.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_128apsk.dat: %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 7
figures.gen/modem_128apsk.gnu: %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 7 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_256apsk.dat: %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 8
figures.gen/modem_256apsk.gnu: %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m apsk -p 8 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

# 
# ask
#
figures.gen/modem_2ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 1
figures.gen/modem_2ask.gnu   : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m ask -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_4ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 2
figures.gen/modem_4ask.gnu   : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m ask -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 3
figures.gen/modem_8ask.gnu   : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m ask -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16ask.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 4
figures.gen/modem_16ask.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m ask -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

# 
# qam
#
figures.gen/modem_8qam.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 3
figures.gen/modem_8qam.gnu   : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m qam -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 4
figures.gen/modem_16qam.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m qam -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_32qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 5
figures.gen/modem_32qam.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m qam -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_64qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 6
figures.gen/modem_64qam.gnu  : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m qam -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_128qam.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 7
figures.gen/modem_128qam.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m qam -p 7 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_256qam.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 8
figures.gen/modem_256qam.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m qam -p 8 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

# 
# arb/specific
#
figures.gen/modem_bpsk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m bpsk -p 1
figures.gen/modem_bpsk.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m bpsk -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_qpsk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m qpsk -p 2
figures.gen/modem_qpsk.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m qpsk -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_ook.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ook -p 1
figures.gen/modem_ook.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m ook -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_sqam32.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m sqam32 -p 5
figures.gen/modem_sqam32.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m sqam32 -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_sqam128.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m sqam128 -p 7
figures.gen/modem_sqam128.gnu   : %.gnu : src/modem.genplot %.dat ; ./$< -f $@ -m sqam128 -p 7 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_V29.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m V29 -p 4
figures.gen/modem_V29.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m V29 -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_arb16opt.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m arb16opt -p 4
figures.gen/modem_arb16opt.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m arb16opt -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_arb32opt.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m arb32opt -p 5
figures.gen/modem_arb32opt.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m arb32opt -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_arb64vt.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m arb64vt -p 6
figures.gen/modem_arb64vt.gnu : %.gnu : src/modem.genplot %.dat;  ./$< -f $@ -m arb64vt -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

#
# modem BER data
#

modem_ber_gnufiles :=				\
	figures.gen/modem_ber_ebn0_psk.gnu	\
	figures.gen/modem_ber_ebn0_apsk.gnu	\
	figures.gen/modem_ber_ebn0_ask.gnu	\
	figures.gen/modem_ber_ebn0_qam.gnu	\
	figures.gen/modem_ber_ebn0_arb.gnu	\
	figures.gen/modem_ber_ebn0_M8.gnu	\
	figures.gen/modem_ber_ebn0_M16.gnu	\
	figures.gen/modem_ber_ebn0_M32.gnu	\
	figures.gen/modem_ber_ebn0_M64.gnu	\
	figures.gen/modem_ber_ebn0_M128.gnu	\

modem_ber_epsfiles := $(patsubst %.gnu,%.eps,$(modem_ber_gnufiles))

modem_ber_data :=			\
	data/ber-modem/ber_bpsk.dat	\
	data/ber-modem/ber_qpsk.dat	\
	data/ber-modem/ber_psk8.dat	\
	data/ber-modem/ber_psk16.dat	\
	data/ber-modem/ber_psk32.dat	\
	data/ber-modem/ber_psk64.dat	\
	data/ber-modem/ber_apsk4.dat	\
	data/ber-modem/ber_apsk8.dat	\
	data/ber-modem/ber_apsk16.dat	\
	data/ber-modem/ber_apsk32.dat	\
	data/ber-modem/ber_apsk64.dat	\
	data/ber-modem/ber_apsk128.dat	\
	data/ber-modem/ber_apsk256.dat	\
	data/ber-modem/ber_ask2.dat	\
	data/ber-modem/ber_ask4.dat	\
	data/ber-modem/ber_ask8.dat	\
	data/ber-modem/ber_ask16.dat	\
	data/ber-modem/ber_qam4.dat	\
	data/ber-modem/ber_qam8.dat	\
	data/ber-modem/ber_qam16.dat	\
	data/ber-modem/ber_qam32.dat	\
	data/ber-modem/ber_qam64.dat	\
	data/ber-modem/ber_qam128.dat	\
	data/ber-modem/ber_qam256.dat	\
	data/ber-modem/ber_V29.dat	\
	data/ber-modem/ber_sqam32.dat	\
	data/ber-modem/ber_sqam128.dat	\
	data/ber-modem/ber_arb16opt.dat	\
	data/ber-modem/ber_arb32opt.dat	\
	data/ber-modem/ber_arb64vt.dat	\

# re-simulate modem BER data
modem_ber_opts      := -cnone -knone -b1e-5 -e 500 -n160000 -t200000000 -s-9 -d1.0 -x60
resimulate-modem-ber-data : src/simulate_ber
	@echo "re-simulating modem ber data..."
	./src/simulate_ber -m bpsk -p 1 $(modem_ber_opts) -o data/ber-modem/ber_bpsk.dat
	./src/simulate_ber -m qpsk -p 2 $(modem_ber_opts) -o data/ber-modem/ber_qpsk.dat
	./src/simulate_ber -m psk  -p 3 $(modem_ber_opts) -o data/ber-modem/ber_psk8.dat
	./src/simulate_ber -m psk  -p 4 $(modem_ber_opts) -o data/ber-modem/ber_psk16.dat
	./src/simulate_ber -m psk  -p 5 $(modem_ber_opts) -o data/ber-modem/ber_psk32.dat
	./src/simulate_ber -m psk  -p 6 $(modem_ber_opts) -o data/ber-modem/ber_psk64.dat
	./src/simulate_ber -m apsk -p 2 $(modem_ber_opts) -o data/ber-modem/ber_apsk4.dat
	./src/simulate_ber -m apsk -p 3 $(modem_ber_opts) -o data/ber-modem/ber_apsk8.dat
	./src/simulate_ber -m apsk -p 4 $(modem_ber_opts) -o data/ber-modem/ber_apsk16.dat
	./src/simulate_ber -m apsk -p 5 $(modem_ber_opts) -o data/ber-modem/ber_apsk32.dat
	./src/simulate_ber -m apsk -p 6 $(modem_ber_opts) -o data/ber-modem/ber_apsk64.dat
	./src/simulate_ber -m apsk -p 7 $(modem_ber_opts) -o data/ber-modem/ber_apsk128.dat
	./src/simulate_ber -m apsk -p 8 $(modem_ber_opts) -o data/ber-modem/ber_apsk256.dat
	./src/simulate_ber -m ask  -p 1 $(modem_ber_opts) -o data/ber-modem/ber_ask2.dat
	./src/simulate_ber -m ask  -p 2 $(modem_ber_opts) -o data/ber-modem/ber_ask4.dat
	./src/simulate_ber -m ask  -p 3 $(modem_ber_opts) -o data/ber-modem/ber_ask8.dat
	./src/simulate_ber -m ask  -p 4 $(modem_ber_opts) -o data/ber-modem/ber_ask16.dat
	./src/simulate_ber -m qam  -p 2 $(modem_ber_opts) -o data/ber-modem/ber_qam4.dat
	./src/simulate_ber -m qam  -p 3 $(modem_ber_opts) -o data/ber-modem/ber_qam8.dat
	./src/simulate_ber -m qam  -p 4 $(modem_ber_opts) -o data/ber-modem/ber_qam16.dat
	./src/simulate_ber -m qam  -p 5 $(modem_ber_opts) -o data/ber-modem/ber_qam32.dat
	./src/simulate_ber -m qam  -p 6 $(modem_ber_opts) -o data/ber-modem/ber_qam64.dat
	./src/simulate_ber -m qam  -p 7 $(modem_ber_opts) -o data/ber-modem/ber_qam128.dat
	./src/simulate_ber -m qam  -p 8 $(modem_ber_opts) -o data/ber-modem/ber_qam256.dat
	./src/simulate_ber -m sqam32   -p 5 $(modem_ber_opts) -o data/ber-modem/ber_sqam32.dat
	./src/simulate_ber -m sqam128  -p 7 $(modem_ber_opts) -o data/ber-modem/ber_sqam128.dat
	./src/simulate_ber -m V29      -p 4 $(modem_ber_opts) -o data/ber-modem/ber_V29.dat
	./src/simulate_ber -m arb16opt -p 4 $(modem_ber_opts) -o data/ber-modem/ber_arb16opt.dat
	./src/simulate_ber -m arb32opt -p 5 $(modem_ber_opts) -o data/ber-modem/ber_arb32opt.dat
	./src/simulate_ber -m arb64vt  -p 6 $(modem_ber_opts) -o data/ber-modem/ber_arb64vt.dat

# copy gnuplot file
$(modem_ber_gnufiles) : figures.gen/%.gnu : data/%.gnu
	cp $< $@

# add ber simulation data files as dependencies
$(modem_ber_epsfiles) : %.eps : %.gnu $(modem_ber_data)



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


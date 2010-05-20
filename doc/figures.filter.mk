#
# Makefile for generating liquid documentation figures
#
# MODULE : filter
#

# local targets
local_pdffiles :=					\
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
	figures.gen/filter_firhilb_decim_crcf_time.pdf	\
	figures.gen/filter_firhilb_decim_crcf_freq.pdf	\
							\
	figures.gen/filter_interp_crcf.pdf		\
	figures.gen/filter_kaiser_time.pdf		\
	figures.gen/filter_kaiser_freq.pdf		\
	figures.gen/filter_resamp_crcf.pdf		\
	figures.gen/filter_resamp_crcf_psd.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## iir filter design
##
src/filter_iirdes : src/filter_iirdes.c $(lib_objects)

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

##
## firdespm
## 
src/filter_firdespm : src/filter_firdespm.c $(lib_objects)
figures.gen/filter_firdespm.gnu : src/filter_firdespm
	./$<


##
## firhilb decimator
## 
src/filter_firhilb_decim_crcf : src/filter_firhilb_decim_crcf.c $(lib_objects)
figures.gen/filter_firhilb_decim_crcf_time.gnu \
figures.gen/filter_firhilb_decim_crcf_freq.gnu : src/filter_firhilb_decim_crcf
	./$<

## 
## interpolator
##
src/filter_interp_crcf : src/filter_interp_crcf.c $(lib_objects)
figures.gen/filter_interp_crcf.gnu : src/filter_interp_crcf
	./$<

## 
## kaiser window filter design
##
src/filter_kaiser : src/filter_kaiser.c $(lib_objects)
figures.gen/filter_kaiser_time.gnu \
figures.gen/filter_kaiser_freq.gnu : src/filter_kaiser
	./$<

## 
## resamp_crcf
##
src/filter_resamp_crcf : src/filter_resamp_crcf.c $(lib_objects)
figures.gen/filter_resamp_crcf.gnu \
figures.gen/filter_resamp_crcf_psd.gnu: src/filter_resamp_crcf
	./$<


##
## target collection
## 

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/filter_firhilb_decim_crcf	\
	src/filter_iirdes		\
	src/filter_interp_crcf		\
	src/filter_kaiser		\
	src/filter_resamp_crcf


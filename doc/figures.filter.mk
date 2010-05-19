#
# Makefile for generating liquid documentation figures
#
# MODULE : filter
#

# local targets
local_pdffiles :=					\
	figures.gen/filter_butter.pdf			\
	figures.gen/filter_cheby1.pdf			\
	figures.gen/filter_cheby2.pdf			\
	figures.gen/filter_ellip.pdf			\
	figures.gen/filter_bessel.pdf			\
	figures.gen/filter_firhilb_decim_crcf_time.pdf	\
	figures.gen/filter_firhilb_decim_crcf_freq.pdf	\
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
fc	:= 0.2
order	:= 7
ripple	:= 1.0
slsl	:= 60.0
nfft	:= 1024
filter_iirdes_opts := -f $(fc) -n $(order) -w $(nfft) -r $(ripple) -s $(slsl)
figures.gen/filter_butter.gnu : src/filter_iirdes ; ./$< -g $@ $(filter_iirdes_opts) -t butter
figures.gen/filter_cheby1.gnu : src/filter_iirdes ; ./$< -g $@ $(filter_iirdes_opts) -t cheby1
figures.gen/filter_cheby2.gnu : src/filter_iirdes ; ./$< -g $@ $(filter_iirdes_opts) -t cheby2
figures.gen/filter_ellip.gnu  : src/filter_iirdes ; ./$< -g $@ $(filter_iirdes_opts) -t ellip
figures.gen/filter_bessel.gnu : src/filter_iirdes ; ./$< -g $@ $(filter_iirdes_opts) -t bessel

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


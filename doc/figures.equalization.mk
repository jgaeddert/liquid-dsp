#
# Makefile for generating liquid documentation figures
#
# MODULE : equalization
#

local_pdffiles :=				\
	figures.gen/equalizer_cccf_const.pdf	\
	figures.gen/equalizer_cccf_mse.pdf		\
	figures.gen/equalizer_cccf_psd.pdf		\
	figures.gen/equalizer_cccf_taps.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## equalizer_cccf
## 
src/equalizer_cccf : src/equalizer_cccf.c $(lib_objects)

figures.gen/equalizer_cccf_const.gnu	\
figures.gen/equalizer_cccf_mse.gnu	\
figures.gen/equalizer_cccf_psd.gnu	\
figures.gen/equalizer_cccf_taps.gnu	: src/equalizer_cccf
	./$<

##
## target collection
## 

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/equalizer_cccf		\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
	$(local_pdffiles)


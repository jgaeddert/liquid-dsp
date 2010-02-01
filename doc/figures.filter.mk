#
# Makefile for generating liquid documentation filter figures
#

local_epsfiles :=				\
	figures.gen/filter_kaiser.eps		\
	figures.gen/filter_resamp_crcf.eps	\
	figures.gen/filter_resamp_crcf_psd.eps	\

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
#local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))
local_pdffiles := $(patsubst %.eps,%.pdf,$(local_epsfiles))

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# gnuplot script generator programs
src/filter_kaiser : src/filter_kaiser.c $(lib_objects)
src/filter_resamp_crcf : src/filter_resamp_crcf.c $(lib_objects)

figures.gen/filter_kaiser.gnu : src/filter_kaiser
	./$<

figures.gen/filter_resamp_crcf.gnu \
figures.gen/filter_resamp_crcf_psd.gnu: src/filter_resamp_crcf
	./$<

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/filter_kaiser		\
	src/filter_resamp_crcf		\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
	$(local_pdffiles)		\
#	$(local_datfiles)


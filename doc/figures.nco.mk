#
# Makefile for generating liquid documentation nco figures
#

local_epsfiles :=					\
	figures.gen/nco_pll_sincos.eps			\
	figures.gen/nco_pll_error.eps

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
#local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))
local_pdffiles := $(patsubst %.eps,%.pdf,$(local_epsfiles))

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

#
# gnuplot script generator programs
# 

# nco_pll
src/nco_pll : % : %.c $(lib_objects)
figures.gen/nco_pll_sincos.gnu \
figures.gen/nco_pll_error.gnu : src/nco_pll
	./$< -n400

#
# target macros
# 

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


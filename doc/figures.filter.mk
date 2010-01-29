#
# Makefile for generating liquid documentation filter figures
#

local_epsfiles :=				\
	figures.gen/filter_kaiser.eps		\
	figures.gen/filter_resamp_crcf.eps

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
#local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))

# gnuplot script generator programs
src/filter_kaiser : src/filter_kaiser.c
src/filter_resamp_crcf : src/filter_resamp_crcf.c

figures.gen/filter_kaiser.gnu : src/filter_kaiser
	./$<

figures.gen/filter_resamp_crcf.gnu : src/filter_resamp_crcf
	./$<

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# accumulate target
figures_generated += $(local_epsfiles)

extra_clean +=				\
	src/filter_kaiser		\
	src/filter_resamp_crcf		\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
#	$(local_datfiles)


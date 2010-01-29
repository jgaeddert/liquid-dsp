#
# Makefile for generating liquid documentation filter figures
#

GNUPLOT_VERSION = 4.2
GNUPLOT = gnuplot

local_epsfiles :=			\
	figures.gen/filter_kaiser.eps

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
#local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))

# gnuplot script generator function
src/filter_kaiser : src/filter_kaiser.c

figures.gen/filter_kaiser.gnu : src/filter_kaiser
	./$<

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# accumulate target
figures_generated += $(local_epsfiles)

extra_clean +=				\
	src/modem.genplot		\
	src/modem.gendata		\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
#	$(local_datfiles)


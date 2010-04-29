#
# Makefile for generating liquid documentation agc figures
#

local_epsfiles :=			\
	figures.gen/agc_squelch.eps

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))
local_pdffiles := $(patsubst %.eps,%.pdf,$(local_epsfiles))

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# agc_squelch
src/agc_squelch : src/agc_squelch.c $(lib_objects)
figures.gen/agc_squelch.gnu  : src/agc_squelch ; ./$<

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/agc_squelch			\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
	$(local_datfiles)


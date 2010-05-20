#
# Makefile for generating liquid documentation figures
#
# MODULE : interleaver
#

# local targets
local_pdffiles :=					\
	figures.gen/interleaver_example1.pdf		\
	figures.gen/interleaver_example2.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## interleaver
## 
src/interleaver_scatterplot : src/interleaver_scatterplot.c $(lib_objects)

# example 1
figures.gen/interleaver_example1.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_example1.gnu -n8 -t sequence

# example 2
figures.gen/interleaver_example2.gnu : src/interleaver_scatterplot
	./$< -f figures.gen/interleaver_example2.gnu -n64 -t sequence

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
	src/interleaver_scatterplot


#
# Makefile for generating liquid documentation figures
#
# MODULE : interleaver
#

# local targets
local_pdffiles :=					\
	figures.gen/interleaver_scatterplot_i0.pdf	\
	figures.gen/interleaver_scatterplot_i1.pdf	\
	figures.gen/interleaver_scatterplot_i2.pdf	\
	figures.gen/interleaver_scatterplot_i3.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## interleaver scatterplot
## 
src/interleaver_scatterplot : src/interleaver_scatterplot.c $(lib_objects)

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

figures_extra_clean +=			\
	src/interleaver_scatterplot


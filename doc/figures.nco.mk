#
# Makefile for generating liquid documentation figures
#
# MODULE : nco
#

local_pdffiles :=					\
	figures.gen/nco_pll_sincos.pdf			\
	figures.gen/nco_pll_error.pdf			\
	figures.pgf/nco_pll_diagram.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

## 
## nco_pll
##
src/nco_pll : % : %.c $(lib_objects)
figures.gen/nco_pll_sincos.gnu \
figures.gen/nco_pll_error.gnu : src/nco_pll
	./$< -n 400 -p 0.8 -f 0.3 -b 0.01

figures.gen/nco_pll_error.eps : figures.gen/nco_pll_error.gnu
	$(GNUPLOT) $< > $@

figures.gen/nco_pll_error.pdf : figures.gen/nco_pll_error.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

## 
## nco_pll_diagram
##
figures.pgf/nco_pll_diagram.pdf : figures.pgf/nco_pll_diagram.tex
	$(TEX) -interaction=batchmode -output-directory=figures.pgf $<
##
## target collection
## 

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/nco_pll


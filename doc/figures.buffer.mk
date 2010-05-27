#
# Makefile for generating liquid documentation figures
#
# MODULE : buffer
#

# local targets
local_pdffiles :=				\
	figures.pgf/window.pdf

##
## window (PGF)
## 
figures.pgf/window.pdf : figures.pgf/window.tex
	$(TEX) -interaction=batchmode -output-directory=figures.pgf $<

##
## target collection
## 

# accumulate target
figures_generated += $(local_pdffiles)

figures_extra_clean +=



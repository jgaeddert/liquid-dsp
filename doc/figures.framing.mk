#
# Makefile for generating liquid documentation figures
#
# MODULE : framing
#

# local targets
local_pdffiles :=				\
	figures.pgf/framing_structure.pdf	\

##
## framing_structure (PGF)
## 
figures.pgf/framing_structure.pdf : figures.pgf/framing_structure.tex
	$(TEX) -interaction=batchmode -output-directory=figures.pgf $<


##
## target collection
## 

# accumulate target
figures_generated += $(local_pdffiles)

figures_extra_clean +=


#
# Makefile for generating liquid documentation agc figures
#

local_pdffiles :=			\
	figures.pgf/agc_squelch.pdf

##
## agc_squelch (PGF)
## 
src/agc_squelch : src/agc_squelch.c $(lib_objects)

figures.pgf/agc_squelch.dat : src/agc_squelch
	./$<

figures.pgf/agc_squelch.pdf : figures.pgf/agc_squelch.tex figures.pgf/agc_squelch.dat
	$(TEX) --output-directory=figures.pgf $<


# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/agc_squelch			\
	figures.pgf/agc_squelch.aux	\
	figures.pgf/agc_squelch.dat	\
	figures.pgf/agc_squelch.log	\
	$(local_pdffiles)


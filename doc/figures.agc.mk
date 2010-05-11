#
# Makefile for generating liquid documentation agc figures
#

local_pdffiles :=			\
	figures.gen/agc_squelch_pgf.pdf

##
## agc_squelch (PGF)
## 
src/agc_squelch_pgf : src/agc_squelch_pgf.c $(lib_objects)

figures.gen/agc_squelch_pgf.dat : src/agc_squelch_pgf
	./$<

figures.gen/agc_squelch_pgf.pdf : agc_squelch_pgf.tex figures.gen/agc_squelch_pgf.dat
	$(TEX) agc_squelch_pgf.tex
	$(MV) agc_squelch_pgf.pdf figures.gen/


# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/agc_squelch_pgf		\
	agc_squelch_pgf.aux		\
	agc_squelch_pgf.log		\
	figures.gen/agc_squelch_pgf.dat	\
	$(local_pdffiles)


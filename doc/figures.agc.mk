#
# Makefile for generating liquid documentation agc figures
#

local_pdffiles :=				\
	figures.pgf/agc_squelch.pdf		\
	figures.pgf/agc_transfer_function.pdf	\
	figures.gen/agc_transient.pdf

##
## agc_squelch (PGF)
## 
src/agc_squelch : src/agc_squelch.c $(lib_objects)

figures.pgf/agc_squelch.dat : src/agc_squelch
	./$<

figures.pgf/agc_squelch.pdf : figures.pgf/agc_squelch.tex figures.pgf/agc_squelch.dat
	$(TEX) --output-directory=figures.pgf $<


##
## agc_transfer_function (PGF)
## 
figures.pgf/agc_transfer_function.pdf : figures.pgf/agc_transfer_function.tex
	$(TEX) --output-directory=figures.pgf $<

##
## agc_transient
## 
src/agc_transient : src/agc_transient.c $(lib_objects)

figures.gen/agc_transient.gnu : src/agc_transient ; ./$<

figures.gen/agc_transient.eps : figures.gen/agc_transient.gnu
	$(GNUPLOT) $< > $@

figures.gen/agc_transient.pdf : figures.gen/agc_transient.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

##
##
## 

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/agc_squelch			\
	figures.pgf/agc_*.aux		\
	figures.pgf/agc_*.dat		\
	figures.pgf/agc_*.log		\
	figures.gen/agc_*.pdf		\
	figures.gen/agc_*.eps		\
	figures.gen/agc_*.gnu		\
	$(local_pdffiles)


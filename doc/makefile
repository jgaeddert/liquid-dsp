#
# Makefile for generating liquid documentation
#
# Includes targets for listings, figures, etc.
#

projName = liquid
TEX = pdflatex
TEX_FLAGS = --shell-escape
BIB = bibtex
BIB_FLAGS =
CC = gcc
CFLAGS :=
LDFLAGS = -lliquid
RM = rm -f

EPSTOPDF = epstopdf
EPSTOPDF_FLAGS = --compress

GNUPLOT = gnuplot
GNUPLOT_VERSION = 4.2

PYGMENTIZE = pygmentize
PYGMENTS_VERB_OPTS = -P verboptions='fontsize=\small'
PYGMENTS_OPTS = -O linenos=True $(PYGMENTS_VERB_OPTS)
PYGMENTS_STYLE = default

include_dirs	:= . include
INCLUDE_CFLAGS	= $(addprefix -I ,$(include_dirs))
CFLAGS		+= $(INCLUDE_CFLAGS) -g -O2 -Wall

# Listings used in this document
LISTINGS = 					\
	listings/agc.example.c.tex		\
	listings/nco.c++.tex			\
	listings/firhilb.example.c.tex		\
	listings/gradient_search.example.c.tex	\
	listings/gport.direct.example.c.tex	\
	listings/gport.indirect.example.c.tex	\
	listings/interp.example.c.tex		\
	listings/matrix.example.c.tex

# .tex inputs
SECTIONS :=					\
	sections/agc.tex			\
	sections/ann.tex			\
	sections/audio.tex			\
	sections/buffer.tex			\
	sections/channel.tex			\
	sections/dotprod.tex			\
	sections/equalization.tex		\
	sections/fec.tex			\
	sections/fft.tex			\
	sections/filter.tex			\
	sections/framing.tex			\
	sections/interleaver.tex		\
	sections/math.tex			\
	sections/matrix.tex			\
	sections/modem.tex			\
	sections/multicarrier.tex		\
	sections/nco.tex			\
	sections/optim.tex			\
	sections/quantization.tex		\
	sections/random.tex			\
	sections/sequence.tex			\
	sections/utility.tex

lib_sources	:=				\
	lib/liquid_doc_compute_psd.c

# additional targets to clean
extra_clean :=

all: $(projName).pdf


##
## LISTINGS
##

# generate listings style
highlight.sty:
	$(PYGMENTIZE) -f latex -S $(PYGMENTS_STYLE) > highlight.sty

# use pygments (http://pygments.org) to generate fancy syntax-highlighted source
$(LISTINGS) : %.tex : %
	$(PYGMENTIZE) -f latex $(PYGMENTS_OPTS) -o $@ $<

# full listings target
listings: highlight.sty $(LISTINGS)

clean-listings:
	$(RM) highlight.sty
	$(RM) $(LISTINGS)

##
## LIB
##

lib_objects	= $(addsuffix .o,$(lib_sources))

$(lib_objects) : %.o : %
	$(CC) $(CFLAGS) -c $< -o $@

lib: $(lib_objects)

clean-lib:
	$(RM) $(lib_objects)

##
## FIGURES
##

# auto-generated figures, target collection variables
figures_generated :=

# modem figures
include figures.filter.mk
include figures.modem.mk

# full figures target
figures: $(figures_generated)

# clean figures target
clean-figures:
	$(RM) figures.gen/*


##
## DOCUMENT
##

# auxillary
$(projName).aux: $(projName).tex listings highlight.sty $(SECTIONS)
	$(TEX) $(TEX_FLAGS) $(projName)

# bibliography
$(projName).bbl: $(projName).aux $(projName).bib
	$(BIB) $(BIB_FLAGS) $(projName)

# main project file (.pdf)
#$(projName).pdf: $(projName).bbl $(projName).tex listings
# TODO use above line for bibliography (ignore for now)
$(projName).pdf: $(projName).tex listings $(SECTIONS) $(figures_generated)
	$(TEX) $(TEX_FLAGS) $(projName)
	$(TEX) $(TEX_FLAGS) $(projName)

clean-doc:
	$(RM) *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.bbl *.blg $(projName).pdf


##
## OTHER
##

clean: clean-doc

distclean: clean clean-figures clean-listings
	$(RM) $(extra_clean)


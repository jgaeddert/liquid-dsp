#
# Makefile for generating liquid documentation
#
# Includes targets for listings, figures, etc.
#

TEX = pdflatex
TEX_FLAGS = --shell-escape
BIB = bibtex
BIB_FLAGS =
CC = gcc
CFLAGS :=
LDFLAGS = -lliquid
RM = rm -f
MV = mv

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
	listings/dotprod_rrrf.example.c.tex	\
	listings/firhilb.example.c.tex		\
	listings/gradient_search.example.c.tex	\
	listings/gport.direct.example.c.tex	\
	listings/gport.indirect.example.c.tex	\
	listings/interp.example.c.tex		\
	listings/matrix.example.c.tex		\
	listings/nco.c++.tex			\
	listings/packetizer.example.c.tex

# .tex inputs
SECTIONS :=					\
	sections/introduction.tex		\
	sections/installation.tex		\
						\
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
	lib/liquid_doc_compute_psd.c		\
	lib/liquid_doc_freqz.c

# additional targets to clean
extra_clean :=

all: liquid.pdf


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

# module figure makefiles
include figures.agc.mk
include figures.filter.mk
include figures.modem.mk
include figures.nco.mk

# full figures target
figures: $(figures_generated)

# clean figures target (all supplemental files)
clean-figures:
	$(RM) figures.gen/*.eps
	$(RM) figures.gen/*.dat
	$(RM) figures.gen/*.gnu
	$(RM) figures.gen/*.pdf
	$(RM) figures.pgf/*.aux
	$(RM) figures.pgf/*.dat
	$(RM) figures.pgf/*.log
	$(RM) figures.pgf/*.pdf

##
## DOCUMENT
##

# auxillary
liquid.aux liquid.bib.aux : liquid.tex listings highlight.sty $(SECTIONS) $(figures_generated)
	$(TEX) $(TEX_FLAGS) liquid
	touch liquid.bib.aux

# bibliography
liquid.bbl : liquid.bib liquid.bib.aux
	$(BIB) $(BIB_FLAGS) liquid

# main project file (.pdf)
liquid.pdf: liquid.tex liquid.bbl listings $(SECTIONS)
	$(TEX) $(TEX_FLAGS) liquid
	$(TEX) $(TEX_FLAGS) liquid

clean-doc:
	$(RM) *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.bbl *.blg
	$(RM) liquid.pdf


##
## OTHER
##

clean: clean-doc

distclean: clean clean-figures clean-listings
	$(RM) $(extra_clean)


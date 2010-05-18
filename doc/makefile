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
	listings/checksum.example.c.tex		\
	listings/crc32.example.c.tex		\
	listings/dotprod_rrrf.example.c.tex	\
	listings/eqlms_cccf.example.c.tex	\
	listings/firhilb.example.c.tex		\
	listings/gradient_search.example.c.tex	\
	listings/gport.direct.example.c.tex	\
	listings/gport.indirect.example.c.tex	\
	listings/interp.example.c.tex		\
	listings/matrix.example.c.tex		\
	listings/nco.c++.tex			\
	listings/packetizer.example.c.tex	\
	listings/resamp_crcf.example.c.tex

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
	lib/liquid_doc_freqz.c			\
	lib/htmlgen.envparser.c			\
	lib/htmlgen.tokenparser.c		\
	lib/htmlgen.c

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

$(lib_objects) : %.o : % include/liquid.doc.h include/liquid.doc.html.h
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
include figures.equalization.mk
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
## PDF DOCUMENT
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
## HTML DOCUMENTATION
##

# html directories
html/fig/.touch: ; mkdir -p html/fig && touch html/fig/.touch
html/eqn/.touch: ; mkdir -p html/eqn && touch html/eqn/.touch
html-dirs := html/fig/.touch html/eqn/.touch

# html figures
html_figures = $(patsubst %.pdf,%.png,$(figures_generated))
$(html_figures) : %.png : %.pdf
	convert $< $@
	cp $@ html/fig/

# html generator program
src/doc-html-gen : src/doc-html-gen.c $(lib_objects)

# html index file, equations makefile
html/index.html	\
html-equations.mk : src/doc-html-gen liquid.tex $(html-dirs)
	./$< -i scripts/htmlgen_test.tex
	#./$< -i liquid.tex

# include targets for equations
.PRECIOUS: html-equations.mk
-include html-equations.mk
html_eqn_pngfiles = $(patsubst %.tex,%.png,$(html_eqn_texfiles))
html_eqn_dvifiles = $(patsubst %.tex,%.dvi,$(html_eqn_texfiles))

$(html_eqn_dvifiles) : %.dvi : %.tex html-equations.mk
	latex -interaction=batchmode -output-format=dvi -output-directory=html/eqn $<
	$(RM) $*.log
	$(RM) $*.aux

$(html_eqn_pngfiles) : %.png : %.dvi html-equations.mk
	dvipng -q -T tight -x1200 -z 9 -bg Transparent -o $@ $*.dvi

# html equations target
html-equations : $(html_eqn_pngfiles)

# html main target TODO : add $(html_figures) as dependency
html: html/index.html html-equations

# clean html files
clean-html:
	$(RM) src/doc-html-gen
	$(RM) -r html
	$(RM) figures-html.mk
	#$(RM) $(html_figures)

##
## OTHER
##

clean: clean-doc

distclean: clean clean-figures clean-listings
	$(RM) $(extra_clean)


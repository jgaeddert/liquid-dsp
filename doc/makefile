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

# Listings used in this document
LISTINGS = 					\
	listings/nco.c++.tex			\
	listings/gradient_search.example.c.tex	\
	listings/agc.example.c.tex

SECTIONS :=					\
	sections/agc.tex			\
	sections/ann.tex			\
	sections/audio.tex			\
	sections/buffer.tex			\
	sections/filter.tex			\
	sections/multicarrier.tex		\
	sections/optim.tex

PYGMENTS_VERB_OPTS = -P verboptions='fontsize=\small'
PYGMENTS_OPTS = -O linenos=True $(PYGMENTS_VERB_OPTS)
PYGMENTS_STYLE = default

all: $(projName).pdf

# generate listings style
highlight.sty:
	pygmentize -f latex -S $(PYGMENTS_STYLE) > highlight.sty

# use pygments (http://pygments.org) to generate fancy syntax-highlighted source
$(LISTINGS) : %.tex : %
	pygmentize -f latex $(PYGMENTS_OPTS) -o $@ $<

# full listings target
listings: highlight.sty $(LISTINGS)

# auxillary
$(projName).aux: $(projName).tex listings highlight.sty $(SECTIONS)
	$(TEX) $(TEX_FLAGS) $(projName)

# bibliography
$(projName).bbl: $(projName).aux $(projName).bib
	$(BIB) $(BIB_FLAGS) $(projName)

# main project file (.pdf)
# TODO list figures as dependencies
#$(projName).pdf: $(projName).bbl $(projName).tex listings
# TODO use above line for bibliography (ignore for now)
$(projName).pdf: $(projName).tex listings $(SECTIONS)
	$(TEX) $(TEX_FLAGS) $(projName)
	$(TEX) $(TEX_FLAGS) $(projName)

fig-clean:
	#rm -f figures/*.pdf

listings-clean:
	rm -f highlight.sty
	rm -f $(LISTINGS)

doc-clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.bbl *.blg $(projName).pdf

clean: doc-clean

distclean: doc-clean listings-clean fig-clean


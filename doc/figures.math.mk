#
# Makefile for generating liquid documentation figures
#
# MODULE : math
#

# local targets
local_pdffiles :=					\
	figures.gen/math_window.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## window
## 
src/math_window : src/math_window.c $(lib_objects)
figures.gen/math_window.gnu : src/math_window
	./$<


##
## target collection
## 

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# accumulate target
figures_generated += $(local_pdffiles)

figures_extra_clean +=			\
	src/math_window


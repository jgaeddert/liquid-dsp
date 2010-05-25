#
# Makefile for generating liquid documentation figures
#
# MODULE : nco
#

local_pdffiles :=					\
	figures.gen/nco_pll_sincos.pdf			\
	figures.gen/nco_pll_error.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

## 
## nco_pll
##
src/nco_pll : % : %.c $(lib_objects)
figures.gen/nco_pll_sincos.gnu \
figures.gen/nco_pll_error.gnu : src/nco_pll
	./$< -n 400 -p 0.8 -f 0.3 -b 0.01

##
## target collection
## 

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# pdf target files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# accumulate target
figures_generated += $(local_pdffiles)

extra_clean +=				\
	src/nco_pll


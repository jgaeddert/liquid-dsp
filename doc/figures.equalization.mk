#
# Makefile for generating liquid documentation figures
#
# MODULE : equalization
#

# local targets
local_pdffiles :=					\
	figures.gen/equalizer_example1_const.pdf	\
	figures.gen/equalizer_example1_mse.pdf		\
	figures.gen/equalizer_example1_psd.pdf		\
	figures.gen/equalizer_example1_taps.pdf		\
							\
	figures.gen/equalizer_example2_const.pdf	\
	figures.gen/equalizer_example2_mse.pdf		\
	figures.gen/equalizer_example2_psd.pdf		\
	figures.gen/equalizer_example2_taps.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## program(s)
## 
src/equalizer_cccf : src/equalizer_cccf.c $(lib_objects)

##
## equalizer_example1
## 

figures.gen/equalizer_example1_const.gnu	\
figures.gen/equalizer_example1_mse.gnu		\
figures.gen/equalizer_example1_psd.gnu		\
figures.gen/equalizer_example1_taps.gnu	: src/equalizer_cccf
	./$< -f figures.gen/equalizer_example1 -n512 -c6 -p12 -s40

##
## equalizer_example2
## 

figures.gen/equalizer_example2_const.gnu	\
figures.gen/equalizer_example2_mse.gnu		\
figures.gen/equalizer_example2_psd.gnu		\
figures.gen/equalizer_example2_taps.gnu	: src/equalizer_cccf
	./$< -f figures.gen/equalizer_example2 -n512 -c12 -p28 -s10

##
## target collection
## 

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# eps target files
$(local_epsfiles) : %.eps : %.gnu
	$(GNUPLOT) $< > $@

# accumulate target
figures_generated += $(local_pdffiles)

figures_extra_clean +=			\
	src/equalizer_cccf


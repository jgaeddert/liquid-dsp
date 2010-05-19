#
# Makefile for generating liquid documentation figures
#
# MODULE : audio
#

# local targets
local_pdffiles :=					\
	figures.gen/audio_cvsd.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## audio_cvsd
## 

src/audio_cvsd : src/audio_cvsd.c $(lib_objects)

figures.gen/audio_cvsd.gnu : src/audio_cvsd ; ./$<

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

extra_clean +=				\
	src/audio_cvsd


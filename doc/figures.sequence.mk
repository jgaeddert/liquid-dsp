#
# Makefile for generating liquid documentation figures
#
# MODULE : sequence
#

# local targets
local_pdffiles :=					\
	figures.gen/sequence_ccodes.pdf			\
	figures.gen/sequence_msequence.pdf

local_gnufiles := $(patsubst %.pdf,%.gnu,$(local_pdffiles))
local_epsfiles := $(patsubst %.pdf,%.eps,$(local_pdffiles))

##
## ccodes
## 
src/sequence_ccodes : % : %.c $(lib_objects)

figures.gen/sequence_ccodes.gnu : src/sequence_ccodes
	./$< -n 64 -f $@


##
## msequence
## 
src/sequence_msequence : % : %.c $(lib_objects)

figures.gen/sequence_msequence.gnu : src/sequence_msequence
	./$< -m 6 -f $@

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
	src/sequence_ccodes		\
	src/sequence_msequence


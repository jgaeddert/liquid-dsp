#
# Makefile for generating liquid documentation modem figures
#

# local targets
local_epsfiles :=			\
	figures.gen/modem_bpsk.eps	\
	figures.gen/modem_qpsk.eps	\
	figures.gen/modem_8psk.eps	\
	figures.gen/modem_16psk.eps	\
					\
	figures.gen/modem_4apsk.eps	\
	figures.gen/modem_8apsk.eps	\
	figures.gen/modem_16apsk.eps	\
	figures.gen/modem_32apsk.eps	\
	figures.gen/modem_64apsk.eps	\
	figures.gen/modem_128apsk.eps	\
					\
	figures.gen/modem_2ask.eps	\
	figures.gen/modem_4ask.eps	\
	figures.gen/modem_8ask.eps	\
	figures.gen/modem_16ask.eps	\
					\
	figures.gen/modem_8qam.eps	\
	figures.gen/modem_16qam.eps	\
	figures.gen/modem_32qam.eps	\
	figures.gen/modem_64qam.eps	\
	figures.gen/modem_128qam.eps	\
	figures.gen/modem_256qam.eps	\
					\
	figures.gen/modem_arb16opt.eps	\
	figures.gen/modem_arb64vt.eps

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))
local_pdffiles := $(patsubst %.eps,%.pdf,$(local_epsfiles))

# gnuplot script generator function
src/modem.genplot : src/modem.genplot.c

# modem data generator function
src/modem.gendata : src/modem.gendata.c

## 
## psk
##
figures.gen/modem_bpsk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m bpsk -p 1
figures.gen/modem_bpsk.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m bpsk -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_qpsk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m qpsk -p 2
figures.gen/modem_qpsk.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m qpsk -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8psk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 3
figures.gen/modem_8psk.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m psk -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16psk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 4
figures.gen/modem_16psk.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m psk -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

## 
## apsk
##
figures.gen/modem_4apsk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 2
figures.gen/modem_4apsk.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m apsk -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8apsk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 3
figures.gen/modem_8apsk.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m apsk -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16apsk.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 4
figures.gen/modem_16apsk.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m apsk -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_32apsk.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 5
figures.gen/modem_32apsk.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m apsk -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_64apsk.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 6
figures.gen/modem_64apsk.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m apsk -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_128apsk.dat: %.dat : src/modem.gendata ; ./$< -f $@ -m apsk -p 7
figures.gen/modem_128apsk.gnu: %.gnu : src/modem.genplot ; ./$< -f $@ -m apsk -p 7 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

## 
## ask
##
figures.gen/modem_2ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 1
figures.gen/modem_2ask.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_4ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 2
figures.gen/modem_4ask.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 3
figures.gen/modem_8ask.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16ask.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 4
figures.gen/modem_16ask.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

## 
## qam
##
figures.gen/modem_8qam.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 3
figures.gen/modem_8qam.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 4
figures.gen/modem_16qam.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_32qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 5
figures.gen/modem_32qam.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 5 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_64qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 6
figures.gen/modem_64qam.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_128qam.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 7
figures.gen/modem_128qam.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 7 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_256qam.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 8
figures.gen/modem_256qam.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 8 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

## 
## arb
##
figures.gen/modem_arb16opt.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m arb16opt -p 4
figures.gen/modem_arb16opt.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m arb16opt -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_arb64vt.dat : %.dat : src/modem.gendata ; ./$< -f $@ -m arb64vt -p 6
figures.gen/modem_arb64vt.gnu : %.gnu : src/modem.genplot ; ./$< -f $@ -m arb64vt -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

##
## target collection
## 

# target pdf files
$(local_pdffiles) : %.pdf : %.eps
	$(EPSTOPDF) $(EPSTOPDF_FLAGS) $< --outfile=$@

# eps target files
$(local_epsfiles) : %.eps : %.gnu %.dat
	$(GNUPLOT) $< > $@

# accumulate target
figures_generated += $(local_pdffiles)

figures_extra_clean +=			\
	src/modem.genplot		\
	src/modem.gendata

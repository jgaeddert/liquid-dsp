#
# Makefile for generating liquid documentation modem figures
#

GNUPLOT_VERSION = 4.2
GNUPLOT = gnuplot

local_epsfiles :=			\
	figures.gen/modem_bpsk.eps	\
	figures.gen/modem_qpsk.eps	\
	figures.gen/modem_8psk.eps	\
	figures.gen/modem_16psk.eps	\
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
	figures.gen/modem_256qam.eps

local_gnufiles := $(patsubst %.eps,%.gnu,$(local_epsfiles))
local_datfiles := $(patsubst %.eps,%.dat,$(local_epsfiles))

# gnuplot script generator function
src/modem.genplot : src/modem.genplot.c

# modem data generator function
src/modem.gendata : src/modem.gendata.c

# eps target files
$(local_epsfiles) : %.eps : %.gnu %.dat
	$(GNUPLOT) $< > $@

# 
# psk
#
figures.gen/modem_bpsk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m bpsk -p 1
figures.gen/modem_bpsk.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m bpsk -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_qpsk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m qpsk -p 2
figures.gen/modem_qpsk.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m qpsk -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8psk.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 3
figures.gen/modem_8psk.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m psk -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16psk.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m psk -p 4
figures.gen/modem_16psk.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m psk -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

# 
# ask
#
figures.gen/modem_2ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 1
figures.gen/modem_2ask.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 1 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_4ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 2
figures.gen/modem_4ask.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 2 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_8ask.dat   : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 3
figures.gen/modem_8ask.gnu   : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 3 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_16ask.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m ask -p 4
figures.gen/modem_16ask.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m ask -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

# 
# qam
#
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

# accumulate target
figures_generated += $(local_epsfiles)

extra_clean +=				\
	src/modem.genplot		\
	src/modem.gendata		\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
	$(local_datfiles)


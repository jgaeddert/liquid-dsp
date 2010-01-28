#
# Makefile for generating liquid documentation modem figures
#

GNUPLOT_VERSION = 4.2
GNUPLOT = gnuplot

local_epsfiles :=			\
	figures.gen/modem_16qam.eps	\
	figures.gen/modem_64qam.eps	\
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
# qam
#
figures.gen/modem_16qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 4
figures.gen/modem_16qam.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 4 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

figures.gen/modem_64qam.dat  : %.dat : src/modem.gendata ; ./$< -f $@ -m qam -p 6
figures.gen/modem_64qam.gnu  : %.gnu : src/modem.genplot ; ./$< -f $@ -m qam -p 6 -t eps -d $*.dat -g $(GNUPLOT_VERSION)

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


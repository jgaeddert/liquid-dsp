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

# gnu target files (gnuplot)
$(local_gnufiles) : %.gnu : %.dat src/modem.genplot
	./src/modem.genplot -f $@ -g $(GNUPLOT_VERSION) -t eps -d $<

# psk
figures.gen/modem_bpsk.dat   : src/modem.gendata ; ./$< -f $@ -m qpsk -p 1
figures.gen/modem_qpsk.dat   : src/modem.gendata ; ./$< -f $@ -m qpsk -p 2
figures.gen/modem_8psk.dat   : src/modem.gendata ; ./$< -f $@ -m  psk -p 3
figures.gen/modem_16psk.dat  : src/modem.gendata ; ./$< -f $@ -m  psk -p 4

# qam
figures.gen/modem_16qam.dat  : src/modem.gendata ; ./$< -f $@ -m  qam -p 4
figures.gen/modem_64qam.dat  : src/modem.gendata ; ./$< -f $@ -m  qam -p 6
figures.gen/modem_256qam.dat : src/modem.gendata ; ./$< -f $@ -m  qam -p 8

# accumulate target
figures_generated += $(local_epsfiles)

extra_clean +=				\
	src/modem.genplot		\
	src/modem.gendata		\
	$(local_epsfiles)		\
	$(local_gnufiles)		\
	$(local_datfiles)


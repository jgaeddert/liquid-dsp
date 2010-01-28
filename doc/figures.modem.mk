#
# Makefile for generating liquid documentation modem figures
#

GNUPLOT_VERSION = 4.2
GNUPLOT = gnuplot

figures_generated_modem :=		\
	figures.gen/modem_16qam.eps

# gnuplot script generator function
src/modem.genplot : src/modem.genplot.c

# modem data generator function
src/modem.gendata : src/modem.gendata.c

figures.gen/modem_16qam.dat : src/modem.gendata
	./$< -f $@ -g $(GNUPLOT_VERSION) -m qam -p 4
figures.gen/modem_16qam.gnu : src/modem.genplot
	./$< -f $@ -g $(GNUPLOT_VERSION) -t eps -d figures.gen/modem_16qam.dat
figures.gen/modem_16qam.eps : figures.gen/modem_16qam.gnu figures.gen/modem_16qam.dat
	$(GNUPLOT) $< > $@

extra_clean += src/modem.genplot src/modem/gendata

# accumulate target
figures_generated += $(figures_generated_modem)


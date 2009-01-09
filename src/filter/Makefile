# 
# Makefile for filter module
#

EXAMPLES = fir_filter_example iir_filter_example \
		   interp_example decim_example \
		   matched_filter_example firpfb_example \
		   hilbert_decim_example hilbert_interp_example \
		   cresamp2_decim_example cresamp2_interp_example \
		   symsync_example

CC = gcc
CFLAGS = -Wall -O3 -DSYMSYNC_DEBUG=1
LDFLAGS = -lm
OBJS = filter.f.o filter.cf.o \
	   filter_common.o firdes.o \
	   nyquist.o rrcos.o lf2.o
HEADERS = src/filter_common.h src/filter.h src/firdes.h \
		  src/iir_filter.c src/fir_filter.c src/decim.c src/interp.c \
		  src/firpfb.c src/symsync.c src/resamp.c

all: $(EXAMPLES)

MATH_DEPS = lmath.o
BUFFER_DEPS = fbuffer.o cfbuffer.o gport.o gport2.o
DOTPROD_DEPS = dotprod_rrrf.o dotprod_cccf.o dotprod_crcf.o
RANDOM_DEPS = random.o
DEPS = $(MATH_DEPS) $(BUFFER_DEPS) $(DOTPROD_DEPS) $(RANDOM_DEPS)

$(MATH_DEPS) :
	cd ../math ; make $@
	cp ../math/$@ .

$(BUFFER_DEPS) :
	cd ../buffer ; make $@
	cp ../buffer/$@ .

$(DOTPROD_DEPS) :
	cd ../dotprod ; make $@
	cp ../dotprod/$@ .

$(RANDOM_DEPS) :
	cd ../random ; make $@
	cp ../random/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES) *.m

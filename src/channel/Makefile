# 
# Makefile for channel module
#

EXAMPLES = fading_channel_example \
		   fading_generator_example \
		   ricek_channel_example

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = awgn_channel.o ricek_channel.o channel.o
HEADERS = src/channel.h src/channel_internal.h

all: $(EXAMPLES)

DOTPROD_DEPS = dotprod_rrrf.o dotprod_cccf.o
BUFFER_DEPS = fbuffer.o cfbuffer.o gport.o gport2.o
FILTER_DEPS = filter.f.o filter.cf.o firdes.o
RANDOM_DEPS = random.o
MATH_DEPS = lmath.o
DEPS = $(DOTPROD_DEPS) $(BUFFER_DEPS) $(FILTER_DEPS) $(RANDOM_DEPS) $(MATH_DEPS)

$(DOTPROD_DEPS) :
	cd ../dotprod ; make $@
	cp ../dotprod/$@ .

$(BUFFER_DEPS) :
	cd ../buffer ; make $@
	cp ../buffer/$@ .

$(FILTER_DEPS) :
	cd ../filter ; make $@
	cp ../filter/$@ .

$(RANDOM_DEPS) :
	cd ../random ; make $@
	cp ../random/$@ .

$(MATH_DEPS) :
	cd ../math ; make $@
	cp ../math/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES) *.m

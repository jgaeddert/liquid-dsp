# 
# Makefile for framing module
#


EXAMPLES = framing_example cframing_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = framing.f.o framing.cf.o
HEADERS = src/framing.h

BUFFER_DEPS = fbuffer.o cfbuffer.o gport.o gport2.o
DOTPROD_DEPS = dotprod_rrrf.o dotprod_crcf.o dotprod_cccf.o
SEQUENCE_DEPS = bsequence.o msequence.o count_bits.o
RANDOM_DEPS = random.o
DEPS = $(BUFFER_DEPS) $(DOTPROD_DEPS) $(SEQUENCE_DEPS) $(RANDOM_DEPS)

$(BUFFER_DEPS) :
	cd ../buffer ; make $@
	cp ../buffer/$@ .

$(DOTPROD_DEPS) :
	cd ../dotprod ; make $@
	cp ../dotprod/$@ .

$(SEQUENCE_DEPS) :
	cd ../sequence ; make $@
	cp ../sequence/$@ .

$(RANDOM_DEPS) :
	cd ../random ; make $@
	cp ../random/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES)

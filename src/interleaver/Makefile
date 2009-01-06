# 
# Makefile for interleaver module
#


EXAMPLES = interleaver_example

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = interleaver.o interleaver_create.o interleaver_permute.o
HEADERS = src/interleaver.h src/interleaver_internal.h
all: $(EXAMPLES)

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# dependencies
SEQUENCE_DEPS = msequence.o bsequence.o
UTILITY_DEPS = count_bits.o
DEPS = $(SEQUENCE_DEPS) $(UTILITY_DEPS)

$(SEQUENCE_DEPS) :
	cd ../sequence ; make $@
	cp ../sequence/$@ .

$(UTILITY_DEPS) :
	cd ../utility ; make $@
	cp ../utility/$@ .

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES)

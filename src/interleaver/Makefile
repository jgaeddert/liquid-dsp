# 
# Makefile for interleaver module
#


TESTS = interleaver_test

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = interleaver.o interleaver_create.o interleaver_permute.o
HEADERS = src/interleaver.h src/interleaver_internal.h
all: $(TESTS)

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

$(TESTS): %_test : src/main_%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(TESTS)

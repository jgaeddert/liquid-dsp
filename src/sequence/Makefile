# 
# Makefile for sequence module
#

TESTS = msequence_test

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = msequence.o bsequence.o
HEADERS = src/sequence.h

all: $(TESTS)

UTILITY_DEPS = count_bits.o
DEPS = $(UTILITY_DEPS)

$(UTILITY_DEPS) :
	cd ../utility ; make $@
	cp ../utility/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTS): %_test : src/main_%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(TESTS)

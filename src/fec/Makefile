#
# FEC makefile (local)
#

OBJS = checksum.o crc.o fec_rep.o fec_hamming74.o
TESTS = checksum_test crc_test fec_rep3_test fec_hamming74_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm

# 
# Object targets
#
$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#
# External dependencies
#
UTILITY_DEPS = count_bits.o
DEPS = $(UTILITY_DEPS)

$(UTILITY_DEPS) :
	cd ../utility ; make $@
	cp ../utility/$@ .

# 
# Tests: src/main_mytest.c -> mytest_test
#
$(TESTS) : %_test : src/main_%.c $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TESTS) $(DEPS)

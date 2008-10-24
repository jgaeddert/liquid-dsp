#
# FEC makefile (local)
#

OBJS = checksum.o crc.o fec_rep.o
TESTS = checksum_test crc_test fec_rep3_test

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
# Tests: src/main_mytest.c -> mytest_test
#
$(TESTS) : %_test : src/main_%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

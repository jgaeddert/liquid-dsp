# 
# Makefile for utility module
#


TESTS = symbol_errors_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = count_bits.o pack_bytes.o
HEADERS = src/utility.h

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTS): %_test : src/main_%.c $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

# 
# Makefile for nco module
#


TESTS = nco_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm
OBJS = nco.o
HEADERS = src/nco.h

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTS): %_test : src/main_%.c $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

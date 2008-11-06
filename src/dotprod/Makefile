# 
# Makefile for dotprod module
#

TESTS = fdotprod_test cfdotprod_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = fdotprod.o cfdotprod.o
HEADERS = src/dotprod.h

$(TESTS) : %_test : src/main_%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

# 
# Makefile for framing module
#


TESTS = framing_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = framing.o
HEADERS = src/framing.h

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTS): %_test : src/main_%.c $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o *_test

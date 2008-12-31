# 
# Makefile for dotprod module
#

EXAMPLES = fdotprod_example cfdotprod_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
#OBJS = fdotprod.o cfdotprod.o
OBJS = dotprod_rrrf.o dotprod_cccf.o dotprod_crcf.o
HEADERS = src/dotprod.h

$(EXAMPLES) : % : examples/%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXAMPLES)

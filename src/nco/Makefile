# 
# Makefile for nco module
#


EXAMPLES = nco_example pll_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm
OBJS = nco.o pll.o
HEADERS = src/nco.h

FILTER_DEPS = lf2.o
DEPS = $(FILTER_DEPS)

$(FILTER_DEPS) :
	cd ../filter ; make $@
	cp ../filter/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES) : %_example : examples/%_example.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES) *.m

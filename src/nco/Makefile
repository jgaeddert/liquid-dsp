# 
# Makefile for nco module
#


#EXAMPLES = nco_test
EXAMPLES = pll_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm
OBJS = nco.o pll.o
HEADERS = src/nco.h

FILTER_DEPS = lf2.o
RANDOM_DEPS = random.o
DEPS = $(FILTER_DEPS) $(RANDOM_DEPS)

$(FILTER_DEPS) :
	cd ../filter ; make $@
	cp ../filter/$@ .

$(RANDOM_DEPS) :
	cd ../random ; make $@
	cp ../random/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES) : %_example : examples/%_example.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES)

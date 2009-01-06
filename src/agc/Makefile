# 
# Makefile for agc module
#

EXAMPLES = agc_example

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = agc.o
HEADERS = src/agc.h src/agc_internal.h

all: $(EXAMPLES)

BUFFER_DEPS = fwindow.o cfwindow.o
DEPS = 

$(BUFFER_DEPS) :
	cd ../buffer ; make $@
	cp ../buffer/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES) *.m

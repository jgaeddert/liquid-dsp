# 
# Makefile for sequence module
#

EXAMPLES = fir_filter_example

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = decim.o decim2.o decim_create.o decim_execute.o \
	   filter_common.o fir_filter.o firdes.o \
	   nyquist.o rrcos.o window.o lf2.o
HEADERS = src/decim2.h src/decim_internal.h src/filter_common.h \
		  src/filter.h src/firdes.h src/filter.h \
		  src/polyphase.h src/window.h

all: $(EXAMPLES)

MATH_DEPS = math.o
BUFFER_DEPS = fbuffer.o
DOTPROD_DEPS = fdotprod.o
DEPS = $(MATH_DEPS) $(BUFFER_DEPS) $(DOTPROD_DEPS)

$(MATH_DEPS) :
	cd ../math ; make $@
	cp ../math/$@ .

$(BUFFER_DEPS) :
	cd ../buffer ; make $@
	cp ../buffer/$@ .

$(DOTPROD_DEPS) :
	cd ../dotprod ; make $@
	cp ../dotprod/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES)

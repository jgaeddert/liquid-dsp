# 
# Makefile for modem module
#

EXAMPLES = modem_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = modem.o modem_create.o modem_common.o modulate.o demodulate.o

UTILITY_DEPS = count_bits.o
DEPS = $(UTILITY_DEPS)

$(UTILITY_DEPS) :
	cd ../utility ; make $@
	cp ../utility/$@ .

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES) : %_example : examples/%_example.c $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(EXAMPLES) $(DEPS)

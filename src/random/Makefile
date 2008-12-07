# 
# Makefile for random module
#

EXAMPLES = main_random main_scramble

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = random.o scramble.o
HEADERS = src/random.h src/scramble.h

all: $(EXAMPLES)

MATH_DEPS = 
DEPS = $(MATH_DEPS)

$(MATH_DEPS) :
	cd ../math ; make $@
	cp ../math/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES)

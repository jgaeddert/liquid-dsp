# 
# Makefile for math module
#

EXAMPLES = fmatrix_example cfmatrix_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = lmath.o fmatrix.o cfmatrix.o
HEADERS = src/lmath.h src/matrix.h src/matrix_internal.h

$(EXAMPLES) : % : examples/%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXAMPLES)

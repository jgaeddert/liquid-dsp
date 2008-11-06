# 
# Makefile for math module
#

TESTS = matrix_test cfmatrix_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = math.o fmatrix.o cfmatrix.o
HEADERS = src/math.h src/matrix.h src/matrix_internal.h

$(TESTS) : %_test : src/main_%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

# 
# Makefile for math module
#

TESTS = matrix_test cfmatrix_test fdotprod_test cfdotprod_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = math.o fdotprod.o cfdotprod.o fmatrix.o cfmatrix.o
HEADERS = src/dotprod.h src/math.h src/matrix.h src/matrix_internal.h

$(TESTS) : %_test : src/main_%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

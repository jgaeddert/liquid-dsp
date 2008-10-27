# 
# Makefile for math module
#

all: matrix_test cfmatrix_test

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = math.o dotprod.o fmatrix.o cfmatrix.o
HEADERS = src/dotprod.h src/math.h src/matrix.h src/matrix_internal.h

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

matrix_test: $(OBJS) src/main_matrix.c src/math.h
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_matrix.c -o $@

cfmatrix_test: $(OBJS) src/main_cfmatrix.c $(HEADERS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_cfmatrix.c -o $@

clean:
	rm -f *.o *_test

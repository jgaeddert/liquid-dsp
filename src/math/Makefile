# 
# Makefile for math module
#

all: matrix_test

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = math.o dotprod.o matrix.o

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

matrix_test: $(OBJS) src/main_matrix.c src/math.h
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_matrix.c -o $@

clean:
	rm -f *.o *_test

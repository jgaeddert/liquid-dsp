# 
# Makefile for fft module
#

all: fft_test

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = fft_create_plan.o fft_execute.o

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

fft_test: $(OBJS) src/main_fft.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_fft.c -o $@

autotest: $(OBJS) tests/autotest.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) tests/autotest.c -o $@

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

# 
# Makefile for fft module
#

TESTS = fft_test
all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = fft_create_plan.o fft_execute.o

$(TESTS) : %_test : src/main_%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TESTS)

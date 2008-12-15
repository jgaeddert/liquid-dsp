# 
# Makefile for fft module
#

EXAMPLES = fft_example asgram_example
all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm
OBJS = fft_create_plan.o fft_execute.o fft_shift.o asgram.o

$(EXAMPLES) : % : examples/%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXAMPLES)

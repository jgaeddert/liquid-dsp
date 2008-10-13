# 
# Makefile for math module
#

all: kaiser_test firdes_test decim2_test

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = kaiser.o firdes.o decim2.o

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kaiser_test: $(OBJS) src/main_kaiser.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_kaiser.c -o $@

firdes_test: $(OBJS) src/main_firdes.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_firdes.c -o $@

decim2_test: $(OBJS) src/main_decim2.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_decim2.c -o $@

autotest: $(OBJS) tests/autotest.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) tests/autotest.c -o $@

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

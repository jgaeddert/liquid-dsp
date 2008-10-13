# 
# Makefile for modem module
#

all: modem_test autotest

CC = gcc
CFLAGS = -Wall -std=c99 -O3
LDFLAGS = -lm
OBJS = modem.o modem_create.o modem_common.o modulate.o demodulate.o

$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

modem_test: $(OBJS) src/main_modem.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) src/main_modem.c -o $@

autotest: $(OBJS) tests/autotest.c
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) tests/autotest.c -o $@

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

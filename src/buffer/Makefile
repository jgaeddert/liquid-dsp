#
# Buffer makefile (local)
#

OBJS = fbuffer.o cfbuffer.o uibuffer.o metadata.o port.o
TESTS = metadata_test cfbuffer_test

all: $(TESTS)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm

# 
# Buffer object targets
#
$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 
# Tests: src/main_mytest.c -> mytest_test
#
$(TESTS) : %_test : src/main_%.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o *_test

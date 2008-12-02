#
# Buffer makefile (local)
#

OBJS = fbuffer.o cfbuffer.o uibuffer.o metadata.o port.o \
	   fwindow.o cfwindow.o
EXAMPLES = metadata_example fbuffer_example cfbuffer_example \
		   fwindow_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm

# 
# Buffer object targets
#
$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 
# Tests: src/main_mytest.c -> mytest_example
#
$(EXAMPLES) : %_example : examples/%_example.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(EXAMPLES)

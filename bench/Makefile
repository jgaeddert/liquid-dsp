#
# Benchmark example
#

CC	:= gcc
#MV	:= mv -f
RM	:= rm -f
#SED	:= sed
#AR	:= ar
#RANLIB	:= ranlib

CFLAGS	:= -I . -Wall
LDFLAGS := -lm -lc

all: benchmark

benchmarkgen.o main.o : %.o : %.c benchmarkgen.h
	$(CC) $(CFLAGS) -c -o $@ $<

benchmarkgen : benchmarkgen.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

benchinclude.h : benchmarkgen bench_example.h
	./$< > $@

benchmark.o: bench.c benchinclude.h
	$(CC) $(CFLAGS) -c bench.c -o $@

benchmark: benchmark.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	$(RM) benchmarkgen.o main.o benchmarkgen benchinclude.h benchmark.o benchmark



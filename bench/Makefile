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

benchinclude.h: benchmarkgen.py bench_example.h
	python benchmarkgen.py bench_example.h

benchmark.o: bench.c benchinclude.h
	$(CC) $(CFLAGS) -c bench.c -o $@

benchmark: benchmark.o
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	$(RM) benchinclude.h benchmark.o benchmark



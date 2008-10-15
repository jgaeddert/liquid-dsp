#
# Benchmark example
#

all: bench

benchinclude.h: benchmarkgen.py bench_example.h
	python benchmarkgen.py bench_example.h

bench: bench.c benchinclude.h
	gcc -Wall bench.c -o bench

clean:
	rm -f benchinclude.h *.o bench



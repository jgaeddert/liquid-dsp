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

benchmark_headers :=		\
	example_benchmark.h	\

benchmark_headers_obj = $(patsubst %.h,%.o,$(benchmark_headers))

benchmarkgen.o main.o : %.o : %.c benchmarkgen.h
	$(CC) $(CFLAGS) -c -o $@ $<

benchmarkgen : benchmarkgen.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

benchmark_include.h : benchmarkgen example_benchmark.h
	./benchmarkgen ./example_benchmark.h > $@

# NOTE: by default, gcc compiles any file with a '.h' extension as a 'pre-compiled
#       header' so we need to explicity tell it to compile as a c source file with
#       the '-x c' flag
$(benchmark_headers_obj) : %.o : %.h
	$(CC) $(CFLAGS) -x c $< -c -o $@

benchmark.o: bench.c benchmark_include.h
	$(CC) $(CFLAGS) -c bench.c -o $@

benchmark : benchmark.o $(benchmark_headers_obj)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	$(RM) benchmarkgen.o main.o benchmarkgen benchmark_include.h benchmark.o benchmark
	$(RM) $(benchmark_headers_obj)




CFLAGS := -Wall -I/opt/local/include -I. -I./include

LDFLAGS := -lm -lc -lfftw3

all: fft_r2r_test

test: fft_r2r_test
	./$<

fft_r2r_test : % : %.c

clean:
	rm -f *.o
	rm -f fft_r2r_test

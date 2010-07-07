#
# sandbox makefile
#

CFLAGS := -Wall -I/opt/local/include -I. -I.. -I../include

LDFLAGS := -lm -lc -lfftw3

local_src =				\
	ellip_func_test.c		\
	ellip_test.c			\
	fec_rep3_test.c			\
	fec_rep5_test.c			\
	fft_r2r_test.c

	#newbench_example.c
	#iirdes_example.c
	#fec_hamming128_example.c

local_obj	= $(patsubst %.c,%.c.o,$(local_src))
local_prog	= $(patsubst %.c,%,$(local_src))

all: $(local_prog)

test: fft_r2r_test
	./$<

clean:
	rm -f $(local_obj)
	rm -f $(local_prog)

#
# FEC makefile (local)
#

OBJS = c_ones_mod2.o checksum.o crc.o \
	   fec.o \
	   fec_pass.o \
	   fec_rep.o \
	   fec_hamming74.o

EXAMPLES = checksum_example \
		   crc_example \
		   fec_rep3_example \
		   fec_hamming74_example

all: $(EXAMPLES) 
#c_ones_mod2_gentab

CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -lm

# 
# Object targets
#
$(OBJS) : %.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#
# External dependencies
#
UTILITY_DEPS = count_bits.o
DEPS = $(UTILITY_DEPS)

$(UTILITY_DEPS) :
	cd ../utility ; make $@
	cp ../utility/$@ .

# 
# Examples
#
$(EXAMPLES) : % : examples/%.c $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

c_ones_mod2_gentab: src/c_ones_mod2_gentab.c
	$(CC) $(CFLAGS) src/c_ones_mod2_gentab.c -o c_ones_mod2_gentab

clean:
	rm -f $(OBJS) $(EXAMPLES) $(DEPS)

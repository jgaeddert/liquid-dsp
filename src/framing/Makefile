# 
# Makefile for framing module
#


EXAMPLES = frame_example framesync_example cframesync_example packetizer_example

all: $(EXAMPLES)

CC = gcc
CFLAGS = -Wall -O3 -I../modem/src -I../sequence/src -I../fec/src
LDFLAGS = -lm
OBJS = framing.o packetizer.o framesync.f.o framesync.cf.o
HEADERS = src/framing.h src/framing_internal.h

BUFFER_DEPS = fbuffer.o cfbuffer.o gport.o gport2.o
DOTPROD_DEPS = dotprod_rrrf.o dotprod_crcf.o dotprod_cccf.o
FEC_DEPS = fec.o fec_pass.o fec_rep.o fec_hamming74.o c_ones_mod2.o crc.o
INTLV_DEPS = interleaver.o interleaver_create.o interleaver_permute.o
SEQUENCE_DEPS = bsequence.o msequence.o count_bits.o
RANDOM_DEPS = random.o
DEPS = $(BUFFER_DEPS) $(DOTPROD_DEPS) $(FEC_DEPS) $(INTLV_DEPS) $(SEQUENCE_DEPS) $(RANDOM_DEPS)

$(BUFFER_DEPS) :
	cd ../buffer ; make $@
	cp ../buffer/$@ .

$(DOTPROD_DEPS) :
	cd ../dotprod ; make $@
	cp ../dotprod/$@ .

$(INTLV_DEPS) :
	cd ../interleaver ; make $@
	cp ../interleaver/$@ .

$(FEC_DEPS) :
	cd ../fec ; make $@
	cp ../fec/$@ .

$(SEQUENCE_DEPS) :
	cd ../sequence ; make $@
	cp ../sequence/$@ .

$(RANDOM_DEPS) :
	cd ../random ; make $@
	cp ../random/$@ .

$(OBJS) : %.o : src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLES): % : examples/%.c $(OBJS) $(HEADERS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) $(DEPS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXAMPLES)

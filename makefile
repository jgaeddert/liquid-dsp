# 
# Makefile for liquid SDR libraries
#

prefix = /usr/local
exec_prefix = ${prefix}

CC = gcc
LIBS_AGC = agc.o
LIBS_ANN = ann.o ann_layer.o ann_neuron.o
LIBS_BUFFER = cbuffer.o sbuffer.o metadata.o port.o
LIBS_FEC = checksum.o
LIBS_FILTER = fir_filter.o rrcos.o
LIBS_INTERLEAVER = interleaver.o
LIBS_MATH = kaiser.o firdes.o decim2.o
LIBS_MODEM = modem.o modulate.o demodulate.o modem_common.o modem_create.o
LIBS_NCO = nco.o
#LIBS_OPTIM = 
LIBS_RANDOM = random.o scramble.o
#LIBS_SEQUENCE = 
#LIBS_UTILITY = 

LIBS = $(LIBS_AGC) $(LIBS_ANN) $(LIBS_BUFFER) \
	   $(LIBS_FEC) $(LIBS_FILTER) $(LIBS_INTERLEAVER) \
	   $(LIBS_MATH) $(LIBS_MODEM) $(LIBS_NCO) $(LIBS_RANDOM)

CFLAGS = -g -O2 -Wall

SHARED_LIB = libliquid.dylib # darwin
#SHARED_LIB = libliquid.so # linux et al

all: libliquid.a $(SHARED_LIB)

install:
	@echo "installing..."
	mkdir -p ${exec_prefix}/lib
	install -m 644 -p $(SHARED_LIB) libliquid.a ${exec_prefix}/lib
	#mkdir -p ${prefix}/include
	#install -m 644 -p /api/liquid_apis.h

libliquid.a: $(LIBS)
	ar rv $@ $^
	ranlib libliquid.a

libliquid.dylib: $(LIBS)
	$(CC) -dynamiclib -install_name $@ -o $@ $^

libliquid.so: $(LIBS)
	$(CC) -shared -Xlinker -soname=$@ -o $@ -Wl,-whole-archive $^ -Wl,-no-whole-archive -lc

$(LIBS_AGC) : %.o : src/agc/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_ANN) : %.o : src/ann/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_BUFFER) : %.o : src/buffer/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_FEC) : %.o : src/fec/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_FILTER) : %.o : src/filter/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_INTERLEAVER) : %.o : src/interleaver/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_MATH) : %.o : src/math/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_MODEM) : %.o : src/modem/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_NCO) : %.o : src/nco/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBS_RANDOM) : %.o : src/random/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o $(SHARED_LIB) *.a


all: sbuffer cbuffer metadata port buffer_test

sbuffer: src/sbuffer.c src/main_sbuffer.c
	gcc -Wall -c src/sbuffer.c -o sbuffer.o
	gcc -Wall -c src/main_sbuffer.c -o main_sbuffer.o
	gcc -Wall sbuffer.o main_sbuffer.o -lm -o sbuffer_test

cbuffer: src/cbuffer.c src/main_cbuffer.c
	gcc -Wall -c src/cbuffer.c -o cbuffer.o
	gcc -Wall -c src/main_cbuffer.c -o main_cbuffer.o
	gcc -Wall cbuffer.o main_cbuffer.o -lm -o cbuffer_test

metadata: src/metadata.c src/main_metadata.c
	gcc -Wall -c src/metadata.c -o metadata.o
	gcc -Wall -c src/main_metadata.c -o main_metadata.o
	gcc -Wall metadata.o main_metadata.o -lm -o metadata_test

port: cbuffer metadata src/port.c src/port.h buffer.o
	gcc -Wall -c src/port.c -o port.o
	gcc -Wall -c src/main_port.c -o main_port.o
	gcc -Wall port.o main_port.o buffer.o -lm -o port_test

buffer.o: src/buffer.c src/buffer.h src/buffer_internal.h
	gcc -Wall -c src/buffer.c -o buffer.o

main_buffer.o: src/buffer.h src/main_buffer.c
	gcc -Wall -c src/main_buffer.c -o main_buffer.o

buffer_test: buffer.o main_buffer.o
	gcc -Wall buffer.o main_buffer.o -o buffer_test

clean:
	rm -f *.o *_test

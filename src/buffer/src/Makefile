
all: sbuffer cbuffer metadata port buffer_test

sbuffer: sbuffer.c main_sbuffer.c
	gcc -Wall -c sbuffer.c -o sbuffer.o
	gcc -Wall -c main_sbuffer.c -o main_sbuffer.o
	gcc -Wall sbuffer.o main_sbuffer.o -lm -o sbuffer_test

cbuffer: cbuffer.c main_cbuffer.c
	gcc -Wall -c cbuffer.c -o cbuffer.o
	gcc -Wall -c main_cbuffer.c -o main_cbuffer.o
	gcc -Wall cbuffer.o main_cbuffer.o -lm -o cbuffer_test

metadata: metadata.c main_metadata.c
	gcc -Wall -c metadata.c -o metadata.o
	gcc -Wall -c main_metadata.c -o main_metadata.o
	gcc -Wall metadata.o main_metadata.o -lm -o metadata_test

port: cbuffer metadata
	gcc -Wall -c port.c -o port.o
	gcc -Wall -c main_port.c -o main_port.o
	gcc -Wall port.o main_port.o cbuffer.o -lm -o port_test

buffer.o: buffer.c buffer.h buffer_internal.h
	gcc -Wall -c buffer.c -o buffer.o

main_buffer.o: buffer.h
	gcc -Wall -c main_buffer.c -o main_buffer.o

buffer_test: buffer.o main_buffer.o
	gcc -Wall buffer.o main_buffer.o -o buffer_test

clean:
	rm -f *.o *_test

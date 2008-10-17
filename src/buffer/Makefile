
all: metadata port fbuffer_test cfbuffer_test

metadata: src/metadata.c src/main_metadata.c
	gcc -Wall -c src/metadata.c -o metadata.o
	gcc -Wall -c src/main_metadata.c -o main_metadata.o
	gcc -Wall metadata.o main_metadata.o -lm -o metadata_test

port: metadata src/port.c src/port.h fbuffer.o
	gcc -Wall -c src/port.c -o port.o
	gcc -Wall -c src/main_port.c -o main_port.o
	gcc -Wall port.o main_port.o fbuffer.o -lm -o port_test

fbuffer.o: src/buffer.c src/buffer.h src/buffer_internal.h src/fbuffer.c src/buffer_macros.h
	gcc -Wall -c src/fbuffer.c -o fbuffer.o

cfbuffer.o: src/buffer.c src/buffer.h src/buffer_internal.h src/cfbuffer.c src/buffer_macros.h
	gcc -Wall -c src/cfbuffer.c -o cfbuffer.o

# float buffer test
fbuffer_test: fbuffer.o
	gcc -Wall src/main_buffer.c fbuffer.o -o fbuffer_test

# complex float buffer test
cfbuffer_test: cfbuffer.o
	gcc -Wall src/main_cfbuffer.c cfbuffer.o -o cfbuffer_test

clean:
	rm -f *.o *_test

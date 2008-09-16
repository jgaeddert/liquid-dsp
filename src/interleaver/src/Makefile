
all: clean interleaver

interleaver:
	gcc -Wall -c interleaver.c -o interleaver.o
	gcc -Wall -c main_interleaver.c -o main_interleaver.o
	gcc -Wall interleaver.o main_interleaver.o -o interleaver_test


clean:
	rm -f *.o *_test

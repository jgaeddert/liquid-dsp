
all: clean checksum

checksum:
	gcc -Wall -c checksum.c -o checksum.o
	gcc -Wall -c main_checksum.c -o main_checksum.o
	gcc -Wall checksum.o main_checksum.o -o checksum_test


clean:
	rm -f *.o *_test

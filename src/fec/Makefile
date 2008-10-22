
all: checksum_test

checksum.o: src/checksum.c src/fec.h
	gcc -Wall -c src/checksum.c -o checksum.o

main_checksum.o: src/main_checksum.c src/fec.h
	gcc -Wall -c src/main_checksum.c -o main_checksum.o

checksum_test: checksum.o main_checksum.o
	gcc -Wall checksum.o main_checksum.o -o checksum_test

clean:
	rm -f *.o *_test autotest

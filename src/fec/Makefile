
all: checksum_test crc_test

checksum.o: src/checksum.c src/fec.h src/fec_internal.h
	gcc -Wall -c src/checksum.c -o checksum.o

main_checksum.o: src/main_checksum.c src/fec.h src/fec_internal.h
	gcc -Wall -c src/main_checksum.c -o main_checksum.o

checksum_test: checksum.o main_checksum.o
	gcc -Wall checksum.o main_checksum.o -o checksum_test

crc.o: src/crc.c src/fec.h src/fec_internal.h
	gcc -Wall -c src/crc.c -o crc.o

main_crc.o: src/main_crc.c src/fec.h src/fec_internal.h
	gcc -Wall -c src/main_crc.c -o main_crc.o

crc_test: crc.o main_crc.o
	gcc -Wall crc.o main_crc.o -o crc_test

clean:
	rm -f *.o *_test


all: checksum_test autotest

checksum.o: src/checksum.c src/checksum.h
	gcc -Wall -c src/checksum.c -o checksum.o

main_checksum.o: src/main_checksum.c src/checksum.h
	gcc -Wall -c src/main_checksum.c -o main_checksum.o

checksum_test: checksum.o main_checksum.o
	gcc -Wall checksum.o main_checksum.o -o checksum_test

autotest.o: tests/autotest.c tests/checksum_autotest.h src/checksum.h
	gcc -Wall -c tests/autotest.c -o autotest.o

autotest: autotest.o checksum.o
	gcc -Wall autotest.o checksum.o -o autotest

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

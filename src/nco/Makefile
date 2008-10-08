
all: nco_test autotest

nco.o: src/nco.c
	gcc -Wall -c src/nco.c -o nco.o

main_nco.o: src/main_nco.c
	gcc -Wall -c src/main_nco.c -o main_nco.o

nco_test: nco.o main_nco.o
	gcc -Wall nco.o main_nco.o -lm -o nco_test

autotest.o: tests/autotest.c
	gcc -Wall -c tests/autotest.c -o autotest.o

autotest: autotest.o nco.o
	gcc -Wall nco.o autotest.o -lm -o autotest

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest


all: agc_test

agc.o: src/agc.c src/agc_internal.h
	gcc -Wall -c src/agc.c -o agc.o

main_agc.o: src/main_agc.c src/agc.h
	gcc -Wall -c src/main_agc.c -o main_agc.o

agc_test: agc.o main_agc.o
	gcc -Wall agc.o main_agc.o -lm -o agc_test

autotest.o: tests/autotest.c
	gcc -Wall -c tests/autotest.c -o autotest.o

autotest: autotest.o agc.o agc_create.o agc_common.o modulate.o demodulate.o
	gcc -Wall agc.o agc_common.o agc_create.o modulate.o demodulate.o autotest.o -lm -o autotest

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

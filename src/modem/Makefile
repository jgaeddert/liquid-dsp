
all: modem_test autotest

modem.o: src/modem.c
	gcc -Wall -c src/modem.c -o modem.o

modem_create.o: src/modem_create.c
	gcc -Wall -c src/modem_create.c -o modem_create.o

modem_common.o: src/modem_common.c
	gcc -Wall -c src/modem_common.c -o modem_common.o

modulate.o: src/modulate.c
	gcc -Wall -c src/modulate.c -o modulate.o

demodulate.o: src/demodulate.c
	gcc -Wall -c src/demodulate.c -o demodulate.o

main_modem.o: src/main_modem.c
	gcc -Wall -c src/main_modem.c -o main_modem.o

modem_test: modem.o modem_create.o modem_common.o modulate.o demodulate.o main_modem.o
	gcc -Wall modem.o modem_common.o modem_create.o modulate.o demodulate.o main_modem.o -lm -o modem_test

autotest.o: tests/autotest.c
	gcc -Wall -c tests/autotest.c -o autotest.o

autotest: autotest.o modem.o modem_create.o modem_common.o modulate.o demodulate.o
	gcc -Wall modem.o modem_common.o modem_create.o modulate.o demodulate.o autotest.o -lm -o autotest

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

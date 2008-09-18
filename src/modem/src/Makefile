
#all: clean modem
all: modem

modem: modem.c main_modem.c modulate.c demodulate.c modem_create.c
	gcc -Wall -c modem.c -o modem.o
	gcc -Wall -c modem_create.c -o modem_create.o
	gcc -Wall -c modem_common.c -o modem_common.o
	gcc -Wall -c modulate.c -o modulate.o
	gcc -Wall -c demodulate.c -o demodulate.o
	gcc -Wall -c main_modem.c -o main_modem.o
	gcc -Wall modem_common.o modem_create.o modulate.o demodulate.o main_modem.o -lm -o modem_test

clean:
	rm -f *.o *_test

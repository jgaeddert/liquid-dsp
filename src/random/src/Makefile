
all: clean scramble

scramble:
	gcc -Wall -c scramble.c -o scramble.o
	gcc -Wall -c main_scramble.c -o main_scramble.o
	gcc -Wall scramble.o main_scramble.o -lm -o scramble_test

clean:
	rm -f *.o *_test


all: clean scramble random

random.o: random.c
	gcc -Wall -c random.c -o random.o

main_random.o: main_random.c
	gcc -Wall -c main_random.c -o main_random.o

random: random.o main_random.o
	gcc -Wall random.o main_random.o -lm -o random_test

scramble:
	gcc -Wall -c scramble.c -o scramble.o
	gcc -Wall -c main_scramble.c -o main_scramble.o
	gcc -Wall scramble.o main_scramble.o -lm -o scramble_test

clean:
	rm -f *.o *_test

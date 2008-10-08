
all: clean scramble random autotest

random.o: src/random.c
	gcc -Wall -c src/random.c -o random.o

main_random.o: src/main_random.c
	gcc -Wall -c src/main_random.c -o main_random.o

random: random.o main_random.o
	gcc -Wall random.o main_random.o -lm -o random_test

scramble:
	gcc -Wall -c src/scramble.c -o scramble.o
	gcc -Wall -c src/main_scramble.c -o main_scramble.o
	gcc -Wall scramble.o main_scramble.o -lm -o scramble_test

autotest.o: tests/autotest.c
	gcc -Wall -c tests/autotest.c -o autotest.o

autotest: autotest.o random.o
	gcc -Wall autotest.o random.o -lm -o autotest

check: autotest
	./autotest

clean:
	rm -f *.o *_test autotest

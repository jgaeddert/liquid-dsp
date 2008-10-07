
all: clean fir_filter

rrcos.o: rrcos.c
	gcc -Wall -c rrcos.c -o rrcos.o

fir_filter:
	gcc -Wall -c fir_filter.c -o fir_filter.o
	gcc -Wall -c main_fir_filter.c -o main_fir_filter.o
	gcc -Wall fir_filter.o main_fir_filter.o -lm -o fir_filter_test

clean:
	rm -f *.o *_test


all: clean firdes decim2 kaiser matrix

kaiser:
	gcc -Wall -c kaiser.c -o kaiser.o
	gcc -Wall -c main_kaiser.c -o main_kaiser.o
	gcc -Wall kaiser.o main_kaiser.o -lm -o kaiser_test

firdes: kaiser
	gcc -Wall -c firdes.c -o firdes.o
	gcc -Wall -c main_firdes.c -o main_firdes.o
	gcc -Wall firdes.o main_firdes.o -lm -o firdes_test

decim2: firdes
	gcc -Wall -c decim2.c -o decim2.o
	gcc -Wall -c main_decim2.c -o main_decim2.o
	gcc -Wall decim2.o firdes.o main_decim2.o -lm -o decim2_test

matrix:
	gcc -Wall -c matrix.c -o matrix.o
	gcc -Wall -c main_matrix.c -o main_matrix.o
	gcc -Wall matrix.o main_matrix.o -o matrix_test


clean:
	rm -f *.o *_test

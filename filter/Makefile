
all: clean firdes decim2 cbuff kaiser

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

cbuff:
	gcc -Wall -c cbuff.c -o cbuff.o
	gcc -Wall -c main_cbuff.c -o main_cbuff.o
	gcc -Wall cbuff.o main_cbuff.o -o cbuff_test

clean:
	rm -f *.o decim2_test cbuff_test

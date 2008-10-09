
all: ann_test

ann_neuron.o: src/ann_neuron.c src/ann_internal.h
	gcc -Wall -c src/ann_neuron.c -o ann_neuron.o

ann_layer.o: src/ann_layer.c src/ann_internal.h
	gcc -Wall -c src/ann_layer.c -o ann_layer.o

ann.o: src/ann.c src/ann_internal.h
	gcc -Wall -c src/ann.c -o ann.o

main_ann.o: src/main_ann.c src/ann_internal.h
	gcc -Wall -c src/main_ann.c -o main_ann.o

ann_test: ann_neuron.o ann_layer.o ann.o main_ann.o
	gcc -Wall ann_neuron.o ann_layer.o ann.o main_ann.o -lm -o ann_test

clean:
	rm -f *.o *_test

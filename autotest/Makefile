# 
#
#

all: autotest example

test_autotest.o: test_autotest.c
	gcc -Wall -c test_autotest.c -o test_autotest.o

autotest: test_autotest.o
	gcc -Wall test_autotest.o -o autotest

example: autotest.c
	gcc -Wall autotest.c -o example

clean:
	rm -rf *.o autotest example


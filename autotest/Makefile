# 
#
#

all: autotest

test_autotest.o: test_autotest.c
	gcc -Wall -c test_autotest.c -o test_autotest.o

autotest: test_autotest.o
	gcc -Wall test_autotest.o -o autotest

clean:
	rm -rf *.o autotest


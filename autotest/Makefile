# 
#
#

all: autotest example

test_autotest.o: test_autotest.c
	gcc -Wall -c test_autotest.c -o test_autotest.o

example: test_autotest.o
	gcc -Wall test_autotest.o -o example

autotest_include.h: autotest_example.h
	python autotest_gen.py autotest_example.h

autotest: autotest.c autotest_include.h autotest.h
	gcc -Wall autotest.c -o autotest

clean:
	rm -rf *.o autotest autotest_include.h example


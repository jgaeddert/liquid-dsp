# 
# autotest example
#

all: autotest_example

autotest_example.o: autotest_example.c autotest.h
	gcc -Wall -c $< -o $@

autotest_example: autotest_example.o
	gcc -Wall $< -o $@

check: autotest_example
	./$<

clean:
	rm -f autotest_example.o autotest_example


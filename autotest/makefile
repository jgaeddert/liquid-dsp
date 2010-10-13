# 
# autotest example
#

all: autotest_example

CC	:= gcc
CFLAGS	:= -I.. -Wall

autotestlib.o autotest_example.o : %.o : %.c autotest.h

autotest_example: autotest_example.o autotestlib.o

check: autotest_example
	./$<

clean:
	rm -f autotestlib.o autotest_example.o autotest_example


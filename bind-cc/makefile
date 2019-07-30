
all:

CXXFLAGS := -Wall -O3 -std=c++11

bind-cc/firfilt.o : %.o : %.hh %.cc
	g++ -c ${CXXFLAGS} -I ./include/ -I bind-cc/ -o bind-cc/firfilt.o bind-cc/firfilt.cc

bind-cc/main.o : %.o : %.cc bind-cc/firfilt.hh
	g++ -c ${CXXFLAGS} -I ./include/ -I bind-cc/ -o bind-cc/main.o bind-cc/main.cc

bind-cc/main : % : %.o bind-cc/firfilt.o
	g++ ${CXXFLAGS} -o bind-cc/main $^ -lm -lc -lliquid

all: bind-cc/main

clean:
	rm -f bind-cc/firfilt.o bind-cc/main.o bind-cc/main


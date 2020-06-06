# makefile for python bindings with c++14 and pybind11
all:

#CXXFLAGS := -Wall -O3 -std=c++11 -shared -fPIC
CXXFLAGS := -Wall -O3 -std=c++14 -I./include -I./bind/ # -undefined dynamic_lookup
LIBS := -lm -lc -lliquid

bind_headers :=					\
	bind/firfilt.hh				\
	bind/fg64.hh				\
	bind/fs64.hh				\

# the "-undefined dynamic_lookup" is needed on macOS
# https://pybind11.readthedocs.io/en/stable/compiling.html#building-manually
PYBIND11_INCLUDES := $(shell python3 -m pybind11 --includes)
PYBIND11_SUFFIX   := $(shell python3-config --extension-suffix)
PYBIND11_FLAGS    := -Wall -O3 -std=c++11 -fPIC # -undefined dynamic_lookup
PYBIND11_LIBS     := $(shell python3-config --libs)
PYBIND11_LDFLAGS  := $(shell python3-config --ldflags)

bind/main.o : %.o : %.cc ${bind_headers}
	g++ ${CXXFLAGS} -c -o bind/main.o bind/main.cc

bind/main : % : %.o 
	g++ ${CXXFLAGS} -o bind/main $^ ${LIBS}

# python
bind/liquid.python.o : %.o : %.cc %.hh ${bind_headers}
	g++ ${PYBIND11_FLAGS} ${CXXFLAGS} -D PYTHONLIB ${PYBIND11_INCLUDES} -c -o $@ $<

pythonlib := liquid${PYBIND11_SUFFIX}

${pythonlib} : bind/liquid.python.o
	g++ ${CXXFLAGS}  -shared -fPIC -o $@ $^ ${LIBS} ${PYBIND11_LDFLAGS} ${PYBIND11_LIBS}
#${pythonlib} :
#	g++ ${CXXFLAGS} `python3 -m pybind11 --includes` -D PYTHONLIB bind/liquid.python.cc -o liquid`python3-config --extension-suffix` -lliquid `python3-config --ldflags`

test: ${pythonlib}
	./bind/example.py

all: bind/main ${pythonlib}

clean:
	rm -f bind/*.o bind/main
	rm -f ${pythonlib}


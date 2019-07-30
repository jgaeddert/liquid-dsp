
all:

CXX_INCLUDES := -I./ -I ./include -I ./bind/
CXXFLAGS := -Wall -O3 -std=c++11 ${CXX_INCLUDES}
LIBS := -lm -lc -lliquid

# the "-undefined dynamic_lookup" is needed on macOS
# https://pybind11.readthedocs.io/en/stable/compiling.html#building-manually
PYBIND11_INCLUDES := $(shell python3 -m pybind11 --includes)
PYBIND11_SUFFIX   := $(shell python3-config --extension-suffix)
PYBIND11_FLAGS    := -shared -fPIC -undefined dynamic_lookup
#PYBIND11_LIBS     := $(shell python3-config --libs)
#PYBIND11_LDFLAGS  := $(shell python3-config --ldflags)

bind/firfilt.o : %.o : %.hh %.cc
	g++ ${CXXFLAGS} -c -o bind/firfilt.o bind/firfilt.cc

bind/main.o : %.o : %.cc bind/firfilt.hh
	g++ ${CXXFLAGS} -c -o bind/main.o bind/main.cc

bind/main : % : %.o bind/firfilt.o
	g++ ${CXXFLAGS} -o bind/main $^ ${LIBS}

# python
libs_python := bind/firfilt.python.o
${libs_python} : %.python.o : %.cc %.hh bind/liquid.python.hh
	g++ ${CXXFLAGS} -D PYTHONLIB ${PYBIND11_INCLUDES} -c -o $@ $<
bind/liquid.python.o : %.o : %.cc %.hh
	g++ ${CXXFLAGS} -D PYTHONLIB ${PYBIND11_INCLUDES} -c -o $@ $<

pythonlib := liquid${PYBIND11_SUFFIX}
${pythonlib} : ${libs_python} bind/liquid.python.o
	g++ ${PYBIND11_FLAGS} -o $@ $^ ${LIBS}

all: bind/main ${pythonlib}

clean:
	rm -f bind/*.o bind/main
	rm -f ${pythonlib}


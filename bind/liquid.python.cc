#include "liquid.python.hh"

PYBIND11_MODULE(liquid, m) {
    init_firfilt(m);
}


#include "liquid.python.hh"

PYBIND11_MODULE(LIQUID, m) {
    init_firfilt(m);
}


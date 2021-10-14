// most basic example for using liquid-dsp C++ bindings
#include <stdio.h>
#include "firfilt.hh"

namespace dsp = liquid;

int link_test_func(dsp::firfilt & _filter)
{
    std::cout << _filter << std::endl;
    return 41;
}


// most basic example for using liquid-dsp C++ bindings
#include <stdio.h>
#include "firfilt.hh"

namespace dsp = liquid;

int link_test_func(dsp::firfilt & _filter);

int main()
{
    dsp::firfilt filter(13U, 0.25f);
    int rc = link_test_func(filter);
    std::cout << rc << std::endl;
    return 0;
}

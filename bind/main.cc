// most basic example for using liquid-dsp C++ bindings
#include <stdio.h>
#include "firfilt.hh"

namespace dsp = liquid;

int main()
{
    dsp::firfilt filter;
    for (auto i=0U; i<filter.get_length(); i++) {
        std::complex<float> x(i==0 ? 1.0 : 0.0, 0.0);
        std::complex<float> y = filter.step(x);
        printf("%12.8f %12.8f\n", y.real(), y.imag());
    }
    std::cout << filter << std::endl;
    return 0;
}

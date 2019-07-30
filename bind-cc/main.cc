
#include <stdio.h>
#include "firfilt.hh"

int main()
{
    firfilt filter;
    for (auto i=0; i<10; i++) {
        std::complex<float> x(i==0 ? 1.0 : 0.0, 0.0);
        std::complex<float> y = filter.execute(x);
        printf("%12.8f %12.8f\n", y.real(), y.imag());
    }
    return 0;
}

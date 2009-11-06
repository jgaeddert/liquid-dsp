// file:    listings/nco.c++
// build:   g++ -Wall nco.c++ -lliquid -o nco_test

#include <iostream>
#include <math.h>

#include <liquid/liquid.h>

// NOTE: the definition for liquid_float_complex will change
//       depending upon whether the standard C++ <complex>
//       header file is included before or after including
//       <liquid/liquid.h>
#include <complex>

int main() {
    // create nco object
    nco n = nco_create(LIQUID_NCO);
    nco_set_phase(n,0.3f);
    nco_set_frequency(n,0.0f);

    // Test liquid complex data type
    liquid_float_complex x;
    nco_cexpf(n, &x);
    std::cout << "liquid complex:     "
              << x[0] << " + j" << x[1] << std::endl;

    // Test native c++ complex data type
    std::complex<float> y;
    nco_cexpf(n, reinterpret_cast<liquid_float_complex*>(&y));
    std::cout << "c++ native complex: "
              << y.real() << " + j" << y.imag() << std::endl;

    // destroy nco object
    nco_destroy(n);

    std::cout << "done." << std::endl;
    return 0;
}

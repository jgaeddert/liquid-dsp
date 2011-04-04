// file:    doc/listings/nco.cc
// build:   g++ -c -o nco.cc.o nco.cc
// link:    g++ -lm -lc -lliquid nco.cc.o -o nco

#include <iostream>
#include <math.h>

#include <liquid/liquid.h>

// NOTE: the definition for liquid_float_complex will change
//       depending upon whether the standard C++ <complex>
//       header file is included before or after including
//       <liquid/liquid.h>
#include <complex>

int main() {
    // create nco object and initialize
    nco_crcf n = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase(n,0.3f);

    // Test liquid complex data type
    liquid_float_complex x;
    nco_crcf_cexpf(n, &x);
    std::cout << "liquid complex:     "
              << x.real << " + j" << x.imag << std::endl;

    // Test native c++ complex data type
    std::complex<float> y;
    nco_crcf_cexpf(n, reinterpret_cast<liquid_float_complex*>(&y));
    std::cout << "c++ native complex: "
              << y.real() << " + j" << y.imag() << std::endl;

    // destroy nco object
    nco_crcf_destroy(n);

    std::cout << "done." << std::endl;
    return 0;
}

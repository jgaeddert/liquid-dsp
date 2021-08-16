// create object of each class and print to screen
#include <stdio.h>
#include "fg64.hh"
#include "firfilt.hh"
#include "firinterp.hh"
#include "fs64.hh"
#include "msresamp.hh"
#include "spwaterfall.hh"

namespace dsp = liquid;

int main()
{
    dsp::fg64 fg;
    std::cout << fg << std::endl;

    dsp::firfilt filter;
    std::cout << filter << std::endl;

    dsp::firinterp interp(20);
    std::cout << interp << std::endl;

    dsp::fs64 fs(NULL,NULL);
    std::cout << fs << std::endl;

    dsp::msresamp resamp(0.123456789f);
    std::cout << resamp << std::endl;

    dsp::spwaterfall water;
    std::cout << water << std::endl;

    return 0;
}

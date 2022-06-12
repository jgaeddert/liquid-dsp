// test copy constructor
#include <stdio.h>
#include "modem.hh"

namespace dsp = liquid;

int main()
{
    // create original object
    dsp::modem mod_1("sqam128");
    std::cout << mod_1 << std::endl;

    // copy to new object
    dsp::modem mod_2 = mod_1;
    std::cout << mod_2 << std::endl;
    return 0;
}

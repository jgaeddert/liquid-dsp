// test copy constructor
#include <stdio.h>
#include "modem.hh"
#include "firfilt.hh"

namespace dsp = liquid;

int main()
{
    // create original object
    dsp::modem mod_1("sqam128");
    dsp::modem mod_2 = mod_1;
    std::cout << mod_1 << std::endl;
    std::cout << mod_2 << std::endl;

    // test vector operations
    std::cout << "\ncreating array of modem objects:" << std::endl;
    std::vector<dsp::modem> modems;
    for (auto i=0; i<10; i++)
        modems.emplace_back(LIQUID_MODEM_UNKNOWN + i + 1);
    for (auto m: modems)
        std::cout << m << std::endl;

    std::cout << "\ncreate filter and copy:" << std::endl;
    dsp::firfilt filt_1(57);
    dsp::firfilt filt_2 = filt_1;
    std::cout << filt_1 << std::endl;
    std::cout << filt_2 << std::endl;

    //
    return 0;
}

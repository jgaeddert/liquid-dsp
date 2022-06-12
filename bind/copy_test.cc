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

    // test vector operations
    std::cout << "creating array of modem objects:" << std::endl;
    std::vector<dsp::modem> modems;
    for (auto i=0; i<10; i++)
        modems.emplace_back(LIQUID_MODEM_QPSK);
    for (auto m: modems)
        std::cout << m << std::endl;

    //
    return 0;
}

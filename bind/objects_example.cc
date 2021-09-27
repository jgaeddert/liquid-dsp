// create object of each class and print to screen
#include <stdio.h>

#include "fdelay.hh"
#include "fg64.hh"
#include "firfilt.hh"
#include "firinterp.hh"
#include "firpfbch2.hh"
#include "fs64.hh"
#include "msresamp.hh"
#include "ofdmflexframetx.hh"
#include "ofdmflexframerx.hh"
#include "rresamp.hh"
#include "spgram.hh"
#include "spwaterfall.hh"

namespace dsp = liquid;

int main()
{
    dsp::fdelay fd(400,12);
    std::cout << fd << std::endl;

    dsp::fg64 fg;
    std::cout << fg << std::endl;

    dsp::fs64 fs(NULL,NULL);
    std::cout << fs << std::endl;

    dsp::firfilt filter;
    std::cout << filter << std::endl;

    dsp::firinterp interp(20);
    std::cout << interp << std::endl;

    dsp::firpfbch2a cha(20);
    std::cout << cha << std::endl;

    dsp::firpfbch2s chs(20);
    std::cout << chs << std::endl;

    dsp::msresamp resamp(0.123456789f);
    std::cout << resamp << std::endl;

    dsp::ofdmflexframetx tx(1200);
    std::cout << tx << std::endl;

    dsp::ofdmflexframetx rx(1200);
    std::cout << rx << std::endl;

    dsp::rresamp r(3,5);
    std::cout << r << std::endl;

    dsp::spgram psd;
    std::cout << psd << std::endl;

    dsp::spwaterfall water;
    std::cout << water << std::endl;

    return 0;
}

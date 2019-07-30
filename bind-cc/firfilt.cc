
#include "firfilt.hh"

firfilt::~firfilt()
{
    firfilt_crcf_destroy(q);
}

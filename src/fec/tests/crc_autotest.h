#ifndef __CRC_AUTOTEST_H__
#define __CRC_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/fec_internal.h"

//
// AUTOTEST: reverse byte
//
void autotest_reverse_byte()
{
    // 0110 0010
    unsigned char b = 0x62;

    // 0100 0110
    unsigned char r = 0x46;

    // 
    CONTEND_EQUALITY(reverse_byte(b),r);
}

#endif 


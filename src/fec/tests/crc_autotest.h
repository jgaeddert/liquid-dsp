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

//
// AUTOTEST: reverse uint32_t
//
void autotest_reverse_uint()
{
    // 0110 0010 1101 1001 0011 1011 1111 0000
    unsigned int b = 0x62D93BF0;

    // 0000 1111 1101 1100 1001 1011 0100 0110
    unsigned int r = 0x0FDC9B46;

    // 
    CONTEND_EQUALITY(reverse_uint32(b),r);
}

#endif 


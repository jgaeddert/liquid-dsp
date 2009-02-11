#ifndef __CHECKSUM_AUTOTEST_H__
#define __CHECKSUM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: basic checksum
//
void autotest_basic_checksum()
{
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};
    unsigned char key = checksum_generate_key(data, 4);

    // contend data/key are valid
    CONTEND_EXPRESSION(checksum_validate_message(data, 4, key));

    // corrupt data
    data[0]++;

    // contend data/key are invalid
    CONTEND_EXPRESSION(!checksum_validate_message(data, 4, key));
}

#endif 


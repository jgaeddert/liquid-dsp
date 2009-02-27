//
// Checksum
//

#include <stdio.h>
#include <stdlib.h>

#include "fec_internal.h"

unsigned char checksum_generate_key(unsigned char *_data, unsigned int _n)
{
    unsigned int i, sum=0;
    for (i=0; i<_n; i++)
        sum += (unsigned int) (_data[i]);
    //sum &= 0x00ff;

    // mask and convert to 2's complement
    unsigned char key = ~(sum&0x00ff) + 1;

    return key;
}

int checksum_validate_message(unsigned char *_data, unsigned int _n, unsigned char _key)
{
    unsigned int i, sum=0;
    for (i=0; i<_n; i++)
        sum += (unsigned int) (_data[i]);
    //sum &= 0x00ff;

    unsigned char check = ((sum&0x00ff)+_key)&0x0ff;
    //printf("check: 0x%0x\n", (unsigned int) check);
    return (check==0);
}


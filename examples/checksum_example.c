//
// checksum_example.c
//
// This example demonstrates how a checksum can be used to validate
// data received through un-reliable means (e.g. a noisy channel).
// A checksum is, in essence, a weak error detection code that simply
// counts the number of ones in a block of data (modulo 256). The
// limitation, however, is that multiple bit errors might result in a
// false positive validation of the corrupted data.  For a much
// much stronger eror detection, use the cyclic redundancy check.
// SEE ALSO: crc_example.c
//           fec_example.c
//

#include <stdio.h>
#include <string.h>

#include "liquid.h"

int main() {
    // initialize data array
    unsigned char data[4] = {0x25, 0x62, 0x3F, 0x52};

    // compute checksum on original data
    unsigned char key = checksum_generate_key(data, 4);
    printf("key: 0x%0x\n", (unsigned int) key);

    // validate original message
    printf("testing uncorrupted data...     ");
    if (checksum_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");

    // check corrupted message
    printf("testing corrupted data...       ");
    data[0] ^= 0x01;
    if (checksum_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");

    // check doubly-corrupted message
    printf("testing doubly-corrupted data...");
    data[1] ^= 0x01;
    if (checksum_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");

    printf("done.\n");
    return 0;
}

//
// crc_example.c
//
// Cyclic redundancy check (CRC) example.  This example demonstrates
// how a CRC can be used to validate data received through un-reliable
// means (e.g. a noisy channel).  A CRC is, in essence, a strong
// algebraic error detection code that computes a key on a block of data
// using base-2 polynomials.
// SEE ALSO: checksum_example.c
//           fec_example.c
//

#include <stdio.h>

#include "liquid.h"

int main() {
    // initial data array
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};

    // compute key on original data
    unsigned int key = crc32_generate_key(data, 4);
    printf("key: 0x%0x\n", (unsigned int) key);

    printf("testing uncorrupted data... ");
    if (crc32_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");

    printf("testing corrupted data...   ");
    data[0]++;
    if (crc32_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");

    printf("done.\n");
    return 0;
}

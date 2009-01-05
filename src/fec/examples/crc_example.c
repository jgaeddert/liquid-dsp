//
//
//

#include <stdio.h>

#include "../src/fec.h"

int main() {
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};
    unsigned int key = crc32_generate_key(data, 4);
    printf("key: 0x%0x\n", (unsigned int) key);

    printf("testing uncorrupted data...\n");
    if (crc32_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");

    printf("testing corrupted data...\n");
    data[0]++;
    if (crc32_validate_message(data, 4, key))
        printf("  data are valid!\n");
    else
        printf("  data are not valid\n");


    printf("done.\n");
    return 0;
}

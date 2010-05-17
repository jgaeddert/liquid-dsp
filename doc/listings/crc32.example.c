#include <liquid/liquid.h>
// ...
{
    // initialize data array
    unsigned char data[4] = {0x25, 0x62, 0x3F, 0x52};

    // compute crc on original data
    unsigned int key = crc32_generate_key(data, 4);

    // ... channel impairments ...

    // validate (received) message
    int valid_data = crc32_validate_message(data, 4, key);
}

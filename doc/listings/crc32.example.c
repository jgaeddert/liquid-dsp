#include <liquid/liquid.h>
// ...
{
    // initialize data array
    unsigned char data[4] = {0x25, 0x62, 0x3F, 0x52};

    // compute CRC on original data
    unsigned char key = crc_generate_key(CRC_32, data, 4);

    // ... channel impairments ...

    // validate (received) message
    int valid_data = crc_validate_message(CRC_32, data, 4, key);
}

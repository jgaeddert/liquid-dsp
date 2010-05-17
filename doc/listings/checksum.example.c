#include <liquid/liquid.h>
// ...
{
    // initialize data array
    unsigned char data[4] = {0x25, 0x62, 0x3F, 0x52};

    // compute checksum on original data
    unsigned char key = checksum_generate_key(data, 4);

    // ... channel impairments ...

    // validate (received) message
    int valid_data = checksum_validate_message(data, 4, key);
}

// file: doc/listings/crc.example.c
#include <liquid/liquid.h>

int main() {
    // initialize data array
    unsigned char data[4] = {0x25, 0x62, 0x3F, 0x52};
    crc_scheme scheme = LIQUID_CRC_32;

    // compute CRC on original data
    unsigned char key = crc_generate_key(scheme, data, 4);

    // ... channel ...

    // validate (received) message
    int valid_data = crc_validate_message(scheme, data, 4, key);
}

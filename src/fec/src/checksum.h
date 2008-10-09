//
// Checksum
//

#include <stdbool.h>

unsigned char checksum_generate_key(unsigned char *_data, unsigned int _n);

bool checksum_validate(unsigned char *_data, unsigned int _n, unsigned char _key);


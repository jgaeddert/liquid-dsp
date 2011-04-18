// file: doc/listings/pack_array.example.c
#include <liquid/liquid.h>

int main() {
    unsigned int sym_size[9] = {8, 2, 3, 6, 1, 3, 3, 4, 3};
    unsigned char input[9] = {
        0x81,   // 1000 0001
        0x03,   //        11
        0x05,   //       101
        0x3a,   //   11 1010
        0x01,   //         1
        0x07,   //       111
        0x06,   //       110
        0x0a,   //      1010
        0x04    //     10[0] <- last bit is stripped
    };

    unsigned char output[4];

    unsigned int k=0;
    unsigned int i;
    for (i=0; i<9; i++) {
        liquid_pack_array(output, 4, k, sym_size[i], input[i]);
        k += sym_size[i];
    }
    // output       : 1000 0001 1110 1111 0101 1111 1010 1010
    // symbol       : 0000 0000 1122 2333 3334 5556 6677 7788
    // output is now {0x81, 0xEF, 0x5F, 0xAA};
}

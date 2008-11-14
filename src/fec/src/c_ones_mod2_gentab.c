//
// script to generate c_ones_mod2 table
//

#include <stdio.h>

// count the number of ones in a byte
unsigned int count_ones(unsigned char);

int main() {

    unsigned char c_ones_mod2[256];

    unsigned int b;
    for (b=0; b<256; b++)
        c_ones_mod2[b] = count_ones((unsigned char)b) % 2;
    
    // print results
    printf("unsigned char c_ones_mod2[] = {\n    ");
    for (b=0; b<256; b++) {
        printf("%1u", c_ones_mod2[b]);
        if (b==255)
            printf("\n");
        else if (((b+1)%16)==0)
            printf(",\n    ");
        else
            printf(", ");
    }
    printf("};\n");
    return 0;
}

// count the number of ones in a byte
unsigned int count_ones(unsigned char _b)
{
    unsigned int i, n=0;
    for (i=0; i<8; i++) {
        n += _b & 0x01;
        _b >>= 1;
    }
    return n;
}

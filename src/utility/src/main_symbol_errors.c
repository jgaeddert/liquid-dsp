//
// Count symbol errors
//

#include <stdio.h>
#include <stdlib.h>

#include "utility.h"

int main() {
    unsigned int x=rand();
    unsigned int mask=0x0000000f; // 4 errors

    unsigned int y = x ^ mask;

    printf("bit errors in symbol: %u\n", count_ones(x^y));
    printf("done.\n");
    return 0;
}

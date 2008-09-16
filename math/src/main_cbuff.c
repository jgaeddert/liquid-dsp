//
//
//

#include <stdio.h>
#include "cbuff.h"

int main() {
    cbuff c = cbuff_create(12);

    cbuff_destroy(c);

    printf("done.\n");
    return 0;
}


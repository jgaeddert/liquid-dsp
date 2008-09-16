//
//
//

#include <stdio.h>

#include "fir_filter.h"

int main() {
    float h[] = {1,2,3,4,5,6,7,8};
    fir_filter f = fir_filter_create(h,8);
    fir_filter_print(f);
    fir_filter_destroy(f);

    printf("done.\n");
    return 0;
}


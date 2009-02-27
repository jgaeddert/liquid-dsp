//
// Test floating-point dot product
//

#include <stdio.h>
#include "liquid.h"

int main() {
    float x[] = {1, 2, 3, 4, 5};
    float y[] = {1, 1, 1, 1, 1};

    float z;
    dotprod_rrrf_run(x,y,5,&z);
    printf("dotprod:  %8.2f\n", z);

    float z4;
    dotprod_rrrf_run4(x,y,5,&z4);
    printf("dotprod4: %8.2f\n", z4);

    printf("---\n");

    dotprod_rrrf q = dotprod_rrrf_create(x,5);
    float zq;
    dotprod_rrrf_execute(q,y,&zq);
    printf("dotprodq: %8.2f\n", zq);
    dotprod_rrrf_destroy(q);
    return 0;
}



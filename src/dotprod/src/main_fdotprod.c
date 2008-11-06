//
// Test floating-point dot product
//

#include <stdio.h>
#include "dotprod.h"

int main() {
    float x[] = {1, 2, 3, 4, 5};
    float y[] = {1, 1, 1, 1, 1};

    float z = fdotprod_run(x,y,5);
    printf("fdotprod:  %8.2f\n", z);

    float z4 = fdotprod_run4(x,y,5);
    printf("fdotprod4: %8.2f\n", z4);

    printf("---\n");

    fdotprod q = fdotprod_create(x,5);
    float zq = fdotprod_execute(q,y);
    printf("fdotprodq: %8.2f\n", zq);
    fdotprod_destroy(q);
    return 0;
}



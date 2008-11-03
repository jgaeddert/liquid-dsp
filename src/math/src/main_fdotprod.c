//
// Test floating-point dot product
//

#include <stdio.h>
#include "dotprod.h"

int main() {
    float x[] = {1, 2, 3, 4, 5};
    float y[] = {1, 1, 1, 1, 1};

    float z = fdotprod(x,y,5);
    printf("fdotprod:  %8.2f\n", z);

    float z4 = fdotprod4(x,y,5);
    printf("fdotprod4: %8.2f\n", z4);
    return 0;
}



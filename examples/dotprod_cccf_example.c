//
// Test floating-point dot product
//

#include <stdio.h>
#include "liquid.h"

int main() {
    float complex x[] = {1, 2, 3, 4, 5};
    float complex y[] = {1, 1, 1, 1, 1};

    float complex z;
    dotprod_cccf_run(x,y,5,&z);
    printf("fdotprod:  %8.2f + j%8.2f\n", crealf(z), cimagf(z));

    float complex z4;
    dotprod_cccf_run4(x,y,5,&z4);
    printf("fdotprod:  %8.2f + j%8.2f\n", crealf(z4), cimagf(z4));
    return 0;
}



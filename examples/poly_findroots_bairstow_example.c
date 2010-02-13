// 
// poly_findroots_bairstow_example.c
//
// test polynomial root-finding algorithm (Bairstow's method)
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

int main() {
    float p[6] = {6,11,-33,-33,11,6};
    float u, v;

    fpoly_findroots_bairstow_recursion(p,6,&u,&v);
    printf("u : %12.8f\n", u);
    printf("v : %12.8f\n", v);

    printf("done.\n");
    return 0;
}

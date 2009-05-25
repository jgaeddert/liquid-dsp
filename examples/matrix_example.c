// 
// Test fmatrix ops
//

#include <stdio.h>
#include "liquid.h"

int main() {

    float x[6] = {
        1, 2, 3,
        4, 5, 6};

    float y[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9};

    float z[6];

    // compute z = x * y
    printf("z = x * y :\n");
    fmatrix_mul(x,2,3,y,3,3,z,2,3);
    fmatrix_print(z,2,3);

    /*
    // compute z = y * x'
    fmatrix_transpose(x);
    printf("x' : \n");
    fmatrix_print(x);
    fmatrix_transpose(z);
    fmatrix_multiply(y,x,z);
    printf("z = y * x' :\n");
    fmatrix_print(z);

    fmatrix_destroy(x);
    fmatrix_destroy(y);
    fmatrix_destroy(z);
    */

    fmatrix_inv(y,3,3);
    fmatrix_print(y,3,3);

    printf("done.\n");
    return 0;
}


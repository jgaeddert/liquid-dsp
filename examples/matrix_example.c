// 
// Test fmatrix ops
//

#include <stdio.h>
#include "liquid.h"

int main() {

    fmatrix x = fmatrix_create(2,3);
    fmatrix_assign(x, 0, 0, 1.0f);
    fmatrix_assign(x, 0, 1, 2.0f);
    fmatrix_assign(x, 0, 2, 3.0f);
    fmatrix_assign(x, 1, 0, 4.0f);
    fmatrix_assign(x, 1, 1, 5.0f);
    fmatrix_assign(x, 1, 2, 6.0f);
    fmatrix_print(x);

    fmatrix y = fmatrix_create(3,3);
    fmatrix_assign(y, 0, 0, 1.0f);
    fmatrix_assign(y, 0, 1, 2.0f);
    fmatrix_assign(y, 0, 2, 3.0f);
    fmatrix_assign(y, 1, 0, 4.0f);
    fmatrix_assign(y, 1, 1, 5.0f);
    fmatrix_assign(y, 1, 2, 6.0f);
    fmatrix_assign(y, 2, 0, 7.0f);
    fmatrix_assign(y, 2, 1, 8.0f);
    fmatrix_assign(y, 2, 2, 9.0f);
    fmatrix_print(y);

    fmatrix z = fmatrix_create(2,3);

    // compute z = x * y
    printf("z = x * y :\n");
    fmatrix_multiply(x,y,z);
    fmatrix_print(z);

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

    printf("done.\n");
    return 0;
}


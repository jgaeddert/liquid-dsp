// 
// Test cfmatrix ops
//

#include <stdio.h>
#include "matrix.h"

int main() {

    cfmatrix x = cfmatrix_create(2,3);
    cfmatrix_assign(x, 0, 0, 1.0f + 1.0f*_Complex_I);
    cfmatrix_assign(x, 0, 1, 2.0f + 2.0f*_Complex_I);
    cfmatrix_assign(x, 0, 2, 3.0f + 3.0f*_Complex_I);
    cfmatrix_assign(x, 1, 0, 4.0f + 4.0f*_Complex_I);
    cfmatrix_assign(x, 1, 1, 5.0f + 5.0f*_Complex_I);
    cfmatrix_assign(x, 1, 2, 6.0f + 6.0f*_Complex_I);
    cfmatrix_print(x);

    cfmatrix y = cfmatrix_create(3,3);
    cfmatrix_assign(y, 0, 0, 1.0f + 1.0f*_Complex_I);
    cfmatrix_assign(y, 0, 1, 2.0f + 2.0f*_Complex_I);
    cfmatrix_assign(y, 0, 2, 3.0f + 3.0f*_Complex_I);
    cfmatrix_assign(y, 1, 0, 4.0f + 4.0f*_Complex_I);
    cfmatrix_assign(y, 1, 1, 5.0f + 5.0f*_Complex_I);
    cfmatrix_assign(y, 1, 2, 6.0f + 6.0f*_Complex_I);
    cfmatrix_assign(y, 2, 0, 7.0f + 7.0f*_Complex_I);
    cfmatrix_assign(y, 2, 1, 8.0f + 8.0f*_Complex_I);
    cfmatrix_assign(y, 2, 2, 9.0f + 9.0f*_Complex_I);
    cfmatrix_print(y);

    cfmatrix z = cfmatrix_create(2,3);

    // compute z = x * y
    printf("z = x * y :\n");
    cfmatrix_multiply(x,y,z);
    cfmatrix_print(z);

    // compute z = y * x'
    cfmatrix_transpose(x);
    printf("x' : \n");
    cfmatrix_print(x);
    cfmatrix_transpose(z);
    cfmatrix_multiply(y,x,z);
    printf("z = y * x' :\n");
    cfmatrix_print(z);

    cfmatrix_destroy(x);
    cfmatrix_destroy(y);
    cfmatrix_destroy(z);

    printf("done.\n");
    return 0;
}


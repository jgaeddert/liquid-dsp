// 
// Test matrix ops
//

#include <stdio.h>
#include "matrix.h"

int main() {

    matrix x = matrix_create(2,3);
    matrix_assign(x, 0, 0, 1.0f);
    matrix_assign(x, 0, 1, 2.0f);
    matrix_assign(x, 0, 2, 3.0f);
    matrix_assign(x, 1, 0, 4.0f);
    matrix_assign(x, 1, 1, 5.0f);
    matrix_assign(x, 1, 2, 6.0f);
    matrix_print(x);

    matrix y = matrix_create(3,3);
    matrix_assign(y, 0, 0, 1.0f);
    matrix_assign(y, 0, 1, 2.0f);
    matrix_assign(y, 0, 2, 3.0f);
    matrix_assign(y, 1, 0, 4.0f);
    matrix_assign(y, 1, 1, 5.0f);
    matrix_assign(y, 1, 2, 6.0f);
    matrix_assign(y, 2, 0, 7.0f);
    matrix_assign(y, 2, 1, 8.0f);
    matrix_assign(y, 2, 2, 9.0f);
    matrix_print(y);

    matrix z = matrix_create(2,3);

    // compute z = x * y
    printf("z = x * y :\n");
    matrix_multiply(x,y,z);
    matrix_print(z);

    // compute z = y * x'
    matrix_transpose(x);
    printf("x' : \n");
    matrix_print(x);
    matrix_transpose(z);
    matrix_multiply(y,x,z);
    printf("z = y * x' :\n");
    matrix_print(z);

    matrix_destroy(x);
    matrix_destroy(y);
    matrix_destroy(z);

    printf("done.\n");
    return 0;
}


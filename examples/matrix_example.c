// 
// Test fmatrix ops
//

#include <stdio.h>
#include <string.h>
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

    float s[16] = {
        1,2,3,4,
        5,5,7,8,
        6,4,8,7,
        1,0,3,1};
    float s_inv[16];
    memmove(s_inv,s,16*sizeof(float));
    fmatrix_inv(s_inv,4,4);

    float i4[16];
    fmatrix_mul(s,4,4,s_inv,4,4,i4,4,4);

    printf("s:\n");
    fmatrix_print(s,4,4);
    printf("inv(s):\n");
    fmatrix_print(s_inv,4,4);
    printf("s*inv(s):\n");
    fmatrix_print(i4,4,4);

#if 0
    // pivot test (matrix inversion)
    float t[32] = {
        1,2,3,4,  1,0,0,0,
        5,5,7,8,  0,1,0,0,
        6,4,8,7,  0,0,1,0,
        1,0,3,1,  0,0,0,1};

    unsigned int i;
    for (i=0; i<4; i++) {
        fmatrix_pivot(t,4,8,i,i);
        fmatrix_print(t,4,8);
    }

    unsigned int j;
    for (i=0; i<4; i++) {
        float v = matrix_access(t,4,8,i,i);
        for (j=0; j<8; j++)
            matrix_access(t,4,8,i,j) /= v;
    }
    fmatrix_print(t,4,8);
#endif

    printf("testing L/U decomposition [Crout's method]\n");
    float A[4] = {
        4,3,
        6,3};
    float L[4], U[4], P[4];
    fmatrix_ludecomp_crout(A,2,2,L,U,P);
    fmatrix_print(L,2,2);
    fmatrix_print(U,2,2);

    printf("testing L/U decomposition [Doolittle's method]\n");
    fmatrix_ludecomp_doolittle(A,2,2,L,U,P);
    fmatrix_print(L,2,2);
    fmatrix_print(U,2,2);

    printf("done.\n");
    return 0;
}


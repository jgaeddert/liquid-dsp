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

    printf("\ns:\n");
    fmatrix_print(s,4,4);
    printf("\ninv(s):\n");
    fmatrix_print(s_inv,4,4);
    printf("\ns*inv(s):\n");
    fmatrix_print(i4,4,4);

    printf("\n");
    float det = fmatrix_det(s,4);
    printf("det(s) = %12.8f\n", det);

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

    printf("\n");
    printf("testing L/U decomposition [Crout's method]\n");
    float L[16], U[16], P[16];
    fmatrix_ludecomp_crout(s,4,4,L,U,P);
    fmatrix_print(L,4,4);
    fmatrix_print(U,4,4);

    printf("\n");
    printf("testing L/U decomposition [Doolittle's method]\n");
    fmatrix_ludecomp_doolittle(s,4,4,L,U,P);
    fmatrix_print(L,4,4);
    fmatrix_print(U,4,4);

    printf("\n\n");
    float X[16] = {
       0.84382,  -2.38304,   1.43061,  -1.66604,
       3.99475,   0.88066,   4.69373,   0.44563,
       7.28072,  -2.06608,   0.67074,   9.80657,
       6.07741,  -3.93099,   1.22826,  -0.42142};
    float Y[16];
    printf("\nX:\n");
    fmatrix_print(X,4,4);

    // swaprows
    memmove(Y,X,16*sizeof(float));
    fmatrix_swaprows(Y,4,4,0,2);
    printf("\nfmatrix_swaprows(X,4,4,0,2):\n");
    fmatrix_print(Y,4,4);

    // pivot test
    memmove(Y,X,16*sizeof(float));
    fmatrix_pivot(Y,4,4,1,2);
    printf("\nfmatrix_pivot(X,4,4,1,2):\n");
    fmatrix_print(Y,4,4);

    // inverse test
    memmove(Y,X,16*sizeof(float));
    fmatrix_inv(Y,4,4);
    printf("\nfmatrix_inv(X,4,4):\n");
    fmatrix_print(Y,4,4);

    // determinant test
    float D = fmatrix_det(X,4);
    printf("\nfmatrix_det(X,4) = %12.8f\n", D);

    // L/U decomp (Crout's method)
    fmatrix_ludecomp_crout(X,4,4,L,U,P);
    printf("\nfmatrix_ludecomp_crout(X,4,4,L,U,P)\n");
    printf("L:\n");
    fmatrix_print(L,4,4);
    printf("U:\n");
    fmatrix_print(U,4,4);

    // L/U decomp (Doolittle's method)
    fmatrix_ludecomp_doolittle(X,4,4,L,U,P);
    printf("\nfmatrix_ludecomp_doolittle(X,4,4,L,U,P)\n");
    printf("L:\n");
    fmatrix_print(L,4,4);
    printf("U:\n");
    fmatrix_print(U,4,4);

    /*
    float b[4] = {
       0.91489,
       0.71789,
       1.06553,
      -0.81707};
    */

    float Xb[20] = {
       0.84382,  -2.38304,   1.43061,  -1.66604,   0.91489,
       3.99475,   0.88066,   4.69373,   0.44563,   0.71789,
       7.28072,  -2.06608,   0.67074,   9.80657,   1.06553,
       6.07741,  -3.93099,   1.22826,  -0.42142,  -0.81707};
    printf("\n[X b] =\n");
    fmatrix_print(Xb,4,5);

    fmatrix_gjelim(Xb,4,5);
    printf("\nfmatrix_gjelim(Xb,4,5)\n");
    fmatrix_print(Xb,4,5);


    printf("done.\n");
    return 0;
}


// 
// Test polynomial fit
//

#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "liquid.h"

int main() {

    unsigned int n=5;       // number of samples
    unsigned int order=2;   // polynomial order
    float x[6] = {-2, -1, 0, 1, 2};
    float y[6] = { 4,  1, 0, 1, 4};

    // derived values
    unsigned int k=order+1;

    // ...
    float X[n*k];
    unsigned int r,c;
    float v;
    for (r=0; r<n; r++) {
        v = 1;
        for (c=0; c<k; c++) {
            matrix_access(X,n,k,r,c) = v;
            v *= x[r];
        }
    }

    // compute transpose of X
    float Xt[k*n];
    memmove(Xt,X,k*n*sizeof(float));
    fmatrix_trans(Xt,n,k);

    printf("\nX : \n");
    fmatrix_print(X,n,k);

    printf("\n[X'] : \n");
    fmatrix_print(Xt,k,n);

    // compute [X']*y
    float Xty[k];
    fmatrix_mul(Xt,k,n,y,n,1,Xty,k,1);
    printf("\n[X']*y : \n");
    fmatrix_print(Xty,k,1);

    // compute [X']*X
    float X2[k*k];
    fmatrix_mul(Xt,k,n,X,n,k,X2,k,k);
    printf("\n[X']*X : \n");
    fmatrix_print(X2,k,k);

    // compute inv([X']*X)
    float G[k*k];
    memmove(G,X2,k*k*sizeof(float));
    fmatrix_inv(G,k,k);
    fmatrix_print(G,k,k);

    // compute coefficients
    float a[k];
    fmatrix_mul(G,k,k,Xty,k,1,a,k,1);

    unsigned int i;
    for (i=0; i<k; i++)
        printf("a[%3u] = %12.8f\n", i, a[i]);

    return 0;

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

    printf("done.\n");
    return 0;
}


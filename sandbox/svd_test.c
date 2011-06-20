// 
// svd_test.c : test singular value decomposition
//

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

// tri-diagonalization
void matrixf_householder(float * _A,
                         unsigned int _m,
                         unsigned int _n,
                         float * _P,
                         float * _Q)
{
    // TODO : validate input

    unsigned int i;
    unsigned int j;

    // arrays
    float v[_n];
    float A0[_m*_n];
    float A1[_m*_n];
    float alpha;
    float r;

    // initialize P, Q
    
    // compute alpha
    alpha = -copysignf(1.0, matrix_access(_A,_m,_n,1,0));
    // ...
}

void matrixf_svd(float * _A,
                 unsigned int _m,
                 unsigned int _n,
                 float * _U,
                 float * _V,
                 float * _sigma)
{
    printf("matrixf_svd() not yet implemented\n");
}

int main() {
    // problem definition
    //  sig1    =   sqrt(1248)
    //  sig2    =   20
    //  sig3    =   sqrt(384)
    //  sig4    =   0
    //  sig5    =   0
    float A[40] = {
        22,  10,   2,   3,  7,
        14,   7,  10,   0,  8,
        -1,  13,  -1, -11,  3,
        -3,  -2,  13,  -2,  4,
         9,   8,   1,  -2,  4,
         9,   1,  -7,   5, -1,
         2,  -6,   6,   5,  1,
         4,   5,   0,  -2,  2};

    unsigned int m = 8;
    unsigned int n = 5;

    matrixf_print(A,m,n);

    printf("done.\n");
    return 0;
}


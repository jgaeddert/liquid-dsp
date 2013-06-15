/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/matrix/tests/matrix_data.h"

// test matrix addition
void autotest_matrixq16_add()
{
    float tol = 0.008f;

    // convert floating-point precision to fixed
    q16_t x[20];
    q16_t y[20];
    q16_t z[20];
    q16_memmove_float_to_fixed(x, matrixf_data_add_x, 20);
    q16_memmove_float_to_fixed(y, matrixf_data_add_y, 20);

    matrixq16_add(x,y,z,5,4);

    unsigned int i;
    for (i=0; i<20; i++)
        CONTEND_DELTA( q16_fixed_to_float(z[i]), matrixf_data_add_z[i], tol );
}

// test matrix augmentation
void autotest_matrixq16_aug()
{
    float tol = 0.008f;

    // x [size: 5 x 4]
    // y [size: 5 x 3]
    // z [size: 5 x 7]
    q16_t x[20];
    q16_t y[15];
    q16_t z[35];
    q16_memmove_float_to_fixed(x, matrixf_data_aug_x, 20);
    q16_memmove_float_to_fixed(y, matrixf_data_aug_y, 15);
    matrixq16_aug(x, 5, 4,
                y, 5, 3,
                z, 5, 7);
    
    // print result
    if (liquid_autotest_verbose) {
        printf("augment:\n");
        printf("  x: ");        matrixq16_print(x,5,4);
        printf("  y: ");        matrixq16_print(y,5,3);
        printf("  z: ");        matrixq16_print(z,5,3);
        printf("  expected: "); matrixf_print(matrixf_data_aug_z,5,7);
    }

    unsigned int i;
    for (i=0; i<35; i++)
        CONTEND_DELTA( q16_fixed_to_float(z[i]), matrixf_data_aug_z[i], tol );
}

// conjugate gradient solver
void autotest_matrixq16_cgsolve()
{
    AUTOTEST_WARN("matrixq16_cgsolve() not yet implemented");
    return;

    float tol = 0.01;  // error tolerance

    // A [size: 8 x 8], symmetric positive definite matrx
    // x [size: 8 x 1]
    // b [size: 8 x 1]
    q16_t A[64];
    q16_t x[8];
    q16_t b[8];
    q16_memmove_float_to_fixed(A, matrixf_data_cgsolve_A, 64);
    q16_memmove_float_to_fixed(b, matrixf_data_cgsolve_b, 8);
#if 0
    matrixq16_cgsolve(A, 8,
                      b,
                      x, NULL);
#endif

    if (liquid_autotest_verbose) {
        printf("cgsolve:\n");
        printf("  A: ");        matrixq16_print(A, 8, 8);
        printf("  b: ");        matrixq16_print(b, 8, 1);
        printf("  x: ");        matrixq16_print(x, 8, 1);
        printf("  expected: "); matrixf_print(matrixf_data_cgsolve_x, 8, 1);
    }

    unsigned int i;
    for (i=0; i<8; i++)
        CONTEND_DELTA( q16_fixed_to_float(x[i]), matrixf_data_cgsolve_x[i], tol );
}

// Cholesky decomposition
void autotest_matrixq16_chol()
{
    AUTOTEST_WARN("matrixq16_chol() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    // A [size: 4 x 4]
    // L [size: 4 x 4]
    q16_t A[16];
    q16_t L[16];
    q16_memmove_float_to_fixed(A, matrixf_data_chol_A, 16);

    // run decomposition
    //matrixq16_chol(A, 4, L);

    if (liquid_autotest_verbose) {
        printf("chol:\n");
        printf("  A: ");        matrixq16_print(A, 4, 4);
        printf("  L: ");        matrixq16_print(L, 4, 4);
        printf("  expected: "); matrixf_print(matrixf_data_chol_L, 4, 4);
    }

    unsigned int i;
    for (i=0; i<16; i++)
        CONTEND_DELTA( q16_fixed_to_float(L[i]), matrixf_data_chol_L[i], tol );
}

// Gram-Schmidt Orthonormalization
void autotest_matrixq16_gramschmidt()
{
    AUTOTEST_WARN("matrixq16_gramschmidt() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    // A [size: 4 x 3]
    // V [size: 4 x 3]
    q16_t A[12];
    q16_t V[12];
    q16_memmove_float_to_fixed(A, matrixf_data_gramschmidt_A, 12);
    //matrixq16_gramschmidt(A, 4, 3, V);

    if (liquid_autotest_verbose) {
        printf("gramschmidt:\n");
        printf("  A: ");        matrixq16_print(A, 4, 3);
        printf("  V: ");        matrixq16_print(V, 4, 3);
        printf("  expected: "); matrixf_print(matrixf_data_gramschmidt_V, 4, 3);
    }

    unsigned int i;
    for (i=0; i<12; i++)
        CONTEND_DELTA( q16_fixed_to_float(V[i]), matrixf_data_gramschmidt_V[i], tol );
}


// matrix inversion
void autotest_matrixq16_inv()
{
    AUTOTEST_WARN("matrixq16_inv() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    // x [size: 5 x 5]
    // y [size: 5 x 5]
    q16_t x[25];
    q16_t y[25];
    q16_memmove_float_to_fixed(x, matrixf_data_inv_x, 25);
    memmove(y, x, 5*5*sizeof(q16_t));
    matrixq16_inv(y, 5, 5);

    if (liquid_autotest_verbose) {
        printf("inv:\n");
        printf("  x: ");        matrixq16_print(x, 5, 5);
        printf("  y: ");        matrixq16_print(y, 5, 5);
        printf("  expected: "); matrixf_print(matrixf_data_inv_y, 5, 5);
    }

    unsigned int i;
    for (i=0; i<25; i++)
        CONTEND_DELTA( q16_fixed_to_float(y[i]), matrixf_data_inv_y[i], tol );
}

// linsolve (solve linear system of equations)
void autotest_matrixq16_linsolve()
{
    AUTOTEST_WARN("matrixq16_linsolve() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    // A [size: 5 x 5]
    // x [size: 5 x 1]
    // b [size: 5 x 1]
    q16_t A[25];
    q16_t b[5];
    q16_t x[5];
    q16_memmove_float_to_fixed(A, matrixf_data_linsolve_A, 25);
    q16_memmove_float_to_fixed(b, matrixf_data_linsolve_b,  5);
    
    // run solver
    //matrixq16_linsolve(A, 5, b, x, NULL);

    if (liquid_autotest_verbose) {
        printf("linsolve:\n");
        printf("  A: ");        matrixq16_print(A, 5, 5);
        printf("  b: ");        matrixq16_print(b, 5, 1);
        printf("  x: ");        matrixq16_print(x, 5, 1);
        printf("  expected: "); matrixf_print(matrixf_data_linsolve_x, 5, 1);
    }

    unsigned int i;
    for (i=0; i<5; i++)
        CONTEND_DELTA( q16_fixed_to_float(x[i]), matrixf_data_linsolve_x[i], tol );
}


// L/U decomp (Crout)
void autotest_matrixq16_ludecomp_crout()
{
    AUTOTEST_WARN("matrixq16_ludecomp_crout() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    q16_t A[64];        // [size: 8 x 8]
    q16_t L[64];        // [size: 8 x 8]
    q16_t U[64];        // [size: 8 x 8]
    q16_t P[64];        // [size: 8 x 8]
    q16_t LU_test[64];  // [size: 8 x 8]

    // run decomposition
    q16_memmove_float_to_fixed(A, matrixf_data_ludecomp_A, 64);
    //matrixq16_ludecomp_crout(matrixf_data_ludecomp_A, 8, 8, L, U, P);

    // multiply LU
    matrixq16_mul(L,       8, 8,
                  U,       8, 8,
                  LU_test, 8, 8);

    if (liquid_autotest_verbose) {
        printf("ludecomp_crout:\n");
        printf("  A: ");        matrixq16_print(A,       8,8);
        printf("  L: ");        matrixq16_print(L,       8,8);
        printf("  U: ");        matrixq16_print(U,       8,8);
        printf("  LU: ");       matrixq16_print(LU_test, 8,8);
    }

    unsigned int r,c;
    for (r=0; r<8; r++) {
        for (c=0; c<8; c++) {
            if (r < c) {
                CONTEND_DELTA( q16_fixed_to_float(matrix_access(L,8,8,r,c)), 0.0f, tol );
            } else if (r==c) {
                CONTEND_DELTA( q16_fixed_to_float(matrix_access(U,8,8,r,c)), 1.0f, tol );
            } else {
                CONTEND_DELTA( q16_fixed_to_float(matrix_access(U,8,8,r,c)), 0.0f, tol );
            }
        }
    }

    unsigned int i;
    for (i=0; i<64; i++)
        CONTEND_DELTA( q16_fixed_to_float(LU_test[i]), matrixf_data_ludecomp_A[i], tol );
}

// L/U decomp (Doolittle)
void autotest_matrixq16_ludecomp_doolittle()
{
    AUTOTEST_WARN("matrixq16_ludecomp_doolittle() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    q16_t A[64];        // [size: 8 x 8]
    q16_t L[64];        // [size: 8 x 8]
    q16_t U[64];        // [size: 8 x 8]
    q16_t P[64];        // [size: 8 x 8]
    q16_t LU_test[64];  // [size: 8 x 8]

    // run decomposition
    q16_memmove_float_to_fixed(A, matrixf_data_ludecomp_A, 64);
    //matrixq16_ludecomp_doolittle(matrixf_data_ludecomp_A, 8, 8, L, U, P);

    // multiply LU
    matrixq16_mul(L,       8, 8,
                  U,       8, 8,
                  LU_test, 8, 8);

    if (liquid_autotest_verbose) {
        printf("ludecomp_doolittle:\n");
        printf("  A: ");        matrixq16_print(A,       8,8);
        printf("  L: ");        matrixq16_print(L,       8,8);
        printf("  U: ");        matrixq16_print(U,       8,8);
        printf("  LU: ");       matrixq16_print(LU_test, 8,8);
    }

    unsigned int r,c;
    for (r=0; r<8; r++) {
        for (c=0; c<8; c++) {
            if (r < c) {
                CONTEND_DELTA( q16_fixed_to_float(matrix_access(L,8,8,r,c)), 0.0f, tol );
            } else if (r==c) {
                CONTEND_DELTA( q16_fixed_to_float(matrix_access(L,8,8,r,c)), 1.0f, tol );
            } else {
                CONTEND_DELTA( q16_fixed_to_float(matrix_access(U,8,8,r,c)), 0.0f, tol );
            }
        }
    }

    unsigned int i;
    for (i=0; i<64; i++)
        CONTEND_DELTA( q16_fixed_to_float(LU_test[i]), matrixf_data_ludecomp_A[i], tol );
}

// test matrix multiplication
void autotest_matrixq16_mul()
{
    float tol = 0.01f;

    // x [size: 5 x 4]
    // y [size: 4 x 3]
    // z [size: 5 x 3]
    q16_t x[20];
    q16_t y[12];
    q16_t z[15];
    q16_memmove_float_to_fixed(x, matrixf_data_mul_x, 20);
    q16_memmove_float_to_fixed(y, matrixf_data_mul_y, 12);

    // compute multiplication
    matrixq16_mul(x, 5, 4,
                  y, 4, 3,
                  z, 5, 3);

    // print result
    if (liquid_autotest_verbose) {
        printf("multiplication:\n");
        printf("  x: ");        matrixq16_print(x,5,4);
        printf("  y: ");        matrixq16_print(y,4,3);
        printf("  z: ");        matrixq16_print(z,5,3);
        printf("  expected: "); matrixf_print(matrixf_data_mul_z,5,3);
    }

    unsigned int i;
    for (i=0; i<15; i++)
        CONTEND_DELTA( q16_fixed_to_float(z[i]), matrixf_data_mul_z[i], tol );
}

// Q/R decomp (Gram-Schmidt)
void autotest_matrixq16_qrdecomp()
{
    AUTOTEST_WARN("matrixq16_qrdecomp() not yet implemented");
    return;

    float tol = 0.01f;  // error tolerance

    q16_t A[16];        // Q,     [size: 4 x 4]
    q16_t Q[16];        // Q,     [size: 4 x 4]
    q16_t R[16];        // R,     [size: 4 x 4]
    q16_t QR_test[16];  // Q*R,   [size: 4 x 4]
    q16_t QQT_test[16]; // Q*Q^T, [size: 4 x 4]

    q16_memmove_float_to_fixed(Q, matrixf_data_qrdecomp_Q, 16);

    // run decomposition
    //matrixq16_qrdecomp_gramschmidt(A, 4, 4, Q, R);

    // compute Q*R
    matrixq16_mul(Q,       4, 4,
                  R,       4, 4,
                  QR_test, 4, 4);

    // compute Q*Q^T
    matrixq16_mul_transpose(Q, 4, 4, QQT_test);

    if (liquid_autotest_verbose) {
        printf("qrdecomp_gramschmidt:\n");
        printf("  A: ");          matrixq16_print(A,        4,4);
        printf("  Q: ");          matrixq16_print(Q,        4,4);
        printf("  R: ");          matrixq16_print(R,        4,4);
        printf("  QR: ");         matrixq16_print(QR_test,  4,4);
        printf("  QQ: ");         matrixq16_print(QQT_test, 4,4);
        printf("  Q expected: "); matrixf_print(matrixf_data_qrdecomp_Q,4,4);
        printf("  R expected: "); matrixf_print(matrixf_data_qrdecomp_R,4,4);
    }

    unsigned int i;

    // ensure Q*R = A
    for (i=0; i<16; i++)
        CONTEND_DELTA( q16_fixed_to_float(QR_test[i]), matrixf_data_qrdecomp_A[i], tol );

    // ensure Q*Q = I(4)
    float I4[16];
    matrixf_eye(I4,4);
    for (i=0; i<16; i++)
        CONTEND_DELTA( q16_fixed_to_float(QQT_test[i]), I4[i], tol );

    // ensure Q and R are correct
    for (i=0; i<16; i++) {
        CONTEND_DELTA( q16_fixed_to_float(Q[i]), matrixf_data_qrdecomp_Q[i], tol );
        CONTEND_DELTA( q16_fixed_to_float(R[i]), matrixf_data_qrdecomp_R[i], tol );
    }
}

// transpose/multiply
void autotest_matrixq16_transmul()
{
    float tol = 0.02;   // error tolerance

    q16_t x[20];        // [size: 5 x 4]
    q16_t xxT[25];      // [size: 5 x 5]
    q16_t xxH[25];      // [size: 5 x 5]
    q16_t xTx[16];      // [size: 4 x 4]
    q16_t xHx[16];      // [size: 4 x 4]

    q16_memmove_float_to_fixed(x, matrixf_data_transmul_x, 20);

    // run matrix multiplications
    matrixq16_mul_transpose(x, 5, 4, xxT);
    matrixq16_mul_hermitian(x, 5, 4, xxH);
    matrixq16_transpose_mul(x, 5, 4, xTx);
    matrixq16_hermitian_mul(x, 5, 4, xHx);

    if (liquid_autotest_verbose) {
        printf("transmul:\n");
        printf("  x: ");            matrixq16_print(x,  5,4);
        printf("\n");
        printf("  xxT: ");          matrixq16_print(xxT,                    5,5);
        printf("  xxT expected: "); matrixf_print(matrixf_data_transmul_xxT,5,5);
        printf("\n");
        printf("  xxH: ");          matrixq16_print(xxH,                    5,5);
        printf("  xxH expected: "); matrixf_print(matrixf_data_transmul_xxH,5,5);
        printf("\n");
        printf("  xTx: ");          matrixq16_print(xTx,                    4,4);
        printf("  xTx expected: "); matrixf_print(matrixf_data_transmul_xTx,4,4);
        printf("\n");
        printf("  xHx: ");          matrixq16_print(xHx,                    4,4);
        printf("  xHx expected: "); matrixf_print(matrixf_data_transmul_xHx,4,4);
        printf("\n");
    }

    // run tests
    unsigned int i;

    for (i=0; i<25; i++)
        CONTEND_DELTA( matrixf_data_transmul_xxT[i], q16_fixed_to_float(xxT[i]), tol);

    for (i=0; i<25; i++)
        CONTEND_DELTA( matrixf_data_transmul_xxH[i], q16_fixed_to_float(xxH[i]), tol);

    for (i=0; i<16; i++)
        CONTEND_DELTA( matrixf_data_transmul_xTx[i], q16_fixed_to_float(xTx[i]), tol);

    for (i=0; i<16; i++)
        CONTEND_DELTA( matrixf_data_transmul_xHx[i], q16_fixed_to_float(xHx[i]), tol);
}




//
// golay_test.c
//
// Test soft decoding of the Golay(24,12) code using the sum-product
// algorithm (see sandbox/ldpc_test.c)
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "liquid.internal.h"

int main(int argc, char*argv[])
{
    // initialize random seed
    srand(time(NULL));

    // noise standard deviation
    float sigma = 0.8f;

    unsigned int m = 12;    // rows in H
    unsigned int n = 24;    // cols in H

    // generator matrix [12 x 24]
    unsigned char G[288] = {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0,   1,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1,   1,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0,   1,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0,   1,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0,   1,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,  0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,   1,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,  1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1,   1,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,  1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1,   1,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,  1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0,   1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,  0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,   1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1,   1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  
                                             1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   0};

    // parity check matrix [12 x 24]
    unsigned char H[288] = {
        1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1,  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1,  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1,  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1,  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1,  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1,  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
        1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1,  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

    unsigned int i;
    unsigned int j;

    // original message signal
    unsigned char x[m];
    for (i=0; i<m; i++)
        x[i] = rand() % 2;

    // transmitted codeword
#if 0
    unsigned char c[24] = {
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1};
#else
    unsigned char c[24];
    // compute encoded message
    for (i=0; i<n; i++) {
        c[i] = 0;
        for (j=0; j<m; j++)
            c[i] += G[j*n+i] * x[j];
        c[i] %= 2;
    }
#endif

    // print generator check matrix
    printf("G =\n");
    for (j=0; j<m; j++) {
        for (i=0; i<n; i++) {
            printf("%3u", G[j*n+i]);
        }
        printf("\n");
    }
    // print parity check matrix
    printf("H =\n");
    for (j=0; j<m; j++) {
        for (i=0; i<n; i++) {
            printf("%3u", H[j*n+i]);
        }
        printf("\n");
    }
    // print inputs
    printf("x = [");
    for (i=0; i<m; i++)
        printf("%2u", x[i]);
    printf(" ]\n");
    
    // received message
    float y[n];
    for (i=0; i<n; i++)
        y[i] = c[i] ? -1.0f : 1.0f;

    // add noise and compute LLR
    float LLR[n];
    for (i=0; i<n; i++)
        LLR[i] = 2.0f*(y[i] + sigma*randnf()) / (sigma*sigma);

    // print intial transmitted/received codewords
    printf("c    :");
    for (i=0; i<n; i++)
        printf(" %1u", c[i]);
    printf("\n");

    printf("c_hat: ");
    for (i=0; i<n; i++)
        printf("%1u%c", LLR[i]<0?1:0, (LLR[i]<0?1:0)==c[i] ? ' ' : '*' );
    printf("\n");

    unsigned int max_iterations = 30;
    unsigned char c_hat[n];

    // run internal sum-product algorithm
    int parity_pass = fec_sumproduct(H, m, n, LLR, c_hat, max_iterations);

    // compute errors and print results
    unsigned int num_errors = 0;
    for (i=0; i<n; i++)
        num_errors += (c[i] == c_hat[i]) ? 0 : 1;

    printf("\nresults:\n");

    printf("c    :");
    for (i=0; i<n; i++)
        printf(" %1u", c[i]);
    printf("\n");

    printf("c_hat:");
    for (i=0; i<n; i++)
        printf(" %1u", c_hat[i]);
    printf("\n");

    printf("parity : %s\n", parity_pass ? "pass" : "FAIL");
    printf("num errors: %u / %u\n", num_errors, n);

    // decoded original signal (first 12 bits of c_hat)
    unsigned char x_hat[m];
    for (i=0; i<m; i++)
        x_hat[i] = c_hat[i];

    num_errors = 0;
    for (i=0; i<m; i++)
        num_errors += (x[i] == x_hat[i]) ? 0 : 1;
    printf("num errors: %u / %u\n", num_errors, m);

    return 0;
}


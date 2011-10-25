//
// smatrix_example.c
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <math.h>

#include "liquid.internal.h"

int main(int argc, char*argv[]) {
    unsigned int M = 12;
    unsigned int N = 16;

    // create empty list
    smatrix q = smatrix_create(M,N);

    // set value(s)
    smatrix_set(q,1,2);
    smatrix_set(q,2,2);
    smatrix_set(q,2,3);
    smatrix_set(q,2,7);
    smatrix_set(q,2,8);
    smatrix_set(q,2,11);
    smatrix_clear(q,2,2);

    printf("\ncompact form:\n");
    smatrix_print(q);

    printf("\nexpanded form:\n");
    smatrix_print_expanded(q);

    printf("\ncertain values:\n");
    printf("  A[%2u,%2u] = %1u\n", 1, 1, smatrix_get(q,1,1));
    printf("  A[%2u,%2u] = %1u\n", 1, 2, smatrix_get(q,1,2));

    // generate vectors
    unsigned char x[N];
    unsigned char y[M];
    unsigned int i;
    unsigned int j;
    for (j=0; j<N; j++)
        x[j] = rand() % 2 ? 1 : 0;

    smatrix_vmul(q,x,y);

    // print results
    printf("x = [");
    for (j=0; j<N; j++) printf("%2u", x[j]);
    printf(" ];\n");

    printf("y = [");
    for (i=0; i<M; i++) printf("%2u", y[i]);
    printf(" ];\n");

    smatrix_destroy(q);

    // 
    // test matrix multiplication
    //
    smatrix a = smatrix_create( 8,12);
    smatrix b = smatrix_create(12, 5);
    smatrix c = smatrix_create( 8, 5);

    // initialize 'a'
    // 0 0 0 0 1 0 0 0 0 0 0 0
    // 0 0 0 0 0 0 0 0 0 0 0 0
    // 0 0 0 1 0 0 0 1 0 0 0 0
    // 0 0 0 0 1 0 0 0 0 0 0 0
    // 0 0 0 1 0 0 1 1 0 0 0 0
    // 0 0 0 0 0 0 0 0 0 0 0 0
    // 0 1 0 0 0 0 0 0 0 0 0 0
    // 0 0 0 0 0 0 0 1 1 0 0 0
    smatrix_set(a,0,4);
    smatrix_set(a,2,3);
    smatrix_set(a,2,7);
    smatrix_set(a,3,4);
    smatrix_set(a,4,3);
    smatrix_set(a,4,6);
    smatrix_set(a,4,7);
    smatrix_set(a,6,1);
    smatrix_set(a,7,7);
    smatrix_set(a,7,8);

    // initialize 'b'
    // 1 1 0 0 0
    // 0 0 0 0 1
    // 0 0 0 0 0
    // 0 0 0 0 0
    // 0 0 0 0 0
    // 0 0 0 0 1
    // 0 0 0 1 0
    // 0 0 0 1 0
    // 0 0 0 0 0
    // 0 1 0 0 1
    // 1 0 0 1 0
    // 0 1 0 0 0
    smatrix_set(b,0,0);
    smatrix_set(b,0,1);
    smatrix_set(b,1,4);
    smatrix_set(b,5,4);
    smatrix_set(b,6,3);
    smatrix_set(b,7,3);
    smatrix_set(b,9,1);
    smatrix_set(b,9,4);
    smatrix_set(b,10,0);
    smatrix_set(b,11,1);

    // compute 'c'
    // 0 0 0 0 0
    // 0 0 0 0 0
    // 0 0 0 1 0
    // 0 0 0 0 0
    // 0 0 0(0)0
    // 0 0 0 0 0
    // 0 0 0 0 1
    // 0 0 0 1 0
    smatrix_mul(a,b,c);

    // print results
    printf("a:\n"); smatrix_print_expanded(a);
    printf("b:\n"); smatrix_print_expanded(b);
    printf("c:\n"); smatrix_print_expanded(c);

    smatrix_destroy(a);
    smatrix_destroy(b);
    smatrix_destroy(c);

    printf("done.\n");
    return 0;
}


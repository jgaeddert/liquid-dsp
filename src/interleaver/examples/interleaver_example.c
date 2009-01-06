//
//
//

#include <stdio.h>
#include <stdlib.h> // for rand()

#include "../src/interleaver.h"

int main() {
#if 0
    unsigned int i, j=3, M=3, N=5, L=15;

    unsigned int m=2,n=0;
    for (i=0; i<L; i++) {
        //j = m*N + n; // input
        do {
            j = n*M + m; // output
            j = m*N + n; // output
            m++;
            if (m==M) {
                n = (n+1)%N;
                m=0;
            }
        } while (j>=L);
        
        printf("%u, ", j);
    }
    printf("\n");
    return 0;
#else
    unsigned int n=9;
    interleaver q = interleaver_create(n, INT_SEQUENCE);
    interleaver_print(q);
    interleaver_debug_print(q);

    unsigned char x[n];
    unsigned char y[n];
    unsigned char z[n];
    unsigned int i;
    for (i=0; i<n; i++)
        x[i] = rand()%256;

    interleaver_interleave(q,x,y);
    interleaver_deinterleave(q,y,z);
    //interleaver_print(q);

    unsigned int num_errors=0;
    printf("x\ty\tz\n");
    for (i=0; i<n; i++) {
        printf("%u\t%u\t%u\n", (unsigned int) (x[i]), (unsigned int) (y[i]), (unsigned int) (z[i]));
        //printf("y[%u] = %u\n", i, (unsigned int) (y[i]));
        //printf("y[%u] = %#0x\n", i, (unsigned int) (y[i]));
        num_errors += (x[i]==z[i]) ? 0 : 1;
    }
    printf("errors: %u / %u\n", num_errors, n);

    interleaver_destroy(q);

    /*

    for (i=0; i<n; i++)
        printf("x[%u] = %u\n", i, (unsigned int)(x[i]));
    printf("--------\n");

    interleaver_circshift_left(x, n, 5);
    for (i=0; i<n; i++)
        printf("x[%u] = %u\n", i, (unsigned int)(x[i]));

    printf("--------\n");
    interleaver_circshift_right(x, n, 5);
    for (i=0; i<n; i++)
        printf("x[%u] = %u\n", i, (unsigned int)(x[i]));

    interleaver_destroy(q);
    printf("done.\n");
    */
    return 0;
#endif
}


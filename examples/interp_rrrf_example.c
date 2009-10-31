//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"

int main() {
    // options
    unsigned int k=4;   // samples/symbol
    unsigned int m=3;   // filter delay
    float beta = 0.3f;  // filter excess bandwidth
    unsigned int num_symbols=8;   // number of symbols

    // derived values
    unsigned int h_len = 2*k*m+1;
    unsigned int num_samples = k*num_symbols;

    // create filter
    float h[h_len];
    design_rcos_filter(k,m,beta,0.0f,h);

    interp_rrrf q = interp_rrrf_create(k,h,h_len);

    float x[num_symbols];
    float y[num_samples];

    unsigned int i, n;
    for (i=0; i<num_symbols; i++)
        x[i] = (rand()%2 ? 1.0f : -1.0f);

    n=0;
    for (i=0; i<num_symbols; i++) {
        interp_rrrf_execute(q, x[i], &y[n]);
        n+=k;
    }

    printf("h(t) :\n");
    for (i=0; i<h_len; i++)
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++)
        printf("  x(%2u) = %8.4f;\n", i+1, x[i]);

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        printf("  y(%2u) = %8.4f;", i+1, y[i]);
        if ( (i >= k*m) && ((i%k)==0))
            printf(" **\n");
        else
            printf("\n");
    }

    interp_rrrf_destroy(q);
    printf("done.\n");
    return 0;
}

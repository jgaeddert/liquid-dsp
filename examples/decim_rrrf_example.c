//
// decim_rrrf_example.c
//

#include <stdio.h>

#include "liquid.h"

int main() {
    // options
    unsigned int num_symbols = 5;   // number of symbols

    // fixed, derived values
    unsigned int h_len = 4;         // filter length
    unsigned int D = h_len;         // decim factor (samples/symbol)
    unsigned int num_samples = num_symbols * D;

    // fixed, asymmetric filter with E{h^2} = 1
    float h[] = {0.18257, 0.36515, 0.54772, 0.73030};

    float s[num_symbols];           // symbols array
    float x[num_samples];           // samples array
    float y[num_symbols];           // output symbols

    unsigned int i;
    unsigned int j;

    // generate random symbols (+/- 1)
    for (i=0; i<num_symbols; i++)
        s[i] = rand()%2 ? 1.0f : -1.0f;

    // up-sample symbols using filter coefficients
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<D; j++)
            x[i*D + j] = s[i]*h[j];
    }

    // create decimator, loading filter in *reverse* order
    float g[h_len];
    for (i=0; i<h_len; i++)
        g[i] = h[h_len-i-1];
    decim_rrrf q = decim_rrrf_create(D,g,h_len);

    // run decimator
    unsigned int n=0;
    for (i=0; i<num_symbols; i++) {
        decim_rrrf_execute(q, &x[n], &y[i], D-1);
        n+=D;
    }

    // print results

    printf("h(t) :\n");
    for (i=0; i<h_len; i++)
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);

    printf("x(t) :\n");
    for (i=0; i<num_samples; i++)
        printf("  x(%2u) = %8.4f;\n", i+1, x[i]);

    printf("y(t) :\n");
    for (i=0; i<num_symbols; i++)
        printf("  y(%2u) = %8.4f (%8.4f);\n", i+1, y[i], s[i]);

    decim_rrrf_destroy(q);
    printf("done.\n");
    return 0;
}

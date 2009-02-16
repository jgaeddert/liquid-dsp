//
// Matched filter example
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"

#undef DEBUG

int main() {
    // options
    unsigned int k=2;   // samples/symbol
    unsigned int m=3;   // symbol delay
    float beta=0.7f;    // excess bandwidth factor
    unsigned int num_symbols=16;

    // initialize objects
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    design_rrc_filter(k,m,beta,0,h);
    interp_rrrf q  = interp_rrrf_create(k,h,h_len);
    decim_rrrf d   = decim_rrrf_create(k,h,h_len);

    // generate signal
    float sym_in, buff[k], sym_out;

    unsigned int i;
#ifdef DEBUG
    unsigned int num_samples=k*num_symbols;
    float y[num_samples];
    unsigned int n=0;
#endif
    for (i=0; i<num_symbols; i++) {
        // generate random symbol
        sym_in = (rand() % 2) ? 1.0f : -1.0f;

        // interp_rrrfolate
        interp_rrrf_execute(q, sym_in, buff);

        // decim_rrrfate
        decim_rrrf_execute(d, buff, &sym_out, 0);

        // normalize output
        sym_out /= k;

        printf("  %3u : %8.5f", i, sym_out);
        if (i>=2*m) printf(" *\n");
        else        printf("\n");

#ifdef DEBUG
        // debug: save output symbols
        memcpy(&y[n], buff, k*sizeof(float));
        n += k;
#endif
    }

#ifdef DEBUG
    for (i=0; i<num_samples; i++)
        printf(" y(%3u) = %8.5f;\n", i+1, y[i]);
#endif
    
    // clean it up
    interp_rrrf_destroy(q);
    decim_rrrf_destroy(d);
    printf("done.\n");
    return 0;
}


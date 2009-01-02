//
//
//

#include <stdio.h>

#include "../src/filter.h"
#include "../src/firdes.h"

#define DEBUG
#define DEBUG_FILENAME "debug_symsync_example.m"

int main() {
    // options
    unsigned int k=2;
    unsigned int m=3;
    float beta=0.3f;
    unsigned int num_filters=4;
    unsigned int num_symbols=16;

    // design interpolating filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    design_rrc_filter(k,m,beta,0,h);

    // create interpolator
    interp q = interp_create(k,h,h_len);

    // design polyphase filter
    unsigned int H_len = 2*num_filters*k*m + 1;
    float H[H_len];
    design_rrc_filter(k*num_filters,m,beta,0,H);
    // create symbol synchronizer
    symsync d = symsync_create(k, num_filters, H, H_len);
    symsync_print(d);

    unsigned int i, n=0;
    unsigned int num_samples = k*num_symbols;
    float x[num_symbols];
    float y[num_samples];
    float z[num_samples];

#ifdef DEBUG
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% debug_symsync_example.m, auto-generated file\n\n");
    fprintf(fid,"close all;\nclear all;\n\n");
#endif

    for (i=0; i<num_symbols; i++) {
        x[i] = (i%2) ? 1.0f : -1.0f;
    }

    // run interpolator
    for (i=0; i<num_symbols; i++) {
        interp_execute(q, x[i], &y[n]);
        n+=k;
    }

    // run symbol synchronizer
    unsigned int num_symbols_sync;
    symsync_execute(d, y, num_samples, z, &num_symbols_sync);

    printf("h(t) :\n");
    for (i=0; i<h_len; i++) {
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);
#ifdef DEBUG
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);
#endif
    }

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++) {
        printf("  x(%2u) = %8.4f;\n", i+1, x[i]);
#ifdef DEBUG
        fprintf(fid,"x(%3u) = %12.5f;\n", i+1, x[i]);
#endif
    }

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        printf("  y(%2u) = %8.4f;\n", i+1, y[i]);
#ifdef DEBUG
        fprintf(fid,"y(%3u) = %12.5f;\n", i+1, y[i]);
#endif
    }

    printf("z(t) :\n");
    for (i=0; i<num_symbols_sync; i++) {
        printf("  z(%2u) = %8.4f;\n", i+1, z[i]);
#ifdef DEBUG
        fprintf(fid,"z(%3u) = %12.5f;\n", i+1, z[i]);
#endif
    }

#ifdef DEBUG
    fprintf(fid,"\n\n");
    fclose(fid);

    printf("results written to %s.\n", DEBUG_FILENAME);
#endif

    // clean it up
    interp_destroy(q);
    symsync_destroy(d);
    printf("done.\n");
    return 0;
}

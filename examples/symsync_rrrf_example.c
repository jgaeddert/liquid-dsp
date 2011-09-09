//
// symsync_rrrf_example.c
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"

#define OUTPUT_FILENAME "symsync_rrrf_example.m"

int main() {
    // options
    unsigned int k=2;               // samples per symbol
    unsigned int m=5;               // filter delay [symbols]
    float beta=0.3f;                // excess bandwidth factor
    unsigned int num_filters=64;    // number of filters in the bank
    unsigned int num_symbols=256;   // number of data symbols

    float bt=0.2f;                  // loop filter bandwidth
    float dt=0.5f;                  // fractional sample offset
    unsigned int ds=1;              // additional symbol delay
    

    // design interpolating filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    design_rrc_filter(k,m,beta,dt,h);

    // create interpolator
    interp_rrrf q = interp_rrrf_create(k,h,h_len);

    // create symbol synchronizer
    symsync_rrrf d = symsync_rrrf_create_rnyquist(LIQUID_RNYQUIST_RRC,k,m,beta,num_filters);
    symsync_rrrf_set_lf_bw(d,bt);

    unsigned int i;
    unsigned int num_samples = k*num_symbols;
    float x[num_symbols];       // input symbol sequence
    float y[num_samples];       // interpolated time series
    float z[4*num_symbols];     // output time series, decimated to 2 samples/symbol

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"close all;\nclear all;\n\n");

    fprintf(fid,"k=%u;\n",k);
    fprintf(fid,"m=%u;\n",m);
    fprintf(fid,"beta=%12.8f;\n",beta);
    fprintf(fid,"num_filters=%u;\n",num_filters);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);

    for (i=0; i<num_symbols; i++)
        x[i] = rand() % 2 ? 1.0f : -1.0f;   // random BPSK signal

    // run interpolator
    for (i=0; i<num_symbols; i++)
        interp_rrrf_execute(q, x[i], &y[k*i]);

    // run symbol synchronizer
    unsigned int num_symbols_sync;
    symsync_rrrf_execute(d, &y[ds], num_samples-ds, z, &num_symbols_sync);

    printf("h(t) :\n");
    for (i=0; i<h_len; i++) {
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);
    }

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++) {
        //printf("  x(%2u) = %8.4f;\n", i+1, x[i]);
        fprintf(fid,"x(%3u) = %12.5f;\n", i+1, x[i]);
    }

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        //printf("  y(%2u) = %8.4f;\n", i+1, y[i]);
        fprintf(fid,"y(%3u) = %12.5f;\n", i+1, y[i]);
    }

    printf("z(t) :\n");
    for (i=0; i<num_symbols_sync; i++) {
        printf("  z(%2u) = %8.4f;\n", i+1, z[i]);
        fprintf(fid,"z(%3u) = %12.5f;\n", i+1, z[i]);
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"zp = filter(h,1,y);\n");
    fprintf(fid,"figure;\nhold on;\n");
    fprintf(fid,"plot([0:length(x)-1],          x,'ob');\n");
    fprintf(fid,"plot([0:length(y)-1]/2  -m,    y,'-','Color',[0.8 0.8 0.8]);\n");
    fprintf(fid,"plot([0:length(zp)-1]/2 -2*m,  zp/2,'-b');\n");
    fprintf(fid,"plot([0:length(z)-1]    -2*m+1,z/2,'xr');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('symbol index');\n");
    fprintf(fid,"ylabel('symbol/signal');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"legend('sym in','interp_rrrf','mf','sym out',0);\n");
    fclose(fid);

    printf("results written to %s.\n", OUTPUT_FILENAME);

    // clean it up
    interp_rrrf_destroy(q);
    symsync_rrrf_destroy(d);
    printf("done.\n");
    return 0;
}

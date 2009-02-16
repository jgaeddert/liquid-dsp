//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "liquid.h"

#define DEBUG
#define DEBUG_FILENAME "symsync_rrrf_example.m"

int main() {
    // options
    unsigned int k=2;
    unsigned int m=5;
    float beta=0.3f;
    unsigned int num_filters=64;
    unsigned int num_symbols=256;

    float bt=0.2f;      // loop filter bandwidth
    float dt=0.5f;      // fractional sample offset
    unsigned int ds=1;  // additional symbol delay
    
    // use random data or 101010 phasing pattern
    bool random_data=false;



    // design interp_rrrfolating filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    design_rrc_filter(k,m,beta,dt,h);

    // create interp_rrrfolator
    interp_rrrf q = interp_rrrf_create(k,h,h_len);

    // design polyphase filter
    unsigned int H_len = 2*num_filters*k*m + 1;
    float H[H_len];
    design_rrc_filter(k*num_filters,m,beta,0,H);
    // create symbol synchronizer
    symsync_rrrf d = symsync_rrrf_create(k, num_filters, H, H_len);
    symsync_rrrf_set_lf_bw(d,bt);

    unsigned int i, n=0;
    unsigned int num_samples = k*num_symbols;
    float x[num_symbols];
    float y[num_samples];
    float z[num_samples];

#ifdef DEBUG
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"close all;\nclear all;\n\n");

    fprintf(fid,"k=%u;\n",k);
    fprintf(fid,"m=%u;\n",m);
    fprintf(fid,"beta=%12.8f;\n",beta);
    fprintf(fid,"num_filters=%u;\n",num_filters);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);
#endif

    for (i=0; i<num_symbols; i++) {
        if (random_data)
            x[i] = rand() % 2 ? 1.0f : -1.0f;   // random signal
        else
            x[i] = (i%2) ? 1.0f : -1.0f;  // 101010 phasing pattern
    }

    // run interp_rrrfolator
    for (i=0; i<num_symbols; i++) {
        interp_rrrf_execute(q, x[i], &y[n]);
        n+=k;
    }

    // run symbol synchronizer
    unsigned int num_symbols_sync;
    symsync_rrrf_execute(d, &y[ds], num_samples-ds, z, &num_symbols_sync);

    printf("h(t) :\n");
    for (i=0; i<h_len; i++) {
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);
#ifdef DEBUG
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);
#endif
    }

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++) {
        //printf("  x(%2u) = %8.4f;\n", i+1, x[i]);
#ifdef DEBUG
        fprintf(fid,"x(%3u) = %12.5f;\n", i+1, x[i]);
#endif
    }

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        //printf("  y(%2u) = %8.4f;\n", i+1, y[i]);
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

    printf("results written to %s.\n", DEBUG_FILENAME);
#endif

    // clean it up
    interp_rrrf_destroy(q);
    symsync_rrrf_destroy(d);
    printf("done.\n");
    return 0;
}

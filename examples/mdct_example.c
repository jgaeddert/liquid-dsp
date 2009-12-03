//
// mdct_example.c
//
// Modified discrete cosine transform (MDCT) example. Demonstrates
// the functionality and interface for the mdct and imdct routines.
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "mdct_example.m"

int main() {
    // MDCT options
    unsigned int N=64; // MDCT size
    unsigned int num_symbols=16;

    // filter options
    unsigned int h_len=21;  // filter length
    float fc=0.02f;         // filter cutoff
    float slsl=60.0f;       // sidelobe suppression level
    float mu=0.0f;          // timing offset

    // derived values
    unsigned int i,j;
    unsigned int num_samples = N*num_symbols;
    float x[num_samples];
    float X[num_samples];
    float y[num_samples];
    for (i=0; i<num_samples; i++) {
        x[i] = 0.0f;
        X[i] = 0.0f;
        y[i] = 0.0f;
    }

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"N = %u;\n", N);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = N*num_symbols;\n");

    // generate window
    float w[2*N];
    for (i=0; i<2*N; i++) {
        // raised half sine
        //w[i] = sinf(M_PI/(2.0f*N)*(i+0.5f));

        // shaped pulse
        float t0 = sinf(M_PI/(2*N)*(i+0.5));
        w[i] = sinf(M_PI*0.5f*t0*t0);
    }

    // generate basic filter
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,mu,h);
    fir_filter_rrrf f = fir_filter_rrrf_create(h,h_len);

    // generate random signal (filtered noise)
    for (i=0; i<num_samples; i++) {
        // generate filtered noise
        fir_filter_rrrf_push(f, randnf());
        fir_filter_rrrf_execute(f, &x[i]); 

        fprintf(fid,"x(%4u) = %12.4e;", i+1, x[i]);
    }

    // run analyzer
    for (i=0; i<num_symbols-1; i++) {
        mdct(&x[i*N],&X[i*N],w,N);
    }

    // run synthesizer
    float y_tmp[2*N];   // temporary buffer
    for (i=0; i<num_symbols-1; i++) {
        // run inverse MDCT
        imdct(&X[i*N],y_tmp,w,N);

        // accumulate result in output buffer
        for (j=0; j<2*N; j++)
            y[i*N+j] += y_tmp[j];
    }

    // print results to file
    fprintf(fid,"w = zeros(1,2*N);\n");
    for (i=0; i<2*N; i++)
        fprintf(fid,"w(%3u) = %12.4e;\n",i+1,w[i]);

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y[i]);
    }
    fprintf(fid,"X = zeros(N,num_symbols);\n");
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<N; j++)
            fprintf(fid,"X(%3u,%3u) = %12.4e;\n", i+1, j+1, X[i*N+j]);
    }

    // plot spectral response
    /*
    fprintf(fid,"figure;\n");
    fprintf(fid,"f = [0:(N-1)]/(2*N);\n");
    fprintf(fid,"plot(f,20*log10(abs(X')),'Color',[1 1 1]*0.5);\n");
    */

    // plot time-domain reconstruction
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,t,y);\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('signal output');\n");
    fprintf(fid,"legend('original','reconstructed',0);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    fir_filter_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}


//
// Matched filter example
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"

#define OUTPUT_FILENAME "matched_filter_example.m"

int main() {
    // options
    unsigned int k=2;   // samples/symbol
    unsigned int m=6;   // symbol delay
    float beta=0.5f;    // excess bandwidth factor
    unsigned int num_symbols=16;

    // initialize objects
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    //design_rrc_filter(k,m,beta,0,h);
    design_rkaiser_filter(k,m,beta,0,h);
    interp_rrrf q  = interp_rrrf_create(k,h,h_len);
    decim_rrrf d   = decim_rrrf_create(k,h,h_len);

    // generate signal
    float sym_in, buff[k], sym_out;

    unsigned int i;
    for (i=0; i<h_len; i++)
        printf("h(%3u) = %12.8f;\n", i+1, h[i]);

    unsigned int num_samples=k*num_symbols;
    float y[num_samples];
    unsigned int n=0;

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

        // save output symbols
        memcpy(&y[n], buff, k*sizeof(float));
        n += k;
    }

    for (i=0; i<num_samples; i++)
        printf(" y(%3u) = %8.5f;\n", i+1, y[i]);

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%3u) = %20.8e;\n", i+1, h[i]);
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H = 20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD');\n");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);
    
    // clean it up
    interp_rrrf_destroy(q);
    decim_rrrf_destroy(d);
    printf("done.\n");
    return 0;
}


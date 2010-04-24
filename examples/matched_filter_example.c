//
// Matched filter example
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "matched_filter_example.m"

int main() {
    // options
    unsigned int k=2;   // samples/symbol
    unsigned int m=3;   // symbol delay
    float beta=0.7f;    // excess bandwidth factor
    unsigned int num_symbols=16;

    // initialize objects
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    //design_hM3_filter(k,m,beta,0,h);
    design_rkaiser_filter(k,m,beta,0,h);
    //design_rrc_filter(k,m,beta,0,h);
    interp_rrrf q  = interp_rrrf_create(k,h,h_len);
    decim_rrrf d   = decim_rrrf_create(k,h,h_len);

    // compute filter inter-symbol interference
    float isi_mse=0;
    float isi_max=0;
    liquid_filter_isi(h,k,m,&isi_mse, &isi_max);
    printf("  isi (max) : %12.8f dB\n", 20*log10f(isi_max));
    printf("  isi (mse) : %12.8f dB\n", 20*log10f(isi_mse));

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

        // interpolate
        interp_rrrf_execute(q, sym_in, buff);

        // decimate
        decim_rrrf_execute(d, buff, &sym_out, 0);

        // normalize output
        sym_out /= k;

        printf("  %3u : %8.5f", i, sym_out);
        if (i>=2*m) printf(" *\n");
        else        printf("\n");

        // save output symbols
        memmove(&y[n], buff, k*sizeof(float));
        n += k;
    }

    //for (i=0; i<num_samples; i++)
    //    printf(" y(%3u) = %8.5f;\n", i+1, y[i]);

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"beta = %12.8f;\n", beta);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%3u) = %20.8e;\n", i+1, h[i]);
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H = 20*log10(abs(fftshift(fft(h/k,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-','LineWidth',2,...\n");
    fprintf(fid,"     [0.5/k],[-3],'or',...\n");
    fprintf(fid,"     [0.5/k*(1-beta) 0.5/k*(1-beta)],[-100 10],'-r',...\n");
    fprintf(fid,"     [0.5/k*(1+beta) 0.5/k*(1+beta)],[-100 10],'-r');\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD');\n");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");
    fprintf(fid,"grid on;\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"g = conv(h,fliplr(h))/k;\n");
    fprintf(fid,"t = [(-2*k*m):(2*k*m)]/k;\n");
    fprintf(fid,"i0 = [0:k:4*k*m]+1;\n");
    fprintf(fid,"plot(t,g,'-s',...\n");
    fprintf(fid,"     t(i0),g(i0),'or');\n");
    fprintf(fid,"xlabel('symbol index');\n");
    fprintf(fid,"ylabel('matched filter response');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);
    
    // clean it up
    interp_rrrf_destroy(q);
    decim_rrrf_destroy(d);
    printf("done.\n");
    return 0;
}


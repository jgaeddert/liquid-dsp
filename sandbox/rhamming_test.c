// Design (approximate) square-root Nyquist Hamming filter
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "rhamming_test.m"

int main() {
    // options
    unsigned int k     =  2;
    unsigned int m     = 12;
    int          wtype = LIQUID_WINDOW_HAMMING;
    float        arg   = 0;

    // derived values
    unsigned int h_len = 2*k*m+1;
    float h[h_len];

    // design filter
    float gamma = 1.0f;
    float gamma_opt = gamma;
    float isi_opt = 1e12f;
    while (gamma < 1.15f) {
        liquid_firdes_windowf(wtype,h_len,gamma*0.5f/(float)k,arg,h);

        // compute filter ISI
        float isi_rms, isi_max;
        liquid_filter_isi(h,k,m,&isi_rms,&isi_max);
        printf("gamma: %8.6f, isi:%8.4f dB\n", gamma, 20*log10(isi_rms));
        if (isi_rms < isi_opt) {
            isi_opt = isi_rms;
            gamma_opt = gamma;
        }
        gamma += 0.001f;
    }
    // re-design with optimum and save to file
    printf("optimum RMS ISI of %.2f dB with gamma=%.6f\n", 20*log10(isi_opt), gamma_opt);
    liquid_firdes_windowf(wtype,h_len,gamma_opt*0.5f/(float)k,arg,h);
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"k = %u; m = %u; h = zeros(2*k*m+1,1);\n", k, m);
    fprintf(fid,"h = [");
    unsigned int i;
    for (i=0; i<h_len; i++)
        fprintf(fid,"%g,", h[i]);
    fprintf(fid,"];\n");
    fprintf(fid,"g = conv(h,h);\n");
    fprintf(fid,"th = [(-k*m):(k*m)]/k;\n");
    fprintf(fid,"tg = [(-2*k*m):(2*k*m)]/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1), plot(th,h,'.-'); grid on; ylabel('h');\n");
    fprintf(fid,"subplot(2,1,2), plot(tg,g,'.-'); grid on; ylabel('h * h');\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
    return 0;
}


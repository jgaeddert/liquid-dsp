// file: eqrls_rrrf_example.c
//
// Tests recursive least-squares equalizer
//
// Complile and run:
//   $ gcc eqrls_rrrf_example.c -lliquid -o eqrls_rrrf_example
//   $ ./eqrls_rrrf_example

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

#define DEBUG_FILENAME "eqrls_rrrf_example.m"

int main() {
    // options
    unsigned int n=32;      // number of symbols to observe
    unsigned int ntrain=16; // number of training symbols
    unsigned int h_len = 4; // channel filter length
    unsigned int p=6;       // equalizer order

    // bookkeeping variables
    float d[n];     // data sequence
    float y[n];     // received data sequence (filtered by channel)
    float d_hat[n]; // recovered data sequence
    float h[h_len]; // channel filter coefficients
    float w[p];     // equalizer filter coefficients
    unsigned int i;

    // open output file
    FILE * fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);

    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"ntrain=%u;\n",ntrain);
    fprintf(fid,"p=%u;\n",p);
    fprintf(fid,"h_len=%u;\n",h_len);

    // create equalizer
    eqrls_rrrf eq = eqrls_rrrf_create(p);

    // create channel filter (random delay taps)
    h[0] = 1.0f;
    for (i=1; i<h_len; i++)
        h[i] = randnf() * 0.1f;
    fir_filter_rrrf f = fir_filter_rrrf_create(h,h_len);

    // generate random data signal
    for (i=0; i<n; i++)
        d[i] = rand() % 2 ? 1.0f : -1.0f;

    // filter data signal through channel
    for (i=0; i<n; i++) {
        fir_filter_rrrf_push(f,d[i]);
        fir_filter_rrrf_execute(f,&y[i]);
    }

    // run equalizer
    for (i=0; i<p; i++)
        w[i] = 0;
    eqrls_rrrf_train(eq, w, y, d, ntrain);

    // create filter from equalizer output
    fir_filter_rrrf feq = fir_filter_rrrf_create(w,p);

    // run equalizer filter
    for (i=0; i<n; i++) {
        fir_filter_rrrf_push(feq,y[i]);
        fir_filter_rrrf_execute(feq,&d_hat[i]);
    }

    //
    // print results
    //
    printf("channel:\n");
    for (i=0; i<h_len; i++) {
        printf("h(%3u) = %8.4f\n", i+1, h[i]);
        fprintf(fid,"h(%3u) = %12.4e;\n", i+1, h[i]);
    }

    printf("equalizer:\n");
    for (i=0; i<p; i++) {
        printf("w(%3u) = %8.4f\n", i+1, w[i]);
        fprintf(fid,"w(%3u) = %12.4e;\n", i+1, w[i]);
    }

    float e, mse=0.0f;
    for (i=0; i<n; i++) {
        if (i==ntrain)
            printf("----------\n");

        printf("d(%3u) = %8.4f; ", i, d[i]);
        printf("y(%3u) = %8.4f; ", i, y[i]);
        printf("d_hat(%3u) = %8.4f; ", i, d_hat[i]);
        printf("\n");

        fprintf(fid,"d(%3u) = %12.4e; ", i+1, d[i]);
        fprintf(fid,"y(%3u) = %12.4e; ", i+1, y[i]);
        fprintf(fid,"d_hat(%3u) = %12.4e; ", i+1, d_hat[i]);
        fprintf(fid,"\n");

        // compute mse
        e = d[i] - d_hat[i];
        mse += e*e;
    }
    mse /= n;
    printf("mse: %12.8f\n", mse);

    // plot results
    fprintf(fid,"\n\n");

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"W=20*log10(abs(fftshift(fft(w,nfft))));\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"plot(f,H,'-r',f,W,'-b');\n");
    fprintf(fid,"xlabel('Normalied Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"legend('channel','equalizer',0);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"stem(0:(h_len-1),h,'-r');\n");
    fprintf(fid,"stem(0:(p-1),    w,'-b');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('');\n");
    fprintf(fid,"ylabel('Filter Coefficients');\n");
    fprintf(fid,"legend('channel','equalizer',0);\n");
    fprintf(fid,"axis([-0.25 max(h_len,p)-0.75 -0.5 1.5]);\n");

    fclose(fid);
    printf("results written to %s.\n",DEBUG_FILENAME);

    fir_filter_rrrf_destroy(f);
    eqrls_rrrf_destroy(eq);
    fir_filter_rrrf_destroy(feq);
    return 0;
}

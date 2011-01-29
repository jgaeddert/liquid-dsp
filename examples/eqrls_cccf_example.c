// 
// eqrls_cccf_example.c
//
// Tests recursive least-squares (RLS) equalizer (EQ).
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "liquid.h"

#define OUTPUT_FILENAME "eqrls_cccf_example.m"

// print macro for complex numbers
//  F   :   output file
//  S   :   variable name (string)
//  I   :   index
//  V   :   value
#define PRINT_COMPLEX(F,S,I,V) fprintf(F,"%s(%4u) = %5.2f+j*%5.2f;",S,I,crealf(V),cimagf(V));

int main() {
    // options
    unsigned int n=32;      // number of symbols to observe
    unsigned int ntrain=16; // number of training symbols
    unsigned int h_len = 4; // channel filter length
    unsigned int p=6;       // equalizer order

    // bookkeeping variables
    float complex d[n];     // data sequence
    float complex y[n];     // received data sequence (filtered by channel)
    float complex d_hat[n]; // recovered data sequence
    float complex h[h_len]; // channel filter coefficients
    float complex w[p];     // equalizer filter coefficients
    unsigned int i;

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);

    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"ntrain=%u;\n",ntrain);
    fprintf(fid,"p=%u;\n",p);
    fprintf(fid,"h_len=%u;\n",h_len);

    // create equalizer
    eqrls_cccf eq = eqrls_cccf_create(NULL,p);

    // create channel filter (random delay taps)
    h[0] = 1.0f;
    for (i=1; i<h_len; i++)
        h[i] = (randnf() + randnf()*_Complex_I) * 0.1f;
    firfilt_cccf f = firfilt_cccf_create(h,h_len);

    // generate random data signal
    for (i=0; i<n; i++)
        d[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;

    // filter data signal through channel
    for (i=0; i<n; i++) {
        firfilt_cccf_push(f,d[i]);
        firfilt_cccf_execute(f,&y[i]);
    }

    // run equalizer
    for (i=0; i<p; i++)
        w[i] = 0;
    eqrls_cccf_train(eq, w, y, d, ntrain);

    // create filter from equalizer output
    firfilt_cccf feq = firfilt_cccf_create(w,p);

    // run equalizer filter
    for (i=0; i<n; i++) {
        firfilt_cccf_push(feq,y[i]);
        firfilt_cccf_execute(feq,&d_hat[i]);
    }

    //
    // print results
    //
    printf("channel:\n");
    for (i=0; i<h_len; i++) {
        PRINT_COMPLEX(stdout,"h",i+1,h[i]); printf("\n");
        PRINT_COMPLEX(fid,   "h",i+1,h[i]); fprintf(fid,"\n");
    }

    printf("equalizer:\n");
    for (i=0; i<p; i++) {
        PRINT_COMPLEX(stdout,"w",i+1,w[i]); printf("\n");
        PRINT_COMPLEX(fid,   "w",i+1,w[i]); fprintf(fid,"\n");
    }

    float complex e;
    float mse=0.0f;
    for (i=0; i<n; i++) {
        if (i==ntrain)
            printf("----------\n");

        PRINT_COMPLEX(stdout,"d",   i+1,    d[i]);
        PRINT_COMPLEX(stdout,"y",   i+1,    y[i]);
        PRINT_COMPLEX(stdout,"dhat",i+1,    d_hat[i]);
        printf("\n");

        PRINT_COMPLEX(fid,  "d",    i+1,    d[i]);
        PRINT_COMPLEX(fid,  "y",    i+1,    y[i]);
        PRINT_COMPLEX(fid,  "d_hat",i+1,    d_hat[i]);
        fprintf(fid, "\n");

        // compute mse
        e = d[i] - d_hat[i];
        mse += crealf(e*conj(e));
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
    fprintf(fid,"plot(f,H,'-r',f,W,'-b');\n");
    fprintf(fid,"xlabel('Normalied Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"legend('channel','equalizer',0);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"stem(0:(h_len-1),real(h),'-r');\n");
    fprintf(fid,"stem(0:(p-1),    real(w),'-b');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"ylabel('Real Coefficients');\n");
    fprintf(fid,"legend('channel','equalizer',0);\n");
    fprintf(fid,"axis([-0.25 max(h_len,p)-0.75 -0.5 1.5]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"stem(0:(h_len-1),imag(h),'-r');\n");
    fprintf(fid,"stem(0:(p-1),    imag(w),'-b');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"ylabel('Imag Coefficients');\n");
    fprintf(fid,"legend('channel','equalizer',0);\n");
    fprintf(fid,"axis([-0.25 max(h_len,p)-0.75 -0.5 1.5]);\n");


    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(y,'xr',d_hat,'xb');\n");
    fprintf(fid,"axis('square');\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature');\n");
    fprintf(fid,"legend('received','equalized',1');\n");

    fclose(fid);
    printf("results written to %s.\n",OUTPUT_FILENAME);

    firfilt_cccf_destroy(f);
    eqrls_cccf_destroy(eq);
    firfilt_cccf_destroy(feq);
    return 0;
}

//
// firdes_gmskrx_test.c : test synthesis of GMSK receive filter
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firdes_gmskrx_test.m"

int main() {
    // options
    unsigned int k=4;       // samples/symbol
    unsigned int m=3;       // filter delay [symbols]
    float BT = 0.3f;        // bandwidth-time product

    // TODO : read properties from command line

    // TODO : validate input

    unsigned int i;

    // derived values
    unsigned int h_len = 2*k*m+1;   // filter length

    // arrays
    float ht[h_len];         // transmit filter coefficients
    float hr[h_len];         // recieve filter coefficients

    // design transmit filter
    liquid_firdes_gmsktx(k,m,BT,0.0f,ht);

    // print results to screen
    printf("gmsk transmit filter:\n");
    for (i=0; i<h_len; i++)
        printf("  ht(%3u) = %12.8f;\n", i+1, ht[i]);

    //
    // start of filter design procedure
    //

    float beta = BT;        // prototype filter cut-off
    float delta = 1e-2f;    // filter design correction factor

    // temporary arrays
    float h_primef[h_len];          // temporary buffer for real coefficients
    float g_primef[h_len];          // 

    float complex h_tx[h_len];      // impulse response of transmit filter
    float complex h_prime[h_len];   // impulse response of 'ideal' filter
    float complex g_prime[h_len];   // impulse response of 'gain' filter
    float complex h_hat[h_len];     // impulse response of receive filter
    
    float complex H_tx[h_len];      // frequency response of transmit filter
    float complex H_prime[h_len];   // frequency response of 'ideal' filter
    float complex G_prime[h_len];   // frequency response of 'gain' filter
    float complex H_hat[h_len];     // frequency response of receive filter

    // create 'ideal' matched filter
    // for now use raised-cosine
    design_nyquist_filter(LIQUID_NYQUIST_RCOS,k,m,beta,0.0f,h_primef);

    // create 'gain' filter to improve stop-band rejection
    float fc = (0.7f + 0.1*beta) / (float)k;
    float As = 40.0f;
    firdes_kaiser_window(h_len, fc, As, 0.0f, g_primef);

    // copy to fft input buffer, shifting appropriately
    // TODO : check if this fft shift is really necesary
    for (i=0; i<h_len; i++) {
        h_prime[i] = h_primef[ (i+k*m)%h_len ];
        g_prime[i] = g_primef[ (i+k*m)%h_len ];
        h_tx[i]    = ht[       (i+k*m)%h_len ];
    }

    // run ffts
    fft_run(h_len, h_prime, H_prime, FFT_FORWARD, 0);
    fft_run(h_len, g_prime, G_prime, FFT_FORWARD, 0);
    fft_run(h_len, h_tx,    H_tx,    FFT_FORWARD, 0);

#if 0
    // print results
    for (i=0; i<h_len; i++)
        printf("Ht(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(H_tx[i]), cimagf(H_tx[i]));
    for (i=0; i<h_len; i++)
        printf("H_prime(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(H_prime[i]), cimagf(H_prime[i]));
    for (i=0; i<h_len; i++)
        printf("G_prime(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(G_prime[i]), cimagf(G_prime[i]));
#endif

    // find minimum of reponses
    float H_tx_min = 0.0f;
    float H_prime_min = 0.0f;
    float G_prime_min = 0.0f;
    for (i=0; i<h_len; i++) {
        if (i==0 || crealf(H_tx[i]) < H_tx_min)
            H_tx_min = crealf(H_tx[i]);

        if (i==0 || crealf(H_prime[i]) < H_prime_min)
            H_prime_min = crealf(H_prime[i]);

        if (i==0 || crealf(G_prime[i]) < G_prime_min)
            G_prime_min = crealf(G_prime[i]);
    }

    // compute 'ideal' response, removing minima, and add correction factor
    // TODO : include additional term to add stop-band suppression
    for (i=0; i<h_len; i++) {
        H_hat[i] = crealf(H_prime[i] - H_prime_min + delta) / crealf(H_tx[i] - H_tx_min + delta);
        H_hat[i] *= crealf(G_prime[i] - G_prime_min) / crealf(G_prime[0]);
    }

    // compute ifft and copy response
    fft_run(h_len, H_hat, h_hat, FFT_REVERSE, 0);
    for (i=0; i<h_len; i++)
        hr[i] = crealf( h_hat[(i+k*m+1)%h_len] ) / (float)(k*h_len);

    //
    // end of filter design procedure
    //

    // print results to screen
    printf("gmsk receive filter:\n");
    for (i=0; i<h_len; i++)
        printf("  hr(%3u) = %12.8f;\n", i+1, hr[i]);

    // 
    // export output file
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"beta = %f;\n", BT);
    fprintf(fid,"h_len = 2*k*m+1;\n");
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"ht = zeros(1,h_len);\n");
    fprintf(fid,"hp = zeros(1,h_len);\n");
    fprintf(fid,"hr = zeros(1,h_len);\n");

    // print results
    for (i=0; i<h_len; i++)   fprintf(fid,"ht(%3u) = %12.4e;\n", i+1, ht[i]);
    for (i=0; i<h_len; i++)   fprintf(fid,"hr(%3u) = %12.4e;\n", i+1, hr[i]);
    for (i=0; i<h_len; i++)   fprintf(fid,"hp(%3u) = %12.4e;\n", i+1, h_primef[i]);
    
    fprintf(fid,"hc = k*conv(ht,hr);\n");

    // plot results
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Ht = 20*log10(abs(fftshift(fft(ht,  nfft))));\n");
    fprintf(fid,"Hp = 20*log10(abs(fftshift(fft(hp/k,nfft))));\n");
    fprintf(fid,"Hr = 20*log10(abs(fftshift(fft(hr,  nfft))));\n");
    fprintf(fid,"Hc = 20*log10(abs(fftshift(fft(hc/k,nfft))));\n");
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Ht,'LineWidth',1,'Color',[0.00 0.25 0.50],...\n");
    fprintf(fid,"     f,Hp,'LineWidth',1,'Color',[0.80 0.80 0.80],...\n");
    fprintf(fid,"     f,Hr,'LineWidth',1,'Color',[0.00 0.50 0.25],...\n");
    fprintf(fid,"     f,Hc,'LineWidth',2,'Color',[0.50 0.00 0.00],...\n");
    fprintf(fid,"     [-0.5/k 0.5/k], [1 1]*20*log10(0.5),'or');\n");
    fprintf(fid,"legend('transmit','ideal','receive','composite','alias points',1);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -100 20]);\n");

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"tr = [  -k*m:k*m]/k;\n");
    fprintf(fid,"tc = [-2*k*m:2*k*m]/k;\n");
    fprintf(fid,"ic = [0:k:(4*k*m)]+1;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(tr,ht,'-x', tr,hr,'-x');\n");
    fprintf(fid,"  legend('transmit','receive',1);\n");
    fprintf(fid,"  xlabel('Time');\n");
    fprintf(fid,"  ylabel('GMSK Tx/Rx Filters');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-2*m 2*m -0.3 1.2*max([hr ht])]);\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(tc,hc,'-x', tc(ic),hc(ic),'or');\n");
    fprintf(fid,"  xlabel('Time');\n");
    fprintf(fid,"  ylabel('GMSK Composite Response');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  axis([-2*m 2*m -0.2 1.2]);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    return 0;
}


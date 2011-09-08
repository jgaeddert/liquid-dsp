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

    // design receive filter
    // TODO : use 'optimal' method
    liquid_firdes_gmskrx(k,m,BT,0.0f,hr);

     // open output file
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
    fprintf(fid,"hr = zeros(1,h_len);\n");

    // print results
    for (i=0; i<h_len; i++)   fprintf(fid,"ht(%3u) = %12.4e;\n", i+1, ht[i]);
    for (i=0; i<h_len; i++)   fprintf(fid,"hr(%3u) = %12.4e;\n", i+1, hr[i]);
    
    fprintf(fid,"hc = conv(ht,hr);\n");

    // plot results
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Ht = 20*log10(abs(fftshift(fft(ht,nfft))));\n");
    fprintf(fid,"Hr = 20*log10(abs(fftshift(fft(hr,nfft))));\n");
    fprintf(fid,"Hc = 20*log10(abs(fftshift(fft(hc,nfft))));\n");
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Ht,'LineWidth',2,...\n");
    fprintf(fid,"     f,Hr,'LineWidth',1,...\n");
    fprintf(fid,"     f,Hc,'LineWidth',1);\n");
    fprintf(fid,"legend('transmit','receive','composite',0);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -100 20]);\n");

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"tr = [  -k*m:k*m]/k;\n");
    fprintf(fid,"tc = [-2*k*m:2*k*m]/k;\n");
    fprintf(fid,"plot(tr,hr,'-x', tc,hc,'-x');\n");
    fprintf(fid,"xlabel('Time');\n");
    fprintf(fid,"ylabel('Filter Impulse Response');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"legend('receive','composite',0);\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    return 0;
}


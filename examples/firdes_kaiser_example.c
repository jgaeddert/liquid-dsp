//
//
//

#include <stdio.h>

#include "liquid.h"

#define DEBUG 1
#define DEBUG_FILENAME "firdes_kaiser_example.m"

int main() {
    // options
    unsigned int h_len=37;
    float fc=0.4f;
    float slsl=40.0f;

    unsigned int i;
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,h);

#if DEBUG
    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n",h_len);
    fprintf(fid,"fc=%12.4e;\n",fc);
    fprintf(fid,"slsl=%12.4e;\n",slsl);
#endif

    for (i=0; i<h_len; i++) {
        printf("h(%4u) = %12.4e;\n", i+1, h[i]);
#if DEBUG
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);
#endif
    }   

#if DEBUG
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h*fc,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,H,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Filter design/Kaiser window f_c: %f, S_L: %f, h: %u']);\n",
            fc, -slsl, h_len);
    fprintf(fid,"axis([-0.5 0.5 -slsl-40 10]);\n");

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);
#endif

    printf("done.\n");
    return 0;
}


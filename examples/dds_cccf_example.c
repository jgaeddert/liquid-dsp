//
// Arbitrary resampler
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "dds_cccf_example.m"

int main() {
    // options
    float fc=0.17f;              // input (output) decim (interp) frequency
    float r=8.0f;               // resampling rate (output/input)
    //unsigned int n=128;         // number of input samples

    // create resampler
    dds_cccf q = dds_cccf_create(fc,r);
    dds_cccf_print(q);

    float complex x;
    float complex y[8*128];
    unsigned int nw;
    unsigned int i, n=0;
    for (i=0; i<128; i++) {
        //x = i < 100 ? 1.0f : 0.0f;
        x = i == 0 ? 1.0f : 0.0f;
        dds_cccf_execute(q, x, &y[n], &nw);
        n += nw;
    }

    // open/initialize output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"r=%12.8f;\n", r);


    // output results
    fprintf(fid,"\n\n");
    for (i=0; i<n; i++) {
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"plot(f,Y);\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n",OUTPUT_FILENAME);

    // clean up allocated objects
    dds_cccf_destroy(q);

    printf("done.\n");
    return 0;
}

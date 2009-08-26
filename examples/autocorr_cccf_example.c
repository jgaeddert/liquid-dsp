//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "autocorr_cccf_example.m"

int main() {
    srand(time(NULL));
    unsigned int n=32;  // number of training symbols
    float SNRdB=16.0f;  // signal-to-noise ratio (dB)

    // objects
    unsigned int num_samples = 3*n;
    autocorr_cccf q = autocorr_cccf_create(n,n);
    modem mod = modem_create(MOD_QPSK,2);

    FILE* fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid, "%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid, "clear all;\nclose all;\n\n");

    unsigned int i;

    // buffers
    float complex s[n]; // training sequence
    float complex x[num_samples];

    // generate random training sequence
    for (i=0; i<n; i++)
        modem_modulate(mod, rand()%4, &s[i]);

    // write training sequence twice, followed by zeros
    for (i=0; i<num_samples; i++)
        x[i] = (i<2*n) ? s[i%n] : 0.0f;

    // add noise
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples; i++)
        cawgn(&x[i],nstd);
        
    // write signal to output file
    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n",i+1,crealf(x[i]),cimagf(x[i]));

    // compute auto-correlation
    float complex rxx;
    for (i=0; i<num_samples; i++) {
        autocorr_cccf_push(q,x[i]);
        autocorr_cccf_execute(q,&rxx);

        fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rxx), cimagf(rxx));
    }

    fprintf(fid, "\n\n");

    fprintf(fid,"t=1:%u;\n",num_samples);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(x),t,imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal');\n");
    fprintf(fid,"legend('real','imag',0);\n");

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,abs(rxx));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('auto-correlation magnitude');\n");

    fclose(fid);

    printf("data written to %s\n", OUTPUT_FILENAME);

    // clean it up
    modem_destroy(mod);
    autocorr_cccf_destroy(q);

    printf("done.\n");
    return 0;
}

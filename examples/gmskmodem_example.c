// 
// gmskmodem_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "gmskmodem_example.m"

int main() {
    // options
    unsigned int k=4;                   // filter samples/symbol
    unsigned int m=3;                   // filter delay (symbols)
    float BT=0.3f;                      // bandwidth-time product
    unsigned int num_data_symbols = 32; // number of data symbols
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]
    float phi = 0.0f;                   // carrier phase offset
    float dphi = 0.0f;                  // carrier frequency offset

    // derived values
    unsigned int num_symbols = num_data_symbols + 2*m - 1;
    unsigned int num_samples = k*num_symbols;

    // create mod/demod objects
    gmskmodem mod   = gmskmodem_create(k, m, BT);
    gmskmodem demod = gmskmodem_create(k, m, BT);
    gmskmodem_print(mod);

    unsigned int i;
    unsigned int s[num_symbols];
    float complex x[num_samples];
    float complex y[num_samples];
    unsigned int sym_out[num_symbols];

    // generate random data sequence
    for (i=0; i<num_symbols; i++)
        s[i] = rand() % 2;

    // modulate signal
    for (i=0; i<num_symbols; i++)
        gmskmodem_modulate(mod, s[i], &x[k*i]);

    // add channel impairments
    // TODO : compensate for over-sampling rate?
    float nstd = powf(10.0f,-SNRdB*0.1f);
    for (i=0; i<num_samples; i++)
        y[i] = x[i]*cexpf(_Complex_I*(phi + i*dphi)) + nstd*randnf()*cexpf(_Complex_I*2*M_PI*randf());

    // demodulate signal
    for (i=0; i<num_symbols; i++)
        gmskmodem_demodulate(demod, &y[k*i], &sym_out[i]);

    // destroy modem objects
    gmskmodem_destroy(mod);
    gmskmodem_destroy(demod);

    // print results to screen
    unsigned int delay = 2*m-1;
    unsigned int num_errors=0;
    for (i=delay; i<num_symbols; i++) {
        //printf("  %4u : %2u (%2u)\n", i, s[i-delay], sym_out[i]);
        num_errors += (s[i-delay] == sym_out[i]) ? 0 : 1;
    }
    printf("symbol errors : %4u / %4u\n", num_errors, num_data_symbols);

    // write results to output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"t=[0:(num_samples-1)]/k;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");

    // artificially demodulate
    fprintf(fid,"h = exp(-([-k*m:k*m].^2)/4);\n");
    fprintf(fid,"h = h/sum(h) * pi / 2;\n");
    fprintf(fid,"z = filter(h,1,arg( ([y(2:end) 0]).*conj(y) )) / (h*h');\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,z,t(k:k:end),z(k:k:end),'x');\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    return 0;
}

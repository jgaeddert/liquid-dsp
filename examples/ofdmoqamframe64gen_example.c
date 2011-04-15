//
// ofdmoqamframe64gen_example.c
//
// This example demonstrates the ofdmoqamframe64 generator. The
// resulting sequence is decoded without channel impairments
// using the basic ofdmoqam analyzer.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "ofdmoqamframe64gen_example.m"

int main() {
    srand(time(NULL));

    // options
    unsigned int num_symbols_S0=3;      // num short sequence symbols
    unsigned int num_symbols_S1=2;      // num long sequence symbols
    unsigned int num_symbols_data=16;   // num data symbols
    unsigned int m=2;
    float beta = 0.9f;
    modulation_scheme ms = LIQUID_MODEM_QAM;
    unsigned int bps = 4;
    //float SNRdB=30.0f;      // signal-to-noise ratio (dB)

    unsigned int i,j;
    unsigned int num_symbols = num_symbols_S0 +
                               num_symbols_S1 +
                               num_symbols_data;
    unsigned int num_frames  = num_symbols + 2*m+1;
    unsigned int num_samples = 64*num_frames;

    printf("num symbols: %u\n", num_symbols);
    printf("num frames : %u\n", num_frames);
    printf("num samples: %u\n", num_samples);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"m      = %u;\n", m);
    fprintf(fid,"beta   = %f;\n", beta);
    fprintf(fid,"num_symbols_S0     = %u;\n", num_symbols_S0);
    fprintf(fid,"num_symbols_S1     = %u;\n", num_symbols_S1);
    fprintf(fid,"num_symbols_data   = %u;\n", num_symbols_data);
    fprintf(fid,"num_symbols    = %u;\n", num_symbols);
    fprintf(fid,"num_frames     = %u;\n", num_frames);
    fprintf(fid,"num_samples    = %u;\n", num_samples);
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,n);\n");
    fprintf(fid,"z = zeros(1,n);\n");

    // create modems
    modem mod   = modem_create(ms,bps);

    // create frame generator
    ofdmoqamframe64gen fg = ofdmoqamframe64gen_create(m,beta);
    ofdmoqamframe64gen_print(fg);

    // create receiver
    ofdmoqam fs = ofdmoqam_create(64,m,beta,0.0f/*dt*/,OFDMOQAM_ANALYZER,0/*gradient*/);

    float complex x[48];            // data buffer
    float complex y[num_samples];   // framegen output buffer
    float complex z[64];            // receiver output buffer

    unsigned int n=0;

    // write short sequence
    for (i=0; i<num_symbols_S0; i++) {
        ofdmoqamframe64gen_writeshortsequence(fg,&y[n]);
        n += 64;
    }

    // write long sequence
    for (i=0; i<num_symbols_S1; i++) {
        ofdmoqamframe64gen_writelongsequence(fg,&y[n]);
        n += 64;
    }

    unsigned int data_tx[48];
    // modulate data symbols
    for (i=0; i<num_symbols_data; i++) {

        // generate data sequence
        for (j=0; j<48; j++)
            data_tx[j] = modem_gen_rand_sym(mod);
        for (j=0; j<48; j++)
            modem_modulate(mod, data_tx[j], &x[j]);

        ofdmoqamframe64gen_writesymbol(fg,x,&y[n]);
        n += 64;
    }

    // flush
    for (i=0; i<2*m+1; i++) {
        ofdmoqamframe64gen_flush(fg,&y[n]);
        n += 64;
    }
    assert(n==num_samples);
    printf("n = %u\n", n);
    for (i=0; i<n; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1,crealf(y[i]),cimagf(y[i]));

    // add some noise
    //for (i=0; i<n; i++)
    //    y[i] += (randnf() + _Complex_I*randnf())* 0.01f;

    // run receiver (perfect timing, etc)
    //ofdmoqamframe64sync_execute(fs,z,num_samples);
    n=0;
    unsigned int frame=0;
    unsigned int k;
    fprintf(fid,"framesyms = zeros(num_frames,48);\n");
    for (i=0; i<num_frames; i++) {
        ofdmoqam_execute(fs,&y[n],z);
        // print results to output file
        k=0;
        for (j=0; j<64; j++) {
            int sctype = ofdmoqamframe64_getsctype(j);
            if (sctype == OFDMOQAMFRAME64_SCTYPE_DATA) {
                fprintf(fid,"framesyms(%4u,%4u) = %12.4e + j*%12.4e;\n", frame+1,k+1,crealf(z[j]),cimagf(z[j]));
                k++;
            }
        }
        assert(k==48);
        n+=64;
        frame++;
    }

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"%% normalize amplitude\n");
    fprintf(fid,"framesyms = framesyms*sqrt(52)/64;\n");
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('transmitted signal (clean)');\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"i_flush    = 1:(2*m+1);\n");
    fprintf(fid,"i_S0       = [1:num_symbols_S0]   + i_flush(end);\n");
    fprintf(fid,"i_S1       = [1:num_symbols_S1]   + i_S0(end);\n");
    fprintf(fid,"i_data     = [1:num_symbols_data] + i_S1(end);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"    plot(framesyms(i_flush,:),'x');\n");
    fprintf(fid,"    title('flushed symbols');\n");
    fprintf(fid,"    axis('square');\n");
    fprintf(fid,"    axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"    plot(framesyms(i_S0,:),'x');\n");
    fprintf(fid,"    title('S0 symbols');\n");
    fprintf(fid,"    axis('square');\n");
    fprintf(fid,"    axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"    plot(framesyms(i_S1,:),'x');\n");
    fprintf(fid,"    title('S1 symbols');\n");
    fprintf(fid,"    axis('square');\n");
    fprintf(fid,"    axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"    plot(framesyms(i_data,:),'x');\n");
    fprintf(fid,"    title('data symbols');\n");
    fprintf(fid,"    axis('square');\n");
    fprintf(fid,"    axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"    grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    ofdmoqamframe64gen_destroy(fg);
    ofdmoqam_destroy(fs);
    modem_destroy(mod);

    printf("done.\n");
    return 0;
}


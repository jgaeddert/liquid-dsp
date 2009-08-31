//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframe64gen_example.m"

int main() {
    srand(time(NULL));

    // options
    modulation_scheme ms = MOD_QPSK;
    unsigned int bps = 2;
    float cfo=0.2f;        // carrier frequency offset
    float cpo=M_PI/2.0f;    // carrier phase offset
    float SNRdB=30.0f;      // signal-to-noise ratio (dB)

    unsigned int i;

    // create frame generator
    ofdmframe64gen fg = ofdmframe64gen_create();
    ofdmframe64gen_print(fg);

    // create frame synchronizer
    ofdmframe64sync fs = ofdmframe64sync_create(NULL,NULL);
    ofdmframe64sync_print(fs);

    // create modem
    modem mod = modem_create(ms,bps);

    // channel impairments
    nco nco_rx = nco_create();
    nco_set_frequency(nco_rx,cfo);
    nco_set_phase(nco_rx,cpo);
    float nstd = powf(10.0f, -SNRdB/20.0f);

    float complex x[48];    // data buffer
    float complex y[160];   // framegen output buffer

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"y   = [];\n");

    unsigned int n=0;

    // write short sequence
    ofdmframe64gen_writeshortsequence(fg,y);
    for (i=0; i<160; i++) {
        nco_mix_up(nco_rx,y[i],&y[i]);
        cawgn(&y[i],nstd);
        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        n++;
    }

    ofdmframe64sync_execute(fs,y,160);

    // write long sequence
    ofdmframe64gen_writelongsequence(fg,y);
    for (i=0; i<160; i++) {
        nco_mix_up(nco_rx,y[i],&y[i]);
        cawgn(&y[i],nstd);
        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        n++;
    }

    ofdmframe64sync_execute(fs,y,160);

    // generate data sequence
    unsigned int s;
    for (i=0; i<48; i++) {
        s = modem_gen_rand_sym(mod);
        modem_modulate(mod, s, &x[i]);
    }
    ofdmframe64gen_writesymbol(fg,x,y);
    for (i=0; i<80; i++) {
        nco_mix_up(nco_rx,y[i],&y[i]);
        cawgn(&y[i],nstd);
        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        n++;
    }


    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(length(y)-1);\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    ofdmframe64gen_destroy(fg);
    ofdmframe64sync_destroy(fs);
    modem_destroy(mod);
    nco_destroy(nco_rx);

    printf("done.\n");
    return 0;
}


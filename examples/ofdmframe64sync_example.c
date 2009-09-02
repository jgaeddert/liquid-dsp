//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframe64sync_example.m"

int main() {
    srand(time(NULL));

    // options
    unsigned int num_symbols=6;
    modulation_scheme ms = MOD_QPSK;
    unsigned int bps = 2;
    float cfo=0.05f;         // carrier frequency offset
    float cpo=M_PI/7.0f;    // carrier phase offset
    float SNRdB=30.0f;      // signal-to-noise ratio (dB)
    unsigned int p=4;       // number of multi-path channel taps

    unsigned int i;
    unsigned int num_zeros=80;

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"y   = [];\n");

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
    float complex h[p];
    for (i=0; i<p; i++) {
        h[i] = (i==0) ? 1.0f : (randnf() + _Complex_I*randnf())*0.2f;
        printf("h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(h[i]),cimagf(h[i]));
        fprintf(fid,"h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(h[i]),cimagf(h[i]));
    }
    fir_filter_cccf fchannel = fir_filter_cccf_create(h,p);

    unsigned int num_samples = 160+160+80*num_symbols+num_zeros;
    float complex x[48];            // data buffer
    float complex y[num_samples];   // framegen output buffer
    float complex z[num_samples];   // framegen output buffer

    unsigned int n=0;

    // write short sequence
    ofdmframe64gen_writeshortsequence(fg,&y[n]);
    n += 160;

    // write long sequence
    ofdmframe64gen_writelongsequence(fg,&y[n]);
    n += 160;

    // generate data sequence
    unsigned int j;
    for (j=0; j<6; j++) {
        unsigned int s;
        for (i=0; i<48; i++) {
            s = modem_gen_rand_sym(mod);
            modem_modulate(mod, s, &x[i]);
        }
        ofdmframe64gen_writesymbol(fg,x,&y[n]);
        n += 80;
    }

    // pad end with zeros
    for (i=0; i<num_zeros; i++)
        y[n++] = 0.0f;
    // assert(n==num_samples);
    printf("n = %u\n", n);

    // add channel impairments
    n=0;
    for (i=0; i<num_samples; i++) {
        fir_filter_cccf_push(fchannel,y[i]);
        fir_filter_cccf_execute(fchannel,&z[i]);

        nco_mix_up(nco_rx,z[i],&z[i]);

        cawgn(&z[i],nstd);

        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(z[i]), cimagf(z[i]));
        n++;
    }

    ofdmframe64sync_execute(fs,z,num_samples);

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
    fir_filter_cccf_destroy(fchannel);

    printf("done.\n");
    return 0;
}


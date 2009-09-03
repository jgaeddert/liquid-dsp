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

// static data sequence
static unsigned int data_tx[48];

static int callback(float complex * _y, void * _userdata)
{
    unsigned int data_rx[48];
    modem demod = (modem)_userdata;
    unsigned int i, num_sym_errors=0;
    for (i=0; i<48; i++) {
        modem_demodulate(demod,_y[i],&data_rx[i]);
        num_sym_errors += (data_rx[i]==data_tx[i]) ? 0 : 1;
    }
    printf("callback invoked :: num symbol errors : %u / 48\n", num_sym_errors);
    return 0;
}

int main() {
    srand(time(NULL));

    // options
    unsigned int num_symbols=6;
    modulation_scheme ms = MOD_QPSK;
    unsigned int bps = 2;
    float cfo=0.03f;        // carrier frequency offset (max: pi/16 ~ 0.19)
    float cpo=M_PI/7.0f;    // carrier phase offset
    float SNRdB=30.0f;      // signal-to-noise ratio (dB)
    unsigned int p=4;       // number of multi-path channel taps
    float fstd=0.2f;        // multi-path channel taps standard deviation

    unsigned int i;
    unsigned int num_zeros=p;
    unsigned int num_samples = 160+160+80*num_symbols+num_zeros;

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,n);\n");
    fprintf(fid,"z = zeros(1,n);\n");
    fprintf(fid,"h = zeros(1,%u);\n", p+1);

    // create modems
    modem mod   = modem_create(ms,bps);
    modem demod = modem_create(ms,bps);

    // create frame generator
    ofdmframe64gen fg = ofdmframe64gen_create();
    ofdmframe64gen_print(fg);

    // create frame synchronizer
    ofdmframe64sync fs = ofdmframe64sync_create(callback,(void*)demod);
    ofdmframe64sync_print(fs);

    // channel impairments
    nco nco_rx = nco_create();
    nco_set_frequency(nco_rx,cfo);
    nco_set_phase(nco_rx,cpo);
    float nstd = powf(10.0f, -SNRdB/20.0f);
    float complex h[p+1];
    for (i=0; i<p+1; i++) {
        h[i] = (i==0) ? 1.0f : (randnf() + _Complex_I*randnf())*0.707f*fstd;
        printf("h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(h[i]),cimagf(h[i]));
        fprintf(fid,"h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(h[i]),cimagf(h[i]));
    }
    fir_filter_cccf fchannel = fir_filter_cccf_create(h,p+1);

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
    for (i=0; i<48; i++)
        data_tx[i] = modem_gen_rand_sym(mod);
    for (i=0; i<48; i++)
        modem_modulate(mod, data_tx[i], &x[i]);

    // modulate symbols
    for (i=0; i<num_symbols; i++) {
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
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('clean');\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(z),t,imag(z));\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('with channel impairments');\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"%% compute spectral periodigram\n");
    fprintf(fid,"nfft=256;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H = 20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"Z = zeros(1,nfft);\n");
    fprintf(fid,"v0=1; v1=v0+round(nfft); k=0;\n");
    fprintf(fid,"while v1 <= n,\n");
    fprintf(fid,"    Z += abs(fft(z(v0:[v1-1]).*hamming(v1-v0).',nfft))/(sqrt(nfft)*0.53910);\n");
    fprintf(fid,"    v0 = v0 + round(nfft/4);\n");
    fprintf(fid,"    v1 = v1 + round(nfft/4);\n");
    fprintf(fid,"    k = k+1;\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"Z = 20*log10(abs(fftshift(Z/k)));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Z,f,H);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"title('Multipath channel response');\n");

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


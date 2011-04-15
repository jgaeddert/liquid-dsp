//
// ofdmoqamframe64sync_example.c
//
// This example demonstrates the ofdmoqamframe64 synchronizer with
// arbitrary linear data modulation under a multi-path fading
// channel exhibiting additive white Gauss noise and carrier
// frequency offset.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmoqamframe64sync_example.m"

// simulation data structure
typedef struct {
    unsigned int * data_tx;         // original transmitted data
    modem demod;                    // demodulator
    unsigned int num_symbols;       // number of symbols
    unsigned int num_symbols_rx;    // number of symbols received
} simulation_data;

// static callback function invoked when each symbol received
static int callback(float complex * _y, void * _userdata)
{
    simulation_data * q = (simulation_data*) _userdata;
    unsigned int i;
    unsigned int num_sym_errors=0;          // number of symbol errors
    unsigned int n = 48*q->num_symbols_rx;  // array index counter
    unsigned int sym_rx;    // received, demodulated symbol
    for (i=0; i<48; i++) {
        modem_demodulate(q->demod,_y[i],&sym_rx);
        num_sym_errors += (sym_rx==q->data_tx[n+i]) ? 0 : 1;
    }
    printf("callback invoked [%3u] :: num symbol errors : %2u / 48\n", q->num_symbols_rx, num_sym_errors);
    q->num_symbols_rx++;

    // choose appropriate return value
    if (q->num_symbols_rx == q->num_symbols) {
        printf("frame received\n");
        return 1;
    }

    return 0;
}

int main() {
    srand(time(NULL));

    // options
    unsigned int num_symbols_S0=2;  // num short sequence symbols
    unsigned int num_symbols_S1=2;  // num long sequence symbols
    unsigned int num_symbols_S2=4;  // num training sequence symbols
    unsigned int num_symbols_data=16;// num data symbols
    unsigned int m=3;
    float beta = 0.7f;
    modulation_scheme ms = LIQUID_MODEM_QAM;
    unsigned int bps = 2;
    float cfo=0.0f;         // carrier frequency offset (max: pi/(2*64) ~ 0.024544)
    float cpo=0.0f;         // carrier phase offset
    float SNRdB=30.0f;      // signal-to-noise ratio (dB)
    unsigned int p=0;       // number of multi-path channel taps
    float fstd=0.2f;        // multi-path channel taps standard deviation
    unsigned int d=0;       // sample delay (noise samples before frame)

    unsigned int i;
    unsigned int num_symbols = num_symbols_S0 +
                               num_symbols_S1 +
                               num_symbols_S2 +
                               num_symbols_data;
    unsigned int num_frames  = num_symbols + 2*m + 1;
    unsigned int num_samples = 64*num_frames;

    printf("num symbols: %u\n", num_symbols);
    printf("num frames : %u\n", num_frames);
    printf("num samples: %u\n", num_samples);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,n);\n");
    fprintf(fid,"z = zeros(1,n);\n");
    fprintf(fid,"h = zeros(1,%u);\n", p+1);

    // create simulation data structure (pass to callback function)
    simulation_data simdata;
    simdata.num_symbols = num_symbols_data;
    simdata.num_symbols_rx = 0;
    simdata.demod = modem_create(ms,bps);
    simdata.data_tx = (unsigned int*)malloc(48*simdata.num_symbols*sizeof(unsigned int));

    // create modem
    modem mod = modem_create(ms,bps);

    // create frame generator
    ofdmoqamframe64gen fg = ofdmoqamframe64gen_create(m,beta);
    ofdmoqamframe64gen_print(fg);

    // create frame synchronizer
    ofdmoqamframe64sync fs = ofdmoqamframe64sync_create(m,beta,callback,(void*)&simdata);
    ofdmoqamframe64sync_print(fs);

    // channel impairments
    nco_crcf nco_rx = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_frequency(nco_rx,cfo);
    nco_crcf_set_phase(nco_rx,cpo);
    float nstd = powf(10.0f, -SNRdB/20.0f);
    float complex h[p+1];
    for (i=0; i<p+1; i++) {
        h[i] = (i==0) ? 1.0f : (randnf() + _Complex_I*randnf())*0.707f*fstd;
        printf("h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(h[i]),cimagf(h[i]));
        fprintf(fid,"h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(h[i]),cimagf(h[i]));
    }
    firfilt_cccf fchannel = firfilt_cccf_create(h,p+1);

    float complex x[48];            // data buffer
    float complex y[num_samples];   // framegen output buffer
    float complex z[num_samples];   // framegen output buffer

    unsigned int n=0;

    // write short sequence(s)
    for (i=0; i<num_symbols_S0; i++) {
        ofdmoqamframe64gen_writeshortsequence(fg,&y[n]);
        n += 64;
    }

    // write long sequence
    for (i=0; i<num_symbols_S1; i++) {
        ofdmoqamframe64gen_writelongsequence(fg,&y[n]);
        n += 64;
    }

    // write training sequence(s)
    for (i=0; i<num_symbols_S2; i++) {
        ofdmoqamframe64gen_writetrainingsequence(fg,&y[n]);
        n += 64;
    }

    // modulate data symbols
    for (i=0; i<num_symbols_data; i++) {

        // generate data sequence
        unsigned int j;
        for (j=0; j<48; j++) {
            simdata.data_tx[48*i+j] = modem_gen_rand_sym(mod);
            modem_modulate(mod, simdata.data_tx[48*i+j], &x[j]);
        }

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

    // add channel impairments
    n=0;
    for (i=0; i<num_samples; i++) {
        firfilt_cccf_push(fchannel,y[i]);
        firfilt_cccf_execute(fchannel,&z[i]);

        nco_crcf_mix_up(nco_rx,z[i],&z[i]);

        cawgn(&z[i],nstd);

        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(z[i]), cimagf(z[i]));
        n++;
    }

    // execute synchronizer on noise samples (delay);
    float complex noise;
    for (i=0; i<d; i++) {
        noise = 0.0f;
        cawgn(&noise,nstd);
        ofdmoqamframe64sync_execute(fs,&noise,1);
    }

    // execute synchronizer on data samples
    ofdmoqamframe64sync_execute(fs,z,num_samples);

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
    ofdmoqamframe64gen_destroy(fg);
    ofdmoqamframe64sync_destroy(fs);
    modem_destroy(mod);
    nco_crcf_destroy(nco_rx);
    firfilt_cccf_destroy(fchannel);

    // destroy simulation data object internals
    modem_destroy(simdata.demod);
    free(simdata.data_tx);

    printf("done.\n");
    return 0;
}


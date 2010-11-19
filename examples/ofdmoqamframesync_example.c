//
// ofdmoqamframesync_example.c
//
// This example demonstrates the ofdmoqamframe synchronizer with
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

#define OUTPUT_FILENAME "ofdmoqamframesync_example.m"

// simulation data structure
typedef struct {
    unsigned int num_channels;      // number of subcarriers
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
    unsigned int n = q->num_channels*q->num_symbols_rx;  // array index counter
    unsigned int sym_rx;    // received, demodulated symbol
    for (i=0; i<q->num_channels; i++) {
        modem_demodulate(q->demod,_y[i],&sym_rx);
        num_sym_errors += (sym_rx==q->data_tx[n+i]) ? 0 : 1;
    }
    printf("callback invoked [%3u] :: num symbol errors : %2u / num_channels\n", q->num_symbols_rx, num_sym_errors);
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
    unsigned int num_channels = 80; // number of subcarriers
    unsigned int num_symbols_S0=6;  // num short sequence symbols
    unsigned int num_symbols_S1=1;  // num long sequence symbols
    unsigned int num_symbols_data=8;// num data symbols
    unsigned int m=3;
    float beta = 0.7f;
    modulation_scheme ms = MOD_QAM;
    unsigned int bps = 2;
    float cfo=0.0f;         // carrier frequency offset (max: pi/(2*num_channels) ~ 0.024544)
    float cpo=0.0f;         // carrier phase offset
    float SNRdB=30.0f;      // signal-to-noise ratio (dB)
    unsigned int hc_len=0;  // number of multi-path channel taps
    float fstd=0.2f;        // multi-path channel taps standard deviation
    unsigned int d=0;       // sample delay (noise samples before frame)

    unsigned int i;
    unsigned int num_symbols = num_symbols_S0 +
                               num_symbols_S1 +
                               num_symbols_data;
    unsigned int num_frames  = num_symbols + 2*m + 1;
    unsigned int num_samples = num_channels*num_frames;

    printf("num symbols: %u\n", num_symbols);
    printf("num frames : %u\n", num_frames);
    printf("num samples: %u\n", num_samples);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,n);\n");
    fprintf(fid,"z = zeros(1,n);\n");
    fprintf(fid,"h = zeros(1,%u);\n", hc_len+1);

    // create simulation data structure (pass to callback function)
    simulation_data simdata;
    simdata.num_channels = num_channels;
    simdata.num_symbols = num_symbols_data;
    simdata.num_symbols_rx = 0;
    simdata.demod = modem_create(ms,bps);
    simdata.data_tx = (unsigned int*)malloc(num_channels*simdata.num_symbols*sizeof(unsigned int));

    // create modem
    modem mod = modem_create(ms,bps);

    // create frame generator
    ofdmoqamframegen fg = ofdmoqamframegen_create(num_channels,m,beta,NULL);
    ofdmoqamframegen_print(fg);

    // create frame synchronizer
    ofdmoqamframesync fs = ofdmoqamframesync_create(num_channels,m,beta,NULL,callback,(void*)&simdata);
    ofdmoqamframesync_print(fs);

    // channel impairments
    nco_crcf nco_rx = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_frequency(nco_rx,cfo);
    nco_crcf_set_phase(nco_rx,cpo);
    float nstd = powf(10.0f, -SNRdB/20.0f);
    float complex hc[hc_len+1];
    for (i=0; i<hc_len+1; i++) {
        hc[i] = (i==0) ? 1.0f : (randnf() + _Complex_I*randnf())*0.707f*fstd;
        printf("h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(hc[i]),cimagf(hc[i]));
        fprintf(fid,"h(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(hc[i]),cimagf(hc[i]));
    }
    firfilt_cccf fchannel = firfilt_cccf_create(hc,hc_len+1);

    float complex x[num_channels];  // data buffer
    float complex y[num_samples];   // framegen output buffer
    float complex z[num_samples];   // framegen output buffer

    unsigned int n=0;

    // write short sequence(s)
    for (i=0; i<num_symbols_S0; i++) {
        ofdmoqamframegen_writeshortsequence(fg,&y[n]);
        n += num_channels;
    }

    // write long sequence
    for (i=0; i<num_symbols_S1; i++) {
        ofdmoqamframegen_writelongsequence(fg,&y[n]);
        n += num_channels;
    }

    // modulate data symbols
    for (i=0; i<num_symbols_data; i++) {

        // generate data sequence
        unsigned int j;
        for (j=0; j<num_channels; j++) {
            simdata.data_tx[num_channels*i+j] = modem_gen_rand_sym(mod);
            modem_modulate(mod, simdata.data_tx[num_channels*i+j], &x[j]);
        }

        ofdmoqamframegen_writesymbol(fg,x,&y[n]);
        n += num_channels;
    }

    // flush
    for (i=0; i<2*m+1; i++) {
        ofdmoqamframegen_flush(fg,&y[n]);
        n += num_channels;
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
        ofdmoqamframesync_execute(fs,&noise,1);
    }

    // execute synchronizer on data samples
#if 0
    ofdmoqamframesync_execute(fs,z,800);
#else
    ofdmoqamframesync_execute(fs,z,num_samples);
#endif

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"title('clean');\n");
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,real(z),t,imag(z));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('signal');\n");
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
    fprintf(fid,"axis([-0.5 0.5 -40 10]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"title('Multipath channel response');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    ofdmoqamframegen_destroy(fg);
    ofdmoqamframesync_destroy(fs);
    modem_destroy(mod);
    nco_crcf_destroy(nco_rx);
    firfilt_cccf_destroy(fchannel);

    // destroy simulation data object internals
    modem_destroy(simdata.demod);
    free(simdata.data_tx);

    printf("done.\n");
    return 0;
}


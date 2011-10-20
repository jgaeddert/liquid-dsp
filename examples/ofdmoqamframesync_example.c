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
#include <getopt.h>
#include <assert.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmoqamframesync_example.m"

void usage()
{
    printf("ofdmoqamframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  k     : number of channels, default: 80\n");
    printf("  m     : filter delay [symbols], default: 3\n");
    printf("  b     : filter excess bandwidth, in [0,1], default: 0.7\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
    printf("  P     : carrier phase offset [radians], default: 0\n");
    printf("  F     : carrier frequency offset [radians/sample], default: 0\n");
    printf("  d     : delay [samples], default: 0\n");
}


// simulation data structure
typedef struct {
    unsigned int num_channels;      // number of subcarriers
    unsigned char * p;              // subcarrier allocation
    unsigned int M_null;            // number of null subcarriers
    unsigned int M_pilot;           // number of pilot subcarriers
    unsigned int M_data;            // number of data subcarriers
    unsigned int * data_tx;         // original transmitted data, [size: M x num_symbols]
    modem demod;                    // demodulator
    unsigned int num_symbols;       // number of symbols
    unsigned int num_symbols_rx;    // number of symbols received
} simulation_data;

// static callback function invoked when each symbol received
static int callback(float complex * _y, void * _userdata)
{
    simulation_data * q = (simulation_data*) _userdata;

    // check to see if frame has already been received
    if (q->num_symbols_rx >= q->num_symbols) {
        printf("callback invoked :: ignoring symbol\n");
        return 0;
    }

    unsigned int i;
    unsigned int num_sym_errors=0;          // number of symbol errors
    unsigned int n = q->num_channels*q->num_symbols_rx;  // array index counter
    unsigned int sym_rx;    // received, demodulated symbol

    int sctype;
    for (i=0; i<q->num_channels; i++) {
        sctype = q->p[i];
        if (sctype == OFDMOQAMFRAME_SCTYPE_DATA) {
            modem_demodulate(q->demod,_y[i],&sym_rx);
            num_sym_errors += (sym_rx==q->data_tx[n+i]) ? 0 : 1;
        } else {
            // not a data subcarrier
        }
    }
    printf("callback invoked [%3u] :: num symbol errors : %2u / %u\n", q->num_symbols_rx, num_sym_errors, q->M_data);
    q->num_symbols_rx++;

    // choose appropriate return value
    if (q->num_symbols_rx == q->num_symbols) {
        printf("frame received\n");
        //return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // options
    unsigned int num_channels = 80; // number of subcarriers
    unsigned int num_symbols_S0=7;  // num short sequence symbols (2*m + 1)
    unsigned int num_symbols_S1=1;  // num long sequence symbols
    unsigned int num_symbols_data=8;// num data symbols
    unsigned int m=3;
    float beta = 0.9f;
    modulation_scheme ms = LIQUID_MODEM_QAM;
    unsigned int bps = 2;
    float dphi=0.0f;        // carrier frequency offset (max: pi/(2*num_channels) ~ 0.024544)
    float phi=0.0f;         // carrier phase offset
    float SNRdB=30.0f;      // signal-to-noise ratio (dB)
    unsigned int hc_len=0;  // number of multi-path channel taps
    float fstd=0.2f;        // multi-path channel taps standard deviation
    unsigned int d=0;       // sample delay (noise samples before frame)

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"uhk:m:b:s:P:F:d:")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'k': num_channels = atoi(optarg);  break;
        case 'm': m = atoi(optarg);             break;
        case 'b': beta = atof(optarg);          break;
        case 's': SNRdB = atof(optarg);         break;
        case 'P': phi = atof(optarg);           break;
        case 'F': dphi = atof(optarg);          break;
        case 'd': d = atoi(optarg);             break;
        default:
            exit(1);
        }
    }

    // validate input
    if (num_channels < 8) {
        fprintf(stderr,"error: %s, number of subcarriers exceeds minimum\n", argv[0]);
        exit(1);
    } else if ( (num_channels%2) == 1) {
        fprintf(stderr,"error: %s, number of subcarriers must be even\n", argv[0]);
        exit(1);
    } else if (m == 0) {
        fprintf(stderr,"error: %s, filter delay must be greater than zero\n", argv[0]);
        exit(1);
    } else if (beta < 0.0f || beta > 1.0f) {
        fprintf(stderr,"error: %s, filter excess banwidth must be in [0,1]\n", argv[0]);
        exit(1);
    } else if (d > 10000) {
        fprintf(stderr,"error: %s, delay is too large\n", argv[0]);
        exit(1);
    }

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
    simdata.data_tx = (unsigned int*)malloc(num_channels*num_symbols_data*sizeof(unsigned int));
    simdata.p       = (unsigned char*)malloc(num_channels*sizeof(unsigned char));

    ofdmoqamframe_init_default_sctype(simdata.num_channels, simdata.p);

#if 0
    // notch the spectrum
    for (i=12; i<24; i++)
        simdata.p[i] = OFDMOQAMFRAME_SCTYPE_NULL;
#endif

    ofdmoqamframe_validate_sctype(simdata.p,
                                  simdata.num_channels,
                                  &simdata.M_null,
                                  &simdata.M_pilot,
                                  &simdata.M_data);

    // create modem
    modem mod = modem_create(ms,bps);

    // create frame generator
    ofdmoqamframegen fg = ofdmoqamframegen_create(num_channels,m,beta,simdata.p);
    ofdmoqamframegen_print(fg);

    // create frame synchronizer
    ofdmoqamframesync fs = ofdmoqamframesync_create(num_channels,m,beta,simdata.p,callback,(void*)&simdata);
    ofdmoqamframesync_print(fs);

    // channel impairments
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

        // add carrier phase/frequency offset
        z[i] *= 1.0f*cexpf(_Complex_I*(phi + i*dphi));

        // add noise
        z[i] += nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;

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
    ofdmoqamframesync_execute(fs,z,720);
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
    firfilt_cccf_destroy(fchannel);

    // destroy simulation data object internals
    modem_destroy(simdata.demod);
    free(simdata.data_tx);
    free(simdata.p);

    printf("done.\n");
    return 0;
}


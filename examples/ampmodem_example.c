// 
// ampmodem_test.c
//
// Tests simple modulation/demodulation of the ampmodem (analog
// amplitude modulator/demodulator) with noise, carrier phase,
// and carrier frequency offsets.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ampmodem_example.m"

// print usage/help message
void usage()
{
    printf("ampmodem_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  f     : frequency offset [default: 0.02]\n");
    printf("  p     : phase offset [default: pi/4]\n");
    printf("  n     : number of samples [default: 256]\n");
    printf("  s     : SNR (dB) [default: 20]\n");
    printf("  t     : AM type (ssb/dsb) [default: ssb]\n");
}

int main(int argc, char*argv[]) {
    // options
    float mod_index = 0.1f;         // modulation index (bandwidth)
    //float fc = 0.1371f*2.0f*M_PI;   // AM carrier
    float cfo = 0.02f;              // carrier frequency offset
    float cpo = M_PI / 4.0f;        // carrier phase offset
    unsigned int num_samples = 256; // number of samples
    float SNRdB = 20.0f;            // signal-to-noise ratio [dB]
    liquid_modem_amtype type = LIQUID_MODEM_AM_SSB;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhf:p:n:s:t:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'f':   cfo = atof(optarg); break;
        case 'p':   cpo = atof(optarg); break;
        case 'n':   num_samples = atoi(optarg); break;
        case 's':   SNRdB = atof(optarg);       break;
        case 't':
            if (strcmp(optarg,"ssb")==0) {
                type = LIQUID_MODEM_AM_SSB;
            } else if (strcmp(optarg,"dsb")==0) {
                type = LIQUID_MODEM_AM_DSB;
            } else {
                fprintf(stderr,"error: ampmodem_example, invalid AM type: %s\n", optarg);
                usage();
                return 1;
            }
            break;
        default:
            fprintf(stderr,"error: ampmodem_example, unknown option\n");
            usage();
            return 1;
        }
    }

    // create mod/demod objects
    ampmodem mod   = ampmodem_create(mod_index,type);
    ampmodem demod = ampmodem_create(mod_index,type);
    ampmodem_print(mod);

    unsigned int i;
    float x[num_samples];
    float complex y[num_samples];
    float z[num_samples];

    // generate un-modulated signal (band-limited pulse)
    fir_kaiser_window(num_samples, 0.1f, -40.0f, 0.0f, x);

    // modulate signal
    for (i=0; i<num_samples; i++)
        ampmodem_modulate(mod, x[i], &y[i]);

    // add channel impairments
    nco nco_channel = nco_create(LIQUID_VCO);
    nco_set_frequency(nco_channel, cfo);
    nco_set_phase(nco_channel, cpo);
    float nstd = powf(10.0f,-SNRdB*0.1f);
    for (i=0; i<num_samples; i++) {
        cawgn(&y[i], nstd);
        nco_mix_up(nco_channel, y[i], &y[i]);
        nco_step(nco_channel);
    }
    nco_destroy(nco_channel);

    // demodulate signal
    for (i=0; i<num_samples; i++)
        ampmodem_demodulate(demod, y[i], &z[i]);

    // write results to output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n=%u;\n",num_samples);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u) = %12.4e;\n", i+1, z[i]);
    }
    // plot results
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,t,z);\n");
    fprintf(fid,"axis([0 n -0.4 1.2]);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"legend('original','demodulated',1);\n");
    fprintf(fid,"grid on;\n");
    // spectrum
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"Y = Y - max(Y);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y);\n");
    fprintf(fid,"axis([-0.5 0.5 -60 10]);\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    ampmodem_destroy(mod);
    ampmodem_destroy(demod);
    return 0;
}

const char __docstr__[] =
"Tests simple modulation/demodulation of the ampmodem (analog"
" amplitude modulator/demodulator) with noise, carrier phase,"
" and carrier frequency offsets.";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "ampmodem_example.m",'o', "output filename", NULL);
    liquid_argparse_add(float,    mod_index,          0.8f,  'm', "modulation index (bandwidth)", NULL);
    liquid_argparse_add(float,    dphi,               0.05f, 'f', "carrier frequency offset [radians/sample]", NULL);
    liquid_argparse_add(float,    phi,                2.8f,  'p', "carrier phase offset [radians]", NULL);
    liquid_argparse_add(float,    SNRdB,              30.0f, 'S', "signal-to-noise ratio (set very high for testing)", NULL);
    liquid_argparse_add(unsigned, num_samples,        2400,  'n', "number of samples", NULL);
    liquid_argparse_add(char*,    mod_type,           "usb", 't', "modulation type: dsb/usb/lsb", NULL);
    liquid_argparse_add(bool,     suppressed_carrier, 0,     's', "enable carrier suppression", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    liquid_ampmodem_type type = LIQUID_AMPMODEM_USB;
    if (strcmp(mod_type,"dsb")==0) {
        type = LIQUID_AMPMODEM_DSB;
    } else if (strcmp(mod_type,"usb")==0) {
        type = LIQUID_AMPMODEM_USB;
    } else if (strcmp(mod_type,"lsb")==0) {
        type = LIQUID_AMPMODEM_LSB;
    } else {
        return liquid_error(LIQUID_EICONFIG,"invalid AM type: %s", mod_type);
    }

    // create mod/demod objects
    ampmodem mod   = ampmodem_create(mod_index, type, suppressed_carrier);
    ampmodem demod = ampmodem_create(mod_index, type, suppressed_carrier);
    unsigned int delay = ampmodem_get_delay_mod(mod) + ampmodem_get_delay_demod(demod);
    ampmodem_print(mod);

    unsigned int i;
    LIQUID_VLA(float, x, num_samples);
    LIQUID_VLA(liquid_float_complex, y, num_samples);
    LIQUID_VLA(float, z, num_samples);

    // generate 'audio' signal (simple windowed sum of tones)
    unsigned int nw = (unsigned int)(0.90*num_samples); // window length
    unsigned int nt = (unsigned int)(0.05*num_samples); // taper length
    for (i=0; i<num_samples; i++) {
        x[i] =  0.6f*cos(2*M_PI*0.0202*i);
        x[i] += 0.4f*cos(2*M_PI*0.0271*i);
        x[i] *= i < nw ? liquid_rcostaper_window(i,nw,nt) : 0;
    }

    // modulate signal
    for (i=0; i<num_samples; i++)
        ampmodem_modulate(mod, x[i], &y[i]);

    // add channel impairments
    float nstd = powf(10.0f,-SNRdB/20.0f);
    for (i=0; i<num_samples; i++) {
        y[i] *= cexpf(_Complex_I*phi);
        y[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // update phase
        phi += dphi;
        while (phi >  M_PI) phi -= 2*M_PI;
        while (phi < -M_PI) phi += 2*M_PI;
    }

    // demodulate signal
    for (i=0; i<num_samples; i++)
        ampmodem_demodulate(demod, y[i], &z[i]);

    // destroy objects
    ampmodem_destroy(mod);
    ampmodem_destroy(demod);

    // compute demodulation error
    float rmse = 0.0f;
    for (i=delay; i<num_samples; i++)
        rmse += (x[i-delay] - z[i]) * (x[i-delay] - z[i]);
    rmse = 10*log10( rmse / (float)(num_samples-delay) );
    printf("rms error : %.3f dB\n", rmse);

    // export results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n=%u;\n",num_samples);
    fprintf(fid,"delay=%u;\n", delay);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"z(%3u) = %12.4e;\n", i+1, z[i]);
    }
    // plot results
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure('position',[100 100 800 600]);\n");
    // message signals
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,x,t-delay,z);\n");
    fprintf(fid,"  axis([-delay n -1.2 1.2]);\n");
    fprintf(fid,"  xlabel('Time [sample index]');\n");
    fprintf(fid,"  ylabel('Message Signal');\n");
    fprintf(fid,"  legend('original','demodulated');\n");
    fprintf(fid,"  grid on;\n");
    // rf signal
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"  axis([-delay n -1.8 1.8]);\n");
    fprintf(fid,"  xlabel('Time [sample index]');\n");
    fprintf(fid,"  ylabel('RF Signal');\n");
    fprintf(fid,"  legend('real','imag');\n");
    fprintf(fid,"  grid on;\n");
    // spectrum
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  nfft=2^nextpow2(n);\n");
    fprintf(fid,"  f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"  Y = 20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"  Y = Y - max(Y);\n");
    fprintf(fid,"  plot(f,Y);\n");
    fprintf(fid,"  axis([-0.1 0.1 -60 10]);\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Received PSD [dB]');\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s\n", filename);
    return 0;
}

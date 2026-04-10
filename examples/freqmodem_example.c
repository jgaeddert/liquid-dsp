const char __docstr__[] = "Tests simple frequency modulation/demodulation";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "freqmodem_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_samples, 1024, 'n', "modulation factor", NULL);
    liquid_argparse_add(float,    kf,          0.1f, 'k', "number of samples", NULL);
    liquid_argparse_add(float,    SNRdB,         30, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    // create mod/demod objects
    freqmod mod = freqmod_create(kf);   // modulator
    freqdem dem = freqdem_create(kf);   // demodulator
    freqmod_print(mod);

    unsigned int i;
    LIQUID_VLA(float, m, num_samples);       // message signal
    LIQUID_VLA(liquid_float_complex, r, num_samples);       // received signal (complex baseband)
    LIQUID_VLA(float, y, num_samples);       // demodulator output

    // generate message signal (sum of sines)
    for (i=0; i<num_samples; i++) {
        m[i] = 0.3f*cosf(2*M_PI*0.013f*i + 0.0f) +
               0.2f*cosf(2*M_PI*0.021f*i + 0.4f) +
               0.4f*cosf(2*M_PI*0.037f*i + 1.7f);
    }

    // modulate signal
    freqmod_modulate_block(mod, m, num_samples, r);

    // add channel impairments
    float nstd = powf(10.0f,-SNRdB/20.0f);
    for (i=0; i<num_samples; i++)
        r[i] += nstd*( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;

    // demodulate signal
    freqdem_demodulate_block(dem, r, num_samples, y);

    // destroy modem objects
    freqmod_destroy(mod);
    freqdem_destroy(dem);

    // compute RMS error (ignore first sample)
    float rmse = 0.0;
    for (i=1; i<num_samples; i++) {
        float err = y[i] - m[i];
        rmse += err*err;
    }
    rmse = sqrtf( rmse / (float)(num_samples-1) );
    printf("rmse = %12.4e\n", rmse);

    // 
    // write results to output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"n=%u;\n",num_samples);
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"m(%3u) = %12.4e;\n", i+1, m[i]);
        fprintf(fid,"r(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(r[i]), cimagf(r[i]));
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y[i]);
    }
    // plot time-domain result
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,m,'LineWidth',1,'Color',[0 0.2 0.5],...);\n");
    fprintf(fid,"       t,y,'LineWidth',1,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  axis([0 n -1.2 1.2]);\n");
    fprintf(fid,"  xlabel('Normalized Time [t/T_s]');\n");
    fprintf(fid,"  ylabel('m(t), y(t)');\n");
    fprintf(fid,"  grid on;\n");
    // compute spectral responses
    fprintf(fid,"nfft=2^(1+nextpow2(n));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"w = hamming(n)';\n");
    fprintf(fid,"g = 1 / (mean(w) * n);\n");
    fprintf(fid,"M = 20*log10(abs(fftshift(fft(m.*w*g,nfft))));\n");
    fprintf(fid,"R = 20*log10(abs(fftshift(fft(r.*w*g,nfft))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y.*w*g,nfft))));\n");
    // plot spectral response (audio)
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(f,M,'LineWidth',1.2,'Color',[0 0.2 0.5],...\n");
    fprintf(fid,"       f,Y,'LineWidth',1.2,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('Audio PSD [dB]');\n");
    // plot spectral response (RF)
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(f,R,'LineWidth',1.2,'Color',[0.5 0 0]);\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"  ylabel('RF PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s\n", filename);

    return 0;
}

const char __docstr__[] = "Halfband filter example.";

#include <stdio.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "resamp2_crcf_filter_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, m,           5,      'm', "filter semi-length (actual length: 4*m+1)", NULL);
    liquid_argparse_add(unsigned, num_samples, 37,     'n', "number of input samples", NULL);
    liquid_argparse_add(float,    As,          60.0,   'a', "stop-band attenuation [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int h_len = 4*m+1;     // half-band filter length
    unsigned int N = num_samples + h_len;

    // arrays
    LIQUID_VLA(liquid_float_complex, x, N);             // input time series
    LIQUID_VLA(liquid_float_complex, y0, N);            // output time series (lower band)
    LIQUID_VLA(liquid_float_complex, y1, N);            // output time series (upper band)

    // generate input sequence
    unsigned int i;
    for (i=0; i<N; i++) {
        //x[i] = randnf() * cexpf(_Complex_I*M_PI*randf());
        x[i] = cexpf(_Complex_I*2*M_PI*0.072f*i) + 0.6f*cexpf(_Complex_I*2*M_PI*0.37f*i);
        x[i] *= (i < num_samples) ? liquid_hamming(i,num_samples) : 0.0f;
    }

    // create/print the half-band filter with a specified
    // stop-band attenuation
    resamp2_crcf q = resamp2_crcf_create(m,0.0f,As);
    resamp2_crcf_print(q);

    for (i=0; i<N; i++) {
        // run the filter
        resamp2_crcf_filter_execute(q, x[i], &y0[i], &y1[i]);

        //printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
    }

    // clean up allocated objects
    resamp2_crcf_destroy(q);

    // export output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"N = %u;\n", N);
    fprintf(fid,"t = 0:(N-1);\n");

    // save results to output file
    for (i=0; i<N; i++) {
        fprintf(fid,"x(%3u)  = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]),  cimagf(x[i]));
        fprintf(fid,"y0(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y0[i]), cimagf(y0[i]));
        fprintf(fid,"y1(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y1[i]), cimagf(y1[i]));
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,    real(x), 'LineWidth',2,'Color',[0.50 0.50 0.50],...\n");
    fprintf(fid,"       t-2*m,real(y0),'LineWidth',1,'Color',[0.00 0.25 0.50],...\n");
    fprintf(fid,"       t-2*m,real(y1),'LineWidth',1,'Color',[0.00 0.50 0.25]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('original','lower band','upper band');");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,    imag(x), 'LineWidth',2,'Color',[0.50 0.50 0.50],...\n");
    fprintf(fid,"       t-2*m,imag(y0),'LineWidth',1,'Color',[0.00 0.25 0.50],...\n");
    fprintf(fid,"       t-2*m,imag(y1),'LineWidth',1,'Color',[0.00 0.50 0.25]);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('original','lower band','upper band',1);");

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"g = 1 / sqrt( real(x*x') );\n");
    fprintf(fid,"X =20*log10(abs(fftshift(fft(x*g, nfft))));\n");
    fprintf(fid,"Y0=20*log10(abs(fftshift(fft(y0*g,nfft))));\n");
    fprintf(fid,"Y1=20*log10(abs(fftshift(fft(y1*g,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,X, 'LineWidth',2,'Color',[0.50 0.50 0.50],...\n");
    fprintf(fid,"     f,Y0,'LineWidth',1,'Color',[0.00 0.25 0.50],...\n");
    fprintf(fid,"     f,Y1,'LineWidth',1,'Color',[0.00 0.50 0.25]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','lower band','upper band',2);");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

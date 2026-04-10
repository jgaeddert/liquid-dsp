const char __docstr__[] =
"This example demonstrates the interface to the iirdecim (infinite"
" impulse response decimator) family of objects.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "iirdecim_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, M,             4, 'M', "decimation rate", NULL);
    liquid_argparse_add(unsigned, num_samples, 400, 'n', "number of input samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    if (M < 2)
        return liquid_error(LIQUID_EICONFIG,"decim factor must be greater than 1");
    if (num_samples < 1)
        return liquid_error(LIQUID_EICONFIG,"must have at least one sample");

    // ensure number of samples is divisible by M
    num_samples += (num_samples % M);

    // create decimator from prototype
    unsigned int order = 8;
    iirdecim_crcf q = iirdecim_crcf_create_default(M,order);

    // compute group delay
    float delay = iirdecim_crcf_groupdelay(q,0.0f);

    // generate input signal and decimate
    LIQUID_VLA(liquid_float_complex, x, num_samples);   // input samples
    LIQUID_VLA(liquid_float_complex, y, num_samples/M); // output samples
    unsigned int i;
    unsigned int w_len = num_samples > 4*delay ? num_samples - 4*delay : num_samples;
    for (i=0; i<num_samples; i++) {
        x[i] =  cexpf(_Complex_I*(2*M_PI*0.03*i - 0.3*i*i/(float)num_samples));
        x[i] *= i < w_len ? /*liquid_hamming(i,w_len)*/ 1.0f : 0.0f;
    }

    // decimate input
    for (i=0; i<num_samples/M; i++)
        iirdecim_crcf_execute(q, &x[M*i], &y[i]);

    // destroy decimator object
    iirdecim_crcf_destroy(q);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"delay = %f;\n", delay);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples/M);\n");

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<num_samples/M; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"tx = [0:(num_samples  -1)];\n");
    fprintf(fid,"ty = [0:(num_samples/M-1)]*M - delay;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(tx,real(x),'-s','MarkerSize',1,ty,real(y),'-s','MarkerSize',3);\n");
    fprintf(fid,"    legend('input','decim','location','northeast');\n");
    fprintf(fid,"    axis([0 num_samples -1.2 1.2]);\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('real');\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(tx,imag(x),'-s','MarkerSize',1,ty,imag(y),'-s','MarkerSize',3);\n");
    fprintf(fid,"    legend('input','decim','location','northeast');\n");
    fprintf(fid,"    axis([0 num_samples -1.2 1.2]);\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('imag');\n");
    fprintf(fid,"    grid on;\n");
    
    // plot spectral response
    fprintf(fid,"nfft = 2048;\n");
    fprintf(fid,"X  = 20*log10(abs(fftshift(fft(x/length(x),nfft))));\n");
    fprintf(fid,"Y  = 20*log10(abs(fftshift(fft(y/length(y),nfft))));\n");
    fprintf(fid,"fx = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"fy = fx / M;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(fx,X,'-','Color',[0.5 0.5 0.5],...\n");
    fprintf(fid,"     fy,Y,'-','Color',[0.0 0.5 0.3],'LineWidth',2);\n");
    fprintf(fid,"legend('input','decim','location','northeast');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/Fs]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"axis([-0.5 0.5 -100 0]);\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}

const char __docstr__[] =
"Demonstration of resamp object whereby an input noise signal"
" is resampled at a rate 'r'.";

#include <stdio.h>
#include <stdlib.h>
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
    liquid_argparse_add(char*, filename, "resamp_crcf_noise_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    r,   0.60, 'r', "resampling rate (output/input)", NULL);
    liquid_argparse_add(unsigned, m,     13, 'm', "resampling filter semi-length (filter delay)", NULL);
    liquid_argparse_add(float,    As,  60.0, 'a', "resampling filter stop-band attenuation [dB]", NULL);
    liquid_argparse_add(float,    bw,    -1, 'w', "resampling filter bandwidth (normalized)", NULL);
    liquid_argparse_add(unsigned, npfb,  64, 'b', "number of filters in bank (timing resolution)", NULL);
    liquid_argparse_add(unsigned, n,    400, 'n', "number of input samples", NULL);
    liquid_argparse_add(float,    fc, 0.044, 'f', "complex sinusoid frequency", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (r <= 0.0f)
        return liquid_error(LIQUID_EICONFIG,"resampling rate must be greater than zero");
    if (m == 0)
        return liquid_error(LIQUID_EICONFIG,"filter semi-length must be greater than zero");
    if (bw >= 0.5f)
        return liquid_error(LIQUID_EICONFIG,"filter bandwidth must be less than 0.5");
    if (As < 0.0f)
        return liquid_error(LIQUID_EICONFIG,"filter stop-band attenuation must be greater than zero");
    if (npfb == 0)
        return liquid_error(LIQUID_EICONFIG,"filter bank size must be greater than zero");
    if (n == 0)
        return liquid_error(LIQUID_EICONFIG,"number of input samples must be greater than zero");

    // set default bandwidth if not otherwise specified
    if (bw < 0)
        bw = 0.35f*r;

    // create resampler object
    resamp_crcf q = resamp_crcf_create(r,m,bw,As,256);
    //resamp_crcf_print(q);

    // number of sample blocks
    unsigned int num_blocks = 1000;

    // arrays
    unsigned int  buf_len = 1024;
    LIQUID_VLA(liquid_float_complex, buf_x, buf_len);
    LIQUID_VLA(liquid_float_complex, buf_y, 2*buf_len);

    // create signal generator (wide-band noise)
    msourcecf gen = msourcecf_create_default();

    // in-band signal
    msourcecf_add_noise(gen, 0.0f, 0.5f*r, 0);

    // high-power signal just out of band
    msourcecf_add_noise(gen, (0.5f*r+0.04f), 0.02f, 10);

    // create spectral periodogram objects
    unsigned int nfft = 2400;
    spgramcf px = spgramcf_create_default(nfft);
    spgramcf py = spgramcf_create_default(nfft);

    // generate input signal (filtered noise)
    unsigned int i;
    for (i=0; i<num_blocks; i++) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf_x, buf_len);

        // run resampler in blocks
        unsigned int num_written = 0;
        resamp_crcf_execute_block(q, buf_x, buf_len, buf_y, &num_written);

        // write input and output to respective spectral periodogram estimate
        spgramcf_write(px, buf_x, buf_len);
        spgramcf_write(py, buf_y, num_written);
    }
    printf("num samples in  : %llu\n", spgramcf_get_num_samples_total(px));
    printf("num samples out : %llu\n", spgramcf_get_num_samples_total(py));

    // clean up allocated objects
    resamp_crcf_destroy(q);

    // compute power spectral density output
    LIQUID_VLA(float, X, nfft);
    LIQUID_VLA(float, Y, nfft);
    spgramcf_get_psd(px, X);
    spgramcf_get_psd(py, Y);

    // export results to file for plotting
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n",filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"r    = %f;\n", r);
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"X    = zeros(1,nfft);\n");
    fprintf(fid,"Y    = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"X(%3u) = %12.4e;\n", i+1, X[i]);
        fprintf(fid,"Y(%3u) = %12.4e;\n", i+1, Y[i]);
    }
    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot time-domain result\n");
    fprintf(fid,"fx=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fy=fx*r;\n");
    fprintf(fid,"Y = Y - 10*log10(r);\n");
    fprintf(fid,"figure('Color','white','position',[500 500 800 600]);\n");
    fprintf(fid,"plot(fx,X,'-','LineWidth',2,'Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"     fy,Y,'-','LineWidth',2,'Color',[0.5 0 0],    'MarkerSize',1);\n");
    fprintf(fid,"legend('original','resampled','location','northeast');");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"fmin = min(fx(   1),fy(   1));\n");
    fprintf(fid,"fmax = max(fx(nfft),fy(nfft));\n");
    fprintf(fid,"axis([fmin fmax -100 20]);\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n",filename);
    return 0;
}

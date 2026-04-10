const char __docstr__[] =
"Demonstration of rresamp object whereby an input signal"
" is resampled at a rational rate P/Q = interp/decim.";

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
    liquid_argparse_add(char*, filename, "rresamp_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, interp,  3, 'i', "output rate (interpolation factor)", NULL);
    liquid_argparse_add(unsigned, decim,   5, 'd', "input rate (decimation factor)", NULL);
    liquid_argparse_add(unsigned, m,      20, 'm', "filter semi-length (actual length: 4*m+1)", NULL);
    liquid_argparse_add(float,    bw,     -1, 'w', "filter bandwidth (normalized); negative for default", NULL);
    liquid_argparse_add(float,    As,     60, 'a', "stop-band attenuation [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (interp == 0 || interp > 1000)
        return liquid_error(LIQUID_EICONFIG,"interpolation rate must be in [1,1000]");
    if (decim == 0 || decim > 1000)
        return liquid_error(LIQUID_EICONFIG,"decimation rate must be in [1,1000]");

    // create resampler object
    rresamp_crcf q = rresamp_crcf_create_kaiser(interp,decim,m,bw,As);
    if (q == NULL) return -1;
    rresamp_crcf_print(q);
    float rate = rresamp_crcf_get_rate(q);

    // number of sample blocks (limit by large interp/decim rates)
    unsigned int n = 120e3 / (interp > decim ? interp : decim);

    // input/output buffers
    LIQUID_VLA(liquid_float_complex, buf_x, decim); // input
    LIQUID_VLA(liquid_float_complex, buf_y, interp); // output

    // create signal generator (wide-band noise and tone)
    float noise_bw = 0.7f * (rate > 1.0 ? 1.0 : rate);
    msourcecf gen = msourcecf_create_default();
    msourcecf_add_noise(gen, 0.0f, noise_bw,  0);
    msourcecf_add_tone (gen, 0.45, 0.00f, 20);

    // create spectral periodogram objects
    unsigned int nfft = 2400;
    spgramcf px = spgramcf_create_default(nfft);
    spgramcf py = spgramcf_create_default(nfft);

    // generate input signal (filtered noise)
    unsigned int i;
    for (i=0; i<n; i++) {
        // write decim input samples to buffer
        msourcecf_write_samples(gen, buf_x, decim);

        // run resampler and write interp output samples
        rresamp_crcf_execute(q, buf_x, buf_y);

        // write input and output to respective spectral periodogram estimate
        spgramcf_write(px, buf_x, decim);
        spgramcf_write(py, buf_y, interp);
    }
    printf("num samples out : %llu\n", spgramcf_get_num_samples_total(py));
    printf("num samples in  : %llu\n", spgramcf_get_num_samples_total(px));

    // clean up allocated objects
    rresamp_crcf_destroy(q);

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
    fprintf(fid,"interp = %u;\n", interp);
    fprintf(fid,"decim  = %u;\n", decim);
    fprintf(fid,"r      = interp/decim;\n");
    fprintf(fid,"nfft   = %u;\n", nfft);
    fprintf(fid,"X      = zeros(1,nfft);\n");
    fprintf(fid,"Y      = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"X(%3u) = %12.4e;\n", i+1, X[i]);
        fprintf(fid,"Y(%3u) = %12.4e;\n", i+1, Y[i]);
    }
    fprintf(fid,"\n\n");
    fprintf(fid,"%% plot frequency-domain result\n");
    fprintf(fid,"fx=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fy=fx*r;\n");
    fprintf(fid,"figure('Color','white','position',[500 500 800 600]);\n");
    fprintf(fid,"plot(fx,X,'-','LineWidth',2,'Color',[0.5 0.5 0.5],'MarkerSize',1,...\n");
    fprintf(fid,"     fy,Y,'-','LineWidth',2,'Color',[0.5 0 0],    'MarkerSize',1);\n");
    fprintf(fid,"legend('original','resampled','location','northwest');");
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

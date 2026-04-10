const char __docstr__[] = "Demonstrate arbitrary rate symstreamrcf object.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "symstreamrcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*, ftype_str, "arkaiser", 'f', "filter type", liquid_argparse_firfilt);
    liquid_argparse_add(float,    bw, 0.234567,  'w', "filter bandwidth", NULL);
    liquid_argparse_add(unsigned, m,         9,  'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,    0.3,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    mod_str,"qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, nfft,   2400,  'n', "FFT size", NULL);
    liquid_argparse_add(unsigned, num_samples, 80000,'N', "number of samples to simulate", NULL);
    liquid_argparse_parse(argc,argv);

    // spectral periodogram options
    spgramcf periodogram = spgramcf_create_default(nfft);

    // create stream generator
    int ftype = liquid_getopt_str2firfilt(ftype_str);
    int ms    = liquid_getopt_str2mod(mod_str);
    symstreamrcf gen = symstreamrcf_create_linear(ftype,bw,m,beta,ms);
    symstreamrcf_print(gen);

    // create buffer for storing output
    unsigned int buf_len = 1024;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);

    // generate samples
    unsigned int total_samples = 0;
    while (total_samples < num_samples) {
        // write samples to buffer
        symstreamrcf_write_samples(gen, buf, buf_len);

        // push resulting sample through periodogram
        spgramcf_write(periodogram, buf, buf_len);
        
        // accumulated samples
        total_samples += buf_len;
    }
    printf("total samples: %u\n", total_samples);

    // compute power spectral density output
    LIQUID_VLA(float, psd, nfft);
    spgramcf_get_psd(periodogram, psd);

    // destroy objects
    symstreamrcf_destroy(gen);
    spgramcf_destroy(periodogram);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H    = zeros(1,nfft);\n");
    unsigned int i;
    for (i=0; i<nfft; i++)
        fprintf(fid,"H(%6u) = %12.4e;\n", i+1, psd[i]);

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, H, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -120 20]);\n");
    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}


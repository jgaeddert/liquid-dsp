const char __docstr__[] =
"This example demonstrates generating multiple signal sources simultaneously"
" for testing using the msource (multi-source) family of objects.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

// user-defined callback; generate tones
int callback(void *          _userdata,
             liquid_float_complex * _v,
             unsigned int    _n)
{
    unsigned int * counter = (unsigned int*)_userdata;
    unsigned int i;
    for (i=0; i<_n; i++) {
        _v[i] = *counter==0 ? 1 : 0;
        *counter = (*counter+1) % 8;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "msourcecf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    mod_str,   "qpsk", 'M', "linear modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, m,             12, 'm', "modulation filter semi-length", NULL);
    liquid_argparse_add(float,    beta,       0.30f, 'b', "modulation filter excess bandwidth factor", NULL);
    liquid_argparse_add(float,    bt,         0.35f, 'B', "GMSK filter bandwidth-time factor", NULL);
    liquid_argparse_add(unsigned, nfft,        2400, 'n', "spectral periodogram FFT size", NULL);
    liquid_argparse_add(unsigned, num_samples,48000, 'N', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // msource parameters
    int ms = liquid_getopt_str2mod(mod_str);

    // create spectral periodogram
    spgramcf periodogram = spgramcf_create_default(nfft);

    unsigned int buf_len = 1024;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);

    // create multi-signal source generator
    msourcecf gen = msourcecf_create_default();

    // add signals     (gen,  fc,   bw,    gain, {options})
    msourcecf_add_noise(gen,  0.0f, 1.00f, -40);               // wide-band noise
    msourcecf_add_noise(gen,  0.0f, 0.20f,   0);               // narrow-band noise
    msourcecf_add_tone (gen, -0.4f, 0.00f,  20);               // tone
    msourcecf_add_modem(gen,  0.2f, 0.10f,   0, ms, m, beta);  // modulated data (linear)
    msourcecf_add_gmsk (gen, -0.2f, 0.05f,   0, m, bt);        // modulated data (GMSK)
    unsigned int counter = 0;
    msourcecf_add_user (gen,  0.4f, 0.15f, -10, (void*)&counter, callback); // tones

    // print source generator object
    msourcecf_print(gen);

    unsigned int total_samples = 0;
    while (total_samples < num_samples) {
        // write samples to buffer
        msourcecf_write_samples(gen, buf, buf_len);

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
    msourcecf_destroy(gen);
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
    //fprintf(fid,"axis([-0.5 0.5 -60 40]);\n");
    fprintf(fid,"axis([-0.5 0.5 -80 40]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}


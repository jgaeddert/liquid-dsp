const char __docstr__[] =
"Compare spgram between accumulation methods (infinite vs. real-time integration).";

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
    liquid_argparse_add(char*, filename, "spgramcf_compare_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, nfft,           1024, 'n', "spectral periodogram FFT size", NULL);
    liquid_argparse_add(float,    alpha,          0.05, 'a', "integration bandwidth", NULL);
    liquid_argparse_add(int,      num_samples, 2000000, 'N', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // create spectral periodogram
    spgramcf q0 = spgramcf_create_default(nfft);    // infinite integration
    spgramcf q1 = spgramcf_create_default(nfft);    // 'real-time' integration
    spgramcf_set_alpha(q1, alpha);
    spgramcf_print(q0);

    // create multi-signal source generator
    msourcecf    gen    = msourcecf_create_default();
    int          ms     = LIQUID_MODEM_QPSK;    // linear modulation scheme
    unsigned int m      =    12;                // modulation filter semi-length
    float        beta   = 0.20f;                // modulation filter excess bandwidth factor
    float        bt     = 0.35f;                // GMSK filter bandwidth-time factor
    // add signals     (gen,  fc,   bw,    gain, {options})
    msourcecf_add_noise(gen,  0.0f, 1.00f, -60);                // wide-band noise
    msourcecf_add_noise(gen,  0.0f, 0.20f,   0);                // narrow-band noise
    msourcecf_add_tone (gen, -0.4f, 0.00f,  20);                // tone
    msourcecf_add_modem(gen,  0.2f, 0.10f,   0, ms, m, beta);   // modulated data (linear)
    msourcecf_add_gmsk (gen, -0.2f, 0.05f,   0, m, bt);         // modulated data (GMSK)

    // generate samples
    unsigned int i;
    unsigned int  buf_len = 1024;
    LIQUID_VLA(liquid_float_complex, buf, buf_len);
    while (num_samples > 0)
    {
        // write samples to buffer
        msourcecf_write_samples(gen, buf, buf_len);
        num_samples -= buf_len;

        // push resulting sample through periodogram objects
        spgramcf_write(q0, buf, buf_len);
        spgramcf_write(q1, buf, buf_len);
    }

    // compute power spectral density outputs
    LIQUID_VLA(float, psd_0, nfft);
    LIQUID_VLA(float, psd_1, nfft);
    spgramcf_get_psd(q0, psd_0);
    spgramcf_get_psd(q1, psd_1);

    // destroy objects
    msourcecf_destroy(gen);
    spgramcf_destroy(q0);
    spgramcf_destroy(q1);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"psd_0= zeros(1,nfft);\n");
    fprintf(fid,"psd_1= zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"psd_0(%6u) = %12.4e;\n", i+1, psd_0[i]);
        fprintf(fid,"psd_1(%6u) = %12.4e;\n", i+1, psd_1[i]);
    }
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, psd_0, '-', 'LineWidth',1.5, f, psd_1, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"legend('infinite integration','alpha=%.3f');\n", alpha);
    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}

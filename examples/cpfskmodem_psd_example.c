const char __docstr__[] =
"This example demonstrates the differences in power spectral"
" density (PSD) for different continuous-phase frequency-shift"
" keying (CP-FSK) modems in liquid. Identical bit streams are fed"
" into modulators with different pulse-shaping filters and the"
" resulting PSDs are computed and logged to a file.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "cpfskmodem_psd_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char *,   filter,      "square",'t', "filter type: square, rcos-full, rcos-half, gmsk", NULL);
    liquid_argparse_add(unsigned, bps,         1,       'p', "number of bits/symbol", NULL);
    liquid_argparse_add(float,    h,           0.5f,    'H', "modulation index (h=1/2 for MSK)", NULL);
    liquid_argparse_add(unsigned, k,           4,       'k', "filter samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,           3,       'm', "filter delay (symbols)", NULL);
    liquid_argparse_add(float,    beta,        0.35f,   'b', "GMSK bandwidth-time factor", NULL);
    liquid_argparse_add(unsigned, num_symbols, 48000,   'n', "number of data symbols", NULL);
    liquid_argparse_add(unsigned, nfft,        2400,    'N', "FFT size", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int i;

    // derived values
    unsigned int M = 1 << bps;              // constellation size

    // create modulators
    cpfskmod mod_0 = cpfskmod_create(bps, h, k, m, beta, LIQUID_CPFSK_SQUARE);
    cpfskmod mod_1 = cpfskmod_create(bps, h, k, m, beta, LIQUID_CPFSK_RCOS_FULL);
    cpfskmod mod_2 = cpfskmod_create(bps, h, k, m, beta, LIQUID_CPFSK_RCOS_PARTIAL);
    cpfskmod mod_3 = cpfskmod_create(bps, h, k, m, beta, LIQUID_CPFSK_GMSK);

    // buffers
    LIQUID_VLA(liquid_float_complex, buf_0, k);
    LIQUID_VLA(liquid_float_complex, buf_1, k);
    LIQUID_VLA(liquid_float_complex, buf_2, k);
    LIQUID_VLA(liquid_float_complex, buf_3, k);

    // create PSD estimators
    // spectral periodogram options

    // create spectral periodogram
    spgramcf spgram_0 = spgramcf_create_default(nfft);
    spgramcf spgram_1 = spgramcf_create_default(nfft);
    spgramcf spgram_2 = spgramcf_create_default(nfft);
    spgramcf spgram_3 = spgramcf_create_default(nfft);

    // estimate PSD for each symbol
    for (i=0; i<num_symbols; i++) {

        // generate random symbol
        unsigned int sym = rand() % M;

        // modulate
        cpfskmod_modulate(mod_0, sym, buf_0);
        cpfskmod_modulate(mod_1, sym, buf_1);
        cpfskmod_modulate(mod_2, sym, buf_2);
        cpfskmod_modulate(mod_3, sym, buf_3);
        
        // estimate PSD
        spgramcf_write(spgram_0, buf_0, k);
        spgramcf_write(spgram_1, buf_1, k);
        spgramcf_write(spgram_2, buf_2, k);
        spgramcf_write(spgram_3, buf_3, k);
    }
    
    // compute power spectral density estimate output
    LIQUID_VLA(float, psd_0, nfft); spgramcf_get_psd(spgram_0, psd_0);
    LIQUID_VLA(float, psd_1, nfft); spgramcf_get_psd(spgram_1, psd_1);
    LIQUID_VLA(float, psd_2, nfft); spgramcf_get_psd(spgram_2, psd_2);
    LIQUID_VLA(float, psd_3, nfft); spgramcf_get_psd(spgram_3, psd_3);

    // destroy modulators
    cpfskmod_destroy(mod_0);
    cpfskmod_destroy(mod_1);
    cpfskmod_destroy(mod_2);
    cpfskmod_destroy(mod_3);

    // destroy spgram objects
    spgramcf_destroy(spgram_0);
    spgramcf_destroy(spgram_1);
    spgramcf_destroy(spgram_2);
    spgramcf_destroy(spgram_3);

    // 
    // export results
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"nfft        = %u;\n", nfft);

    // save power spectral density
    fprintf(fid,"psd_0 = zeros(1,nfft);\n");
    fprintf(fid,"psd_1 = zeros(1,nfft);\n");
    fprintf(fid,"psd_2 = zeros(1,nfft);\n");
    fprintf(fid,"psd_3 = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++) {
        fprintf(fid,"psd_0(%4u) = %12.8f;\n", i+1, psd_0[i] - 10*log10(k));
        fprintf(fid,"psd_1(%4u) = %12.8f;\n", i+1, psd_1[i] - 10*log10(k));
        fprintf(fid,"psd_2(%4u) = %12.8f;\n", i+1, psd_2[i] - 10*log10(k));
        fprintf(fid,"psd_3(%4u) = %12.8f;\n", i+1, psd_3[i] - 10*log10(k));
    }

    // plot PSD
    fprintf(fid,"figure;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"  plot(f,psd_0,'LineWidth',1.5,'Color',[0.0 0.5 0.2]);\n");
    fprintf(fid,"  plot(f,psd_1,'LineWidth',1.5,'Color',[0.0 0.2 0.5]);\n");
    fprintf(fid,"  plot(f,psd_2,'LineWidth',1.5,'Color',[0.5 0.0 0.0]);\n");
    fprintf(fid,"  plot(f,psd_3,'LineWidth',1.5,'Color',[0.5 0.5 0.0]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"legend('square','rcos (full)','rcos (partial)','GMSK, BT=%.2f');\n", beta);
    fprintf(fid,"axis([-0.5 0.5 -80 10]);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to '%s'\n", filename);

    return 0;
}

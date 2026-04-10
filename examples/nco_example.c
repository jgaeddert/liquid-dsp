const char __docstr__[] =
"This example demonstrates the most basic functionality of the"
" numerically-controlled oscillator (NCO) object.";

#include <stdio.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename,"nco_example.m",'o', "output filename", NULL);
    liquid_argparse_add(char*,    type_str,      "nco", 't', "nco type, {nco, vco}", NULL);
    liquid_argparse_add(float,    fc,       0.07221980, 'r', "center frequency", NULL);
    liquid_argparse_add(unsigned, num_samples,  240000, 'n', "number of samples", NULL);
    liquid_argparse_add(unsigned, nfft,           4000, 'N', "spectral periodogram FFT size", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (strcmp(type_str,"nco") && strcmp(type_str,"vco"))
        return liquid_error(LIQUID_EICONFIG,"invalid nco type '%s' (must be either 'nco' or 'vco')", type_str);

    // create the NCO object
    nco_crcf q = nco_crcf_create(strcmp(type_str,"nco")==0 ? LIQUID_NCO : LIQUID_VCO);
    nco_crcf_set_frequency(q, 2*M_PI*fc);
    nco_crcf_print(q);

    // create spectral periodogram
    spgramcf periodogram = spgramcf_create_default(nfft);
    spgramcf_print(periodogram);

    unsigned int i;
    for (i=0; i<num_samples; i++) {
        liquid_float_complex y;
        nco_crcf_cexpf(q, &y);
        nco_crcf_step(q);
        
        // push resulting sample through periodogram
        spgramcf_push(periodogram, y);
    }

    // compute power spectral density output
    LIQUID_VLA(float, psd, nfft);
    spgramcf_get_psd(periodogram, psd);

    // destroy objects
    spgramcf_destroy(periodogram);
    nco_crcf_destroy(q);

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"psd  = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        fprintf(fid,"psd(%6u) = %12.4e;\n", i+1, psd[i]);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, psd, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -60 40]);\n");

    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}

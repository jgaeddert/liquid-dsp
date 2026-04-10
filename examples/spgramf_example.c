const char __docstr__[] = "Spectral periodogram example with real inputs.";

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
    liquid_argparse_add(char*, filename, "spgramf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, nfft,           1024, 'm', "spectral periodogram FFT size", NULL);
    liquid_argparse_add(unsigned, num_samples, 2000000, 'n', "number of samples", NULL);
    liquid_argparse_add(float,    noise_floor,     -60, '0', "noise floor [dB]", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);

    // create spectral periodogram
    spgramf q = spgramf_create_default(nfft);
    spgramf_print(q);

    // generate signal (band-pass filter)
    iirfilt_rrrf filter = iirfilt_rrrf_create_prototype(
            LIQUID_IIRDES_BUTTER,
            LIQUID_IIRDES_BANDPASS,
            LIQUID_IIRDES_SOS,
            9, 0.17f, 0.20f, 0.1f, 60.0f);

    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // filter input noise signal
        float y = 0;
        iirfilt_rrrf_execute(filter, randnf(), &y);

        // add noise
        y += nstd * randnf();

        // push resulting sample through periodogram
        spgramf_push(q, y);
    }

    // compute power spectral density output
    LIQUID_VLA(float, psd, nfft);
    spgramf_get_psd(q, psd);

    // destroy objects
    iirfilt_rrrf_destroy(filter);
    spgramf_destroy(q);

    // 
    // export output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"nfft = %u;\n", nfft);
    fprintf(fid,"f    = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H    = zeros(1,nfft);\n");
    fprintf(fid,"noise_floor = %12.6f;\n", noise_floor);
    
    for (i=0; i<nfft; i++)
        fprintf(fid,"H(%6u) = %12.4e;\n", i+1, psd[i]);

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f, H, '-', 'LineWidth',1.5);\n");
    fprintf(fid,"xlabel('Normalized Frequency [f/F_s]');\n");
    fprintf(fid,"ylabel('Power Spectral Density [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"ymin = 10*floor([noise_floor-20]/10);\n");
    fprintf(fid,"ymax = 10*floor([noise_floor+80]/10);\n");
    fprintf(fid,"axis([-0.5 0.5 ymin ymax]);\n");
    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}


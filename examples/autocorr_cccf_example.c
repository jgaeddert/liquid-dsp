const char __docstr__[] =
"This example demonstrates the autocorr (auto-correlation) object"
" functionality.  A random time-domain sequence is generated which"
" exhibits time-domain repetitions (auto-correlation properties),"
" for example:  abcdabcdabcd....abcd.  The sequence is pushed through"
" the autocorr object, and the results are written to an output file."
" The command-line arguments allow the user to experiment with the"
" sequence length, number of sequence repetitions, and properties of"
" the auto-correlator, as well as signal-to-noise ratio.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename,"autocorr_cccf_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, sequence_len, 32,           'm', "short sequence length", NULL);
    liquid_argparse_add(unsigned, num_sequences,8,            'n', "number short sequences (repetition length)", NULL);
    liquid_argparse_add(unsigned, window_size,  64,           'w', "autocorr window size", NULL);
    liquid_argparse_add(unsigned, delay,        sequence_len, 'd', "autocorr delay (multiple of 's')", NULL);
    liquid_argparse_add(bool,     normalize,    0,            'e', "normalize output by E{x^2}?", NULL);
    liquid_argparse_add(float,    SNRdB,        20.0f,        's', "signal-to-noise ratio (dB)", NULL);
    liquid_argparse_parse(argc,argv);

    // derived values
    unsigned int num_samples = sequence_len*(num_sequences+2); // pad end w/ zeros

    // data arrays
    LIQUID_VLA(liquid_float_complex, sequence, sequence_len);   // short sequence
    LIQUID_VLA(liquid_float_complex, x, num_samples);           // autocorr input sequence
    LIQUID_VLA(liquid_float_complex, rxx, num_samples);         // autocorr output

    // generate objects
    autocorr_cccf q = autocorr_cccf_create(window_size,delay);

    unsigned int i;

    // generate random training sequence using QPSK symbols
    modemcf mod = modemcf_create(LIQUID_MODEM_QPSK);
    for (i=0; i<sequence_len; i++)
        modemcf_modulate(mod, rand()%4, &sequence[i]);
    modemcf_destroy(mod);

    // write training sequence 'n' times, followed by zeros
    unsigned int t=0;
    for (i=0; i<num_sequences; i++) {
        // copy sequence
        memmove(&x[t], sequence, sequence_len*sizeof(liquid_float_complex));

        t += sequence_len;
    }

    // pad end with zeros
    for (i=t; i<num_samples; i++)
        x[i] = 0;

    // add noise
    float nstd = powf(10.0f, -SNRdB/20.0f);
    for (i=0; i<num_samples; i++)
        cawgn(&x[i],nstd);
        
    // compute auto-correlation
    for (i=0; i<num_samples; i++) {
        autocorr_cccf_push(q,x[i]);
        autocorr_cccf_execute(q,&rxx[i]);

        // normalize by energy
        if (normalize)
            rxx[i] /= autocorr_cccf_get_energy(q);
    }

    // find peak
    liquid_float_complex rxx_peak = 0;
    for (i=0; i<num_samples; i++) {
        if (i==0 || cabsf(rxx[i]) > cabsf(rxx_peak))
            rxx_peak = rxx[i];
    }
    printf("peak auto-correlation : %12.8f, angle %12.8f\n", cabsf(rxx_peak),
                                                             cargf(rxx_peak));


    // destroy allocated objects
    autocorr_cccf_destroy(q);

    // 
    // write results to file
    //
    FILE* fid = fopen(filename, "w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);

    // write signal to output file
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x  (%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]  ), cimagf(x[i]  ));
        fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rxx[i]), cimagf(rxx[i]));
    }
    fprintf(fid,"t=1:num_samples;\n");
    fprintf(fid,"figure('position',[100 100 800 600]);\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),t,imag(x));\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('received signal');\n");
    fprintf(fid,"  legend('real','imag');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,abs(rxx));\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('auto-correlation magnitude');\n");
    fclose(fid);
    printf("data written to %s\n", filename);

    printf("done.\n");
    return 0;
}

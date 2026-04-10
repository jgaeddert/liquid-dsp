const char __docstr__[] =
"This example demonstrates the functionality of the arbitrary"
" modem, a digital modulator/demodulator object with signal"
" constellation points chosen arbitrarily.  A simple bit-error"
" rate simulation is then run to test the performance of the"
" modem.  The results are written to a file.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "modem_arb_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, bps,  6, 'b', "number of bits per symbol", NULL);
    liquid_argparse_add(unsigned, n, 4800, 'n', "number of data points to evaluate", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (bps == 0)
        return liquid_error(LIQUID_EICONFIG,"bits/symbol must be greater than zero");

    // derived values
    unsigned int i;
    unsigned int M = 1<<bps;    // constellation size

    // initialize constellation table
    LIQUID_VLA(liquid_float_complex, constellation, M);
    // initialize constellation (golden spiral)
    float phi = (3 - sqrtf(5))/2.0f;
    for (i=0; i<M; i++) {
        float r     = sqrtf(2.0f*logf((float)M/(float)(M-i)));
        float theta = 2*M_PI*phi*(float)i;
        constellation[i] = r * cexpf(_Complex_I*theta);
    }
    
    // create mod/demod objects
    modemcf mod   = modemcf_create_arbitrary(constellation, M);
    modemcf demod = modemcf_create_arbitrary(constellation, M);

    modemcf_print(mod);

    // run simulation
    LIQUID_VLA(liquid_float_complex, x, n);
    unsigned int num_errors = 0;

    // run simple BER simulation
    num_errors = 0;
    unsigned int sym_in;
    unsigned int sym_out;
    for (i=0; i<n; i++) {
        // generate and modulate random symbol
        sym_in = modemcf_gen_rand_sym(mod);
        modemcf_modulate(mod, sym_in, &x[i]);

        // add noise
        x[i] += 0.05 * randnf() * cexpf(_Complex_I*M_PI*randf());

        // demodulate
        modemcf_demodulate(demod, x[i], &sym_out);

        // accumulate errors
        num_errors += count_bit_errors(sym_in,sym_out);
    }
    printf("num bit errors: %4u / %4u\n", num_errors, bps*n);

    // destroy modem objects
    modemcf_destroy(mod);
    modemcf_destroy(demod);

    // 
    // export output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"bps = %u;\n", bps);
    fprintf(fid,"M = %u;\n", M);

    for (i=0; i<n; i++) {
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1,
                                                     crealf(x[i]),
                                                     cimagf(x[i]));
    }

    // plot results
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(x,'x','MarkerSize',1);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title(['Arbitrary ' num2str(M) '-QAM']);\n");
    fprintf(fid,"axis([-1 1 -1 1]*2);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);

    printf("results written to '%s'\n", filename);
    printf("done.\n");

    return 0;
}

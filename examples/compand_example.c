const char __docstr__[] =
"This example demonstrates the interface to the compand function"
" (compression, expansion).  The compander is typically used with the"
" quantizer to increase the dynamic range of the converter, particularly for"
" low-level signals.  The transfer function is computed (empirically) and"
" printed to the screen.";

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "compand_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(int,   n,       31,    'n', "number of levels to test", NULL);
    liquid_argparse_add(float, mu,      255.0f,'m', "compression factor", NULL);
    liquid_argparse_add(float, range,   1.25f, 'r', "range of values to test", NULL);
    liquid_argparse_add(bool,  verbose, 0,     'v', "enable verbose output", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (mu < 0)
        return liquid_error(LIQUID_EICONFIG,"mu must be positive");
    if (range <= 0)
        return liquid_error(LIQUID_EICONFIG,"range must be greater than zero");

    // open debug file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    float x = -range;
    float y, z;
    float dx = 2.0*range/(float)(n-1);
    unsigned int i;
    for (i=0; i<n; i++) {
        y = compress_mulaw(x,mu);
        z = expand_mulaw(y,mu);
        if (verbose)
            printf("%8.4f > %8.4f > %8.4f\n", x, y, z);

        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x);
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y);
        fprintf(fid,"z(%3u) = %12.4e;\n", i+1, z);

        x += dx;
    }

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(x,y,'-b','LineWidth',2,x,z,'-r');\n");
    fprintf(fid,"axis square\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('f(x)');\n");

    // close debug file
    fclose(fid);
    printf("results written to %s\n", filename);
    printf("done.\n");
    return 0;
}


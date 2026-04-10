const char __docstr__[] = "Demonstrate interface to compress/expand methods.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "compand_cf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(int,   n,  31,     'n', "number of levels to test", NULL);
    liquid_argparse_add(float, mu, 255.0f, 'm', "compression factor", NULL);
    liquid_argparse_parse(argc,argv);

    // open debug file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    liquid_float_complex x, y, z;
    int i, j;

    for (i=0; i<n+1; i++) {
        for (j=0; j<n+1; j++) {
            x = (float)(2*i - n)/(float)(n) + _Complex_I*(float)(2*j-n)/(float)(n);
            compress_cf_mulaw(x,mu,&y);
            expand_cf_mulaw(y,mu,&z);

            if (i==j) {
                printf("%8.4f + j*%8.4f > ", crealf(x), cimagf(x));
                printf("%8.4f + j*%8.4f > ", crealf(y), cimagf(y));
                printf("%8.4f + j*%8.4f\n",  crealf(z), cimagf(z));
            }

            fprintf(fid,"x(%3d,%3d) = %12.4e + j*%12.4e;\n", i+1, j+1, crealf(x), cimagf(x));
            fprintf(fid,"y(%3d,%3d) = %12.4e + j*%12.4e;\n", i+1, j+1, crealf(y), cimagf(y));
        }
    }

    for (i=0; i<n+1; i++)
        fprintf(fid,"t(%3d) = %12.4e;\n", i+1, (float)(2*i-n)/(float)(n));

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"mesh(t,t,real(y));\n");
    fprintf(fid,"xlabel('x: real');\n");
    fprintf(fid,"ylabel('x: imag');\n");
    fprintf(fid,"box off;\n");
    fprintf(fid,"view(3);\n");
    fprintf(fid,"title('real[y]');\n");
    //fprintf(fid,"axis([-1 1 -1 1 -1 1]);\n");

    // close debug file
    fclose(fid);
    printf("results written to %s\n", filename);
    printf("done.\n");
    return 0;
}


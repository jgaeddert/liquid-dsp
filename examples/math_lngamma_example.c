const char __docstr__[] = "Demonstrates accuracy of lngamma function";

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
    liquid_argparse_add(char*, filename, "math_lngamma_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, n,    256,   'n', "number of steps", NULL);
    liquid_argparse_add(float,    zmin, 1e-3f, 'z', "minimum value", NULL);
    liquid_argparse_add(float,    zmax, 6.00f, 'Z', "maximum value", NULL);
    liquid_argparse_parse(argc,argv);

    unsigned int d = n/32;  // print every d values to screen

    // log scale values
    float xmin = logf(zmin);
    float xmax = logf(zmax);
    float dx = (xmax-xmin)/(n-1);

    FILE * fid = fopen(filename,"w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    unsigned int i;
    float z;
    float g;
    float x = xmin; // log(z)
    for (i=0; i<n; i++) {
        z = expf(x);
        g = liquid_lngammaf(z);

        fprintf(fid,"z(%4u) = %16.8e; g(%4u) = %16.8e;\n", i+1, z, i+1, g);
        if ( (i%d)==0 )
            printf("lngamma(%12.8f) = %12.8f\n",z,g);
        x += dx;
    }
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  semilogx(z,g,z,log(gamma(z)));\n");
    fprintf(fid,"  xlabel('z');\n");
    fprintf(fid,"  ylabel('lngamma(z)');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  loglog(z,abs(log(gamma(z))-g));\n");
    fprintf(fid,"  xlabel('z');\n");
    fprintf(fid,"  ylabel('error');\n");
    fclose(fid);
    printf("results written to %s.\n", filename);

    return 0;
}

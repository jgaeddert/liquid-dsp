const char __docstr__[] = "Compare polyfit and polyfit_lagrange.";

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
    liquid_argparse_add(char*, filename,"polyfit_comparison_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, n, 51, 'n', "number of samples to evaluate", NULL);
    liquid_argparse_parse(argc,argv);

    // define x/y pairs
    float x[3] = {-1.0, 0.0, 1.0};
    float y[3] = { 2.0, 7.0, 4.0};
    float p0[3], p1[3];

    // conventional
    polyf_fit         (x,y,3,p0,3);
    polyf_fit_lagrange(x,y,3,p1);

    // evaluate
    LIQUID_VLA(float, x_eval, n);
    LIQUID_VLA(float, y0, n);
    LIQUID_VLA(float, y1, n);
    unsigned int i;
    for (i=0; i<n; i++) {
        x_eval[i] = 2.2f * ((float)i/(float)(n-1) - 0.5f);
        y0[i] = polyf_val(p0, 3, x_eval[i]);
        y1[i] = polyf_val(p1, 3, x_eval[i]);

        printf("x = %8.6f, y(polyfit) = %8.6f, y(lagrange) = %8.6f\n",
            x_eval[i], y0[i], y1[i]);
    }

    // write results to output file for plotting
    FILE * fid = fopen(filename, "w");
    fprintf(fid,"%% %s : auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"x = [%g,%g,%g];\n", x[0], x[1], x[2]);
    fprintf(fid,"y = [%g,%g,%g];\n", y[0], y[1], y[2]);
    fprintf(fid,"p0= [%g,%g,%g];\n", p0[0], p0[1], p0[2]);
    fprintf(fid,"p1= [%g,%g,%g];\n", p1[0], p1[1], p1[2]);
    fprintf(fid,"n = %u;\n", n);
    fprintf(fid,"y0= zeros(1,n);\n");
    fprintf(fid,"y1= zeros(1,n);\n");
    for (i=0; i<n; i++)
        fprintf(fid,"xeval(%u)=%g;y0(%u)=%g;y1(%u)=%g;\n", i+1,x_eval[i],i+1,y0[i],i+1,y1[i]);
    fprintf(fid,"plot(x,y,'s',xeval,y0,'-x',xeval,y1,'-o');\n");
    fprintf(fid,"legend('data','conventional','lagrange');\n");
    fprintf(fid,"grid on;\n");
    //fprintf(fid,"axis([-1.1 1.1 1.5*min(y) 1.5*max(y)]);\n");
    fclose(fid);
    printf("results written to %s\n", filename);
    return 0;
}


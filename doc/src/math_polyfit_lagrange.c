// 
// math_polyfit_lagrange.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/math_polyfit_lagrange.gnu"

int main() {
    unsigned int n=15;      // number of samples

    // initialize data vectors
    float x[n];
    float y[n];
    unsigned int i;
    for (i=0; i<n; i++) {
        // compute Chebyshev points of the second kind
        x[i] = cosf(M_PI*(float)(i)/(float)(n-1));

        // random samples
        y[i] = 0.2f*randnf();
        y[i] = sinf(17.0f*x[i] + 1.1f);
    }

    // compute Lagrange interpolation weights
    //float p[n];
    //polyf_fit_lagrange(x,y,n,p);
    float w[n];
    polyf_fit_lagrange_barycentric(x,n,w);

    // print coefficients
    // NOTE : for Chebyshev points of the second kind, w[i] = (-1)^i * (i==0 || i==n-1 ? 1 : 2)
    for (i=0; i<n; i++)
        printf("  w[%3u] = %12.4e;\n", i, w[i]);

    // 
    // export figure
    //
    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1.1:1.1]\n");
    fprintf(fid,"set yrange [-1.3:1.3]\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'x'\n");
    fprintf(fid,"set ylabel 'y'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"plot '-' using 1:2 with lines linecolor rgb '%s' linewidth 1 notitle,\\\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' notitle\n",LIQUID_DOC_COLOR_PURPLE);

    // evaluate polynomial
    float xmin = -1.1f;
    float xmax =  1.1f;
    unsigned int num_steps = 20*n;
    float dx = (xmax-xmin)/(num_steps-1);
    float xtest = xmin;
    float ytest;
    for (i=0; i<num_steps; i++) {
        ytest = polyf_val_lagrange_barycentric(x,y,w,xtest,n);
        fprintf(fid,"  %12.4e %12.4e\n", xtest, ytest);
        xtest += dx;
    }
    fprintf(fid,"e\n");

    for (i=0; i<n; i++)
        fprintf(fid,"  %12.4e %12.4e\n", x[i], y[i]);
    fprintf(fid,"e\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


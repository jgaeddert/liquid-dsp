// 
// math_polyfit_lagrange.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_GNU "figures.gen/math_polyfit_lagrange.gnu"
#define OUTPUT_FILENAME_TEX "latex.gen/math_polyfit_lagrange.tex"

int main(int argc, char * argv[]) {
    unsigned int n=13;      // number of samples

    // initialize data vectors
    float x[n];
    float y[n];
    unsigned int i;
    for (i=0; i<n; i++) {
        // compute Chebyshev points of the second kind
        x[i] = cosf(M_PI*(float)(i)/(float)(n-1));

        // random samples
        y[i] = 0.2f*randnf();
        y[i] = sinf((n+3)*x[i] + 1.1f);
    }

    // compute Lagrange interpolation weights
    float w[n];
    polyf_fit_lagrange_barycentric(x,n,w);


    // 
    // export files
    //
    FILE * fid;

    // 
    // plot
    //
    fid = fopen(OUTPUT_FILENAME_GNU, "w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], OUTPUT_FILENAME_GNU);
        exit(1);
    }
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_GNU);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1.2:1.2]\n");
    fprintf(fid,"set yrange [-1.2:1.2]\n");
    fprintf(fid,"set size ratio 1.0\n");
    fprintf(fid,"set xlabel 'x'\n");
    fprintf(fid,"set ylabel 'y'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set xtics -5,1,5\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 1.0\n");
    fprintf(fid,"set grid linetype 1 linewidth 0.5 linecolor rgb '%s'\n", LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"plot '-' using 1:2 with lines linecolor rgb '%s' linewidth 4 notitle,\\\n",LIQUID_DOC_COLOR_PURPLE);
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
    printf("results written to %s\n", OUTPUT_FILENAME_GNU);


    // 
    // latex (text)
    //
    fid = fopen(OUTPUT_FILENAME_TEX, "w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], OUTPUT_FILENAME_TEX);
        exit(1);
    }
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME_TEX);

    for (i=0; i<n; i++) {
        fprintf(fid,"$x_{%u} =$ & {\\tt %8.4f}, & ", i, x[i]);
        fprintf(fid,"$w_{%u} =$ & {\\tt %8.1f}, & ", i, w[i]);
        fprintf(fid,"$y_{%u} =$ & {\\tt %8.4f}\\\\", i, y[i]);
        fprintf(fid,"\n");
    }

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_TEX);

    printf("done.\n");
    return 0;
}


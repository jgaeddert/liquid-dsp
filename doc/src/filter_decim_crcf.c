//
// filter_decim_crcf.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_decim_crcf.gnu"

int main() {
    // options
    unsigned int D=4;                   // samples/symbol
    unsigned int m=3;                   // filter delay
    float beta = 0.3f;                  // filter excess bandwidth
    unsigned int num_data_symbols=32;   // number of data symbols

    // derived values
    unsigned int h_len = 2*D*m+1;
    unsigned int num_symbols = num_data_symbols + 2*m;
    unsigned int num_samples = D*num_symbols;

    // design filter and create interpolator
    float h[h_len];
    liquid_firdes_rcos(D,m,beta,0.0f,h);
    decim_crcf q = decim_crcf_create(D,h,h_len);

    // generate input signal and interpolate
    float complex x[num_samples];
    float complex y[num_symbols];
    unsigned int i;
    for (i=0; i<num_samples; i++)
        x[i] = 1.0f * hann(i,num_samples) * cexpf(_Complex_I * 2 * M_PI * i * 0.057f);

    for (i=0; i<num_symbols; i++)
        decim_crcf_execute(q, &x[D*i], &y[i], 0);

    decim_crcf_destroy(q);
    
    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");

    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-1.5:1.5]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'Real'\n");
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 7 pointsize 0.3 linecolor rgb '%s' linewidth 1 title 'input',\\\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'decimated'\n",LIQUID_DOC_COLOR_GREEN);

    for (i=0; i<num_samples; i++)
        fprintf(fid,"%4u %12.4e\n", i, crealf(x[i]));
    fprintf(fid,"e\n");

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"%12.4e %12.4e\n", (float)(i*D) - (float)(D*m), crealf(y[i])/D);
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'Imag'\n");
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 7 pointsize 0.3 linecolor rgb '%s' linewidth 1 title 'input',\\\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'decimated'\n", LIQUID_DOC_COLOR_RED);

    for (i=0; i<num_samples; i++)
        fprintf(fid,"%4u %12.4e\n", i, cimagf(x[i]));
    fprintf(fid,"e\n");

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"%12.4e %12.4e\n", (float)(i*D) - (float)(D*m), cimagf(y[i])/D);
    fprintf(fid,"e\n");

    // close output file
    fclose(fid);

    return 0;
}

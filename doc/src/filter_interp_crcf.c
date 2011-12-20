//
// filter_interp_crcf.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/filter_interp_crcf.gnu"

int main() {
    // options
    unsigned int M=4;                   // samples/symbol
    unsigned int m=3;                   // filter delay
    float As = 40.0f;                   // filter stop-band attenuation
    unsigned int num_data_symbols=32;   // number of data symbols

    // derived values
    unsigned int num_symbols = num_data_symbols + 2*m;
    unsigned int num_samples = M*num_symbols;

    // create interpolator
    interp_crcf q = interp_crcf_create_prototype(M,m,As);

    // generate input signal and interpolate
    float complex x[num_symbols];
    float complex y[num_samples];
    unsigned int i;
    for (i=0; i<num_symbols; i++) {
        float complex sym = (rand() % 2 ? 1.0f : -1.0f) +
                            (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;

        x[i] = (i < num_data_symbols) ? sym : 0.0f;
    }

    for (i=0; i<num_symbols; i++)
        interp_crcf_execute(q, x[i], &y[i*M]);

    // destroy interpolator object
    interp_crcf_destroy(q);
    
    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");

    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-2:2]\n");
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
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 7 pointsize 0.3 linecolor rgb '%s' linewidth 1 title 'interp',\\\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'symbols'\n",LIQUID_DOC_COLOR_GREEN);
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%12.4e %12.4e\n", (float)i / M - (float)m, crealf(y[i]));
    fprintf(fid,"e\n");

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"%4u %12.4e\n", i, crealf(x[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'Imag'\n");
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 7 pointsize 0.3 linecolor rgb '%s' linewidth 1 title 'interp',\\\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '%s' title 'symbols'\n", LIQUID_DOC_COLOR_RED);
    for (i=0; i<num_samples; i++)
        fprintf(fid,"%12.4e %12.4e\n", (float)i / M - (float)m, cimagf(y[i]));
    fprintf(fid,"e\n");
    for (i=0; i<num_symbols; i++)
        fprintf(fid,"%4u %12.4e\n", i, cimagf(x[i]));
    fprintf(fid,"e\n");

    // close output file
    fclose(fid);

    return 0;
}

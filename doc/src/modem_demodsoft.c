//
// modem_demodsoft.c
//
// Generates a gnuplot data file for exporting a modem figure.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.h"
#include "liquid.doc.h"

int main(int argc, char*argv[])
{
    // create mod/demod objects
    modulation_scheme ms = LIQUID_MODEM_QAM;    // modulation scheme
    unsigned int bps=4;                         // bits/symbol
    float complex r = 0.4 + 0.25*_Complex_I;   // received sample
    float gnuplot_version = 4.2;                // gnuplot version
    char input_filename[] = "figures.gen/modem_16qam.dat";

    unsigned int i;
    unsigned int k;

    // 
    unsigned int M = 1<<bps;    // constellation size
    float complex c[M];         // constellation map
    char filename[256];         // filename placeholder

    // generate symbol table
    modem q = modem_create(ms, bps);
    for (i=0; i<M; i++)
        modem_modulate(q, i, &c[i]);
    modem_destroy(q);

    float dmin_0[bps];
    float dmin_1[bps];
    unsigned int smin_0[bps];
    unsigned int smin_1[bps];
    for (k=0; k<bps; k++) {
        dmin_0[k] = 1e9f;
        dmin_1[k] = 1e9f;
        smin_0[k] = 0;
        smin_1[k] = 0;
    }

    // generate plot for each bit
    for (i=0; i<M; i++) {
        // compute distance
        float d = crealf( (r-c[i])*conjf(r-c[i]) );

        for (k=0; k<bps; k++) {
            unsigned int bit = (i >> (bps-k-1)) & 0x01;
            if ( bit ) {
                if (d < dmin_1[k]) {
                    dmin_1[k] = d;
                    smin_1[k] = i;
                }
            } else {
                if (d < dmin_0[k]) {
                    dmin_0[k] = d;
                    smin_0[k] = i;
                }
            }
        }
    }
    for (k=0; k<bps; k++) {
        printf("bit %1u : {%3u, %12.8f} {%3u, %12.8f}\n",
                k,
                smin_0[k], dmin_0[k],
                smin_1[k], dmin_1[k]);
    }

    // 
    // export output
    //

    int plot_labels = (gnuplot_version > 4.1) && (bps < 8);
    int plot_long_labels = 1;

    // generate plot for each bit
    for (k=0; k<bps; k++) {
        // generate gnuplot file
        sprintf(filename,"figures.gen/modem_demodsoft_b%u.gnu", k);
        FILE * fid = fopen(filename,"w");
        if (!fid) {
            fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename);
            exit(1);
        }
        fprintf(fid,"# %s : auto-generated file\n", filename);
        fprintf(fid,"# invoked as :");
        for (i=0; i<argc; i++)
            fprintf(fid," %s",argv[i]);
        fprintf(fid,"\n");
        fprintf(fid,"reset\n");
        // TODO : switch terminal types here
        fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
        // TODO : set range according to scheme
        fprintf(fid,"set xrange [-1.2:1.2]\n");
        fprintf(fid,"set yrange [-1.2:1.2]\n");
        fprintf(fid,"set size square\n");
        //fprintf(fid,"set title \"%s\"\n", figure_title);
        fprintf(fid,"set xlabel \"I\"\n");
        fprintf(fid,"set ylabel \"Q\"\n");
        fprintf(fid,"set nokey # disable legned\n");
        fprintf(fid,"set grid xtics ytics\n");
        fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
        fprintf(fid,"set pointsize 1.0\n");
        if (!plot_labels) {
            // do not print labels
            fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080',\\\n",input_filename);
        } else {
            // print labels
            fprintf(fid,"xoffset = %8.6f\n", plot_long_labels ? 0.0 : 0.06f);
            fprintf(fid,"yoffset = %8.6f\n", 0.06f);
            unsigned int label_line = plot_long_labels ? 3 : 4;
            fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080',\\\n",input_filename);
            fprintf(fid,"     '%s' using ($1+xoffset):($2+yoffset):%u with labels font 'arial,10',\\\n", input_filename,label_line);
        }
        // add received point and connecting lines
        fprintf(fid,"    '-' using 1:2 with lines linewidth 2 linetype 1 linecolor rgb '#000000',\\\n");
        fprintf(fid,"    '-' using 1:2 with lines linewidth 2 linetype 1 linecolor rgb '#000000',\\\n");
        fprintf(fid,"    '-' using 1:2 with points pointtype 2 linewidth 4 linecolor rgb '#800000'\n");

        fprintf(fid,"# received point to '0' bit\n");
        fprintf(fid,"  %12.8f %12.8f\n", crealf(r), cimagf(r));
        fprintf(fid,"  %12.8f %12.8f\n", crealf(c[smin_0[k]]), cimagf(c[smin_0[k]]));
        fprintf(fid,"e\n");

        fprintf(fid,"# received point to '1' bit\n");
        fprintf(fid,"  %12.8f %12.8f\n", crealf(r), cimagf(r));
        fprintf(fid,"  %12.8f %12.8f\n", crealf(c[smin_1[k]]), cimagf(c[smin_1[k]]));
        fprintf(fid,"e\n");

        fprintf(fid,"# received point\n");
        fprintf(fid,"  %12.8f %12.8f\n", crealf(r), cimagf(r));
        fprintf(fid,"e\n");

        fclose(fid);
        printf("results written to '%s'\n", filename);
    }

    return 0;
}

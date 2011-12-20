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
    //float complex r = -0.22 + 0.05*_Complex_I;  // received sample
    float complex r = -0.65 - 0.47*_Complex_I;  // received sample
    float gnuplot_version = 4.2;                // gnuplot version
    float sig = 0.20f;                          // noise standard deviation
    char label_filename[] = "figures.gen/modem_demodsoft_labels.dat";

    unsigned int i;
    unsigned int k;

    // 
    unsigned int M = 1<<bps;    // constellation size
    float complex c[M];         // constellation map

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
        float LLR_hat = (dmin_0[k] - dmin_1[k]) / (2.0f * sig * sig);
        printf("bit %1u : {%3u, %12.8f} {%3u, %12.8f} LLR : %12.8f\n",
                k,
                smin_0[k], dmin_0[k],
                smin_1[k], dmin_1[k],
                LLR_hat);
    }

    // 
    // export output
    //

    int plot_labels = (gnuplot_version > 4.1) && (bps < 8);

    // 
    // generate labels file
    //
    if (plot_labels) {
        // generate output labels file
        FILE * fid = fopen(label_filename,"w");
        if (fid == NULL) {
            fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0], label_filename);
            exit(1);
        }
        fprintf(fid,"# %s : auto-generated file (do not edit)\n", label_filename);
        fprintf(fid,"# invoked as :");
        for (i=0; i<argc; i++)
            fprintf(fid," %s",argv[i]);
        fprintf(fid,"\n");

        char symbol_str[bps+1];
        unsigned int j;
        for (i=0; i<M; i++) {
            // generate binary symbol string
            for (j=0; j<bps; j++)
                symbol_str[j] = ((i>>(bps-j-1)) & 1) ? '1' : '0';
            symbol_str[j] = '\0';   // terminate with null character

            fprintf(fid,"%12.4e %12.4e \"%s\"  ", crealf(c[i]), cimagf(c[i]), symbol_str);

            // now print all bits highlighted...
            for (k=0; k<bps; k++) {
                for (j=0; j<bps; j++) {
                    if (j==k) symbol_str[j] = ((i>>(bps-j-1)) & 1) ? '1' : '0';
                    else      symbol_str[j] = ' ';
                }
                symbol_str[j] = '\0';   // terminate with null character

                fprintf(fid," \"%s\"", symbol_str);
            }
            fprintf(fid,"\n");
        }

        // close output file
        fclose(fid);

        printf("results written to '%s'\n", label_filename);

    }

    // 
    // generate plot for each bit
    //
    char filename[256];         // filename placeholder
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
        fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
        // TODO : set range according to scheme
        fprintf(fid,"set xrange [-1.2:1.2]\n");
        fprintf(fid,"set yrange [-1.2:1.2]\n");
        fprintf(fid,"set size square\n");
        //fprintf(fid,"set title \"%s\"\n", figure_title);
        fprintf(fid,"set xlabel \"In Phase\"\n");
        fprintf(fid,"set ylabel \"Quadrature\"\n");
        fprintf(fid,"set nokey # disable legned\n");
        fprintf(fid,"set xtics -1.5811,1.5811,1.5811 # major xtics: -5/sqrt(10):5/sqrt(10)\n");
        fprintf(fid,"set ytics -1.5811,1.5811,1.5811 # major ytics: -5/sqrt(10):5/sqrt(10)\n");
        fprintf(fid,"set mxtics 5\n");
        fprintf(fid,"set mytics 5\n");
        fprintf(fid,"set grid xtics ytics mxtics mytics\n");
        fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
        fprintf(fid,"set pointsize 1.0\n");
        if (!plot_labels) {
            // do not print labels
            fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080',\\\n",label_filename);
        } else {
            // print labels
            fprintf(fid,"xoffset = 0.0\n");
            fprintf(fid,"yoffset = 0.06\n");
            fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080',\\\n",label_filename);
            unsigned int j;
            for (j=0; j<bps; j++) {
                if (j==k) fprintf(fid,"     '%s' using ($1+xoffset):($2+yoffset):%u with labels font 'Courier Bold,10',\\\n", label_filename, 4+j);
                else      fprintf(fid,"     '%s' using ($1+xoffset):($2+yoffset):%u with labels font 'Courier,10',\\\n",      label_filename, 4+j);
            }
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

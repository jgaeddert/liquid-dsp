//
// quantization_compander.c
//
// Analog-to-digital conversion (ADC).
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("quantization_compander [options]\n");
    printf("  u/h   : print usage\n");
    printf("  m     : mu...\n");
    printf("  o     : output filename\n");
}

int main(int argc, char*argv[]) {
    // options
    float mu = 255.0f;          // mu-law compression factor
    unsigned int n=500;         // input sample size
    char filename[256] = "";    // output filename

    int dopt;
    while ((dopt = getopt(argc,argv,"uhm:o:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'm': mu = atof(optarg);            break;
        case 'o': strncpy(filename,optarg,255); break;
        default:
            exit(1);
        }
    }

    // append null character to end of filename
    filename[255] = '\0';

    // allocate memory for arrays
    float x[n];
    float y[n];
    
    unsigned int i;
    for (i=0; i<n; i++) {
        // generate input signal
        x[i] = (2.0f * (float)(i) / (float)(n)) - 1.0f;

        // compress sample
        y[i] = compress_mulaw(x[i],mu);
    }

    // 
    // export output file
    //
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename);
        exit(1);
    }
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1:1];\n");
    fprintf(fid,"set yrange [-1:1];\n");
    fprintf(fid,"set size ratio 1.0\n");
    fprintf(fid,"set xlabel 'x'\n");
    fprintf(fid,"set ylabel 'f(x)'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' notitle,\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 3 linecolor rgb '%s' notitle\n", LIQUID_DOC_COLOR_PURPLE);
    
    // 
    fprintf(fid,"   -1.0    -1.0\n");
    fprintf(fid,"    1.0     1.0\n");
    fprintf(fid,"e\n");

    for (i=0; i<n; i++)
        fprintf(fid,"   %16.8e  %16.8e;\n", x[i], y[i]);
    fprintf(fid,"e\n");

    // close output file
    fclose(fid);

    return 0;
}


//
// quantization_adc.c
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
    printf("quantize_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  b     : number of bits, 0 < b <= 16 [default: 4]\n");
    printf("  o     : output filename\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int num_bits=4;
    char filename[256] = "";

    int dopt;
    while ((dopt = getopt(argc,argv,"uhb:o:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'b': num_bits = atoi(optarg);      break;
        case 'o': strncpy(filename,optarg,255); break;
        default:
            exit(1);
        }
    }

    // append null character to end of filename
    filename[255] = '\0';

    // derived values
    unsigned int n = 1<<(num_bits+4);

    // allocate memory for arrays
    float x[n];
    float y[n];
    
    unsigned int i;
    for (i=0; i<n; i++) {
        // generate input signal
        x[i] = (2.0f * (float)(i) / (float)(n)) - 1.0f;

        // quantize: analog to digital converter
        unsigned int q = quantize_adc(x[i],num_bits);

        // quantize: digital to analog converter
        y[i] = quantize_dac(q,num_bits);
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
    fprintf(fid,"set xlabel 'DAC input'\n");
    fprintf(fid,"set ylabel 'DAC output'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 1 linecolor rgb '%s' notitle,\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with steps linetype 1 linewidth 3 linecolor rgb '%s' notitle\n", LIQUID_DOC_COLOR_RED);
    
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


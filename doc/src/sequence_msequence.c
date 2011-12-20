//
// sequence_msequence.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME "figures.gen/sequence_msequence.gnu"

// print usage/help message
void usage()
{
    printf("sequence_msequence [options]\n");
    printf("  u/h   : print usage\n");
    printf("  m     : msequence shift register length: m in [2,12]\n");
    printf("  f     : output filename\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int m=5;       // shift register length, n=2^m - 1
    char filename[64] = ""; // output filename

    int dopt;
    while ((dopt = getopt(argc,argv,"uhm:f:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();            return 0;
        case 'm':   m = atoi(optarg);   break;
        case 'f':
            // copy output filename string
            strncpy(filename,optarg,64);
            filename[63] = '\0';
            break;
        default:
            exit(1);
        }
    }

    // ensure output filename is set
    if (strcmp(filename,"")==0) {
        fprintf(stderr,"error: %s, filename not set\n", argv[0]);
        exit(1);
    }

    // validate m
    if (m < 2) {
        fprintf(stderr,"error: %s, m is out of range\n", argv[0]);
        exit(1);
    }

    // create and initialize m-sequence
    msequence ms = msequence_create_default(m);
    msequence_print(ms);
    unsigned int n = msequence_get_length(ms);
    unsigned int sequence[n];   // sequence
    signed int rxx[n];          // auto-correlation
    
    // initialize sequence
    unsigned int i;
    for (i=0; i<n; i++)
        sequence[i] = msequence_advance(ms);

    // reset sequence
    msequence_reset(ms);

    // create and initialize first binary sequence on m-sequence
    bsequence bs1 = bsequence_create(n);
    bsequence_init_msequence(bs1, ms);

    // create and initialize second binary sequence on same m-sequence
    bsequence bs2 = bsequence_create(n);
    bsequence_init_msequence(bs2, ms);

    // when sequences are aligned, autocorrelation is equal to length
    unsigned int k=0;
    rxx[k++] = 2*bsequence_correlate(bs1, bs2) - n;

    // when sequences are misaligned, autocorrelation is equal to -1
    for (i=0; i<n-1; i++) {
        bsequence_push(bs2, msequence_advance(ms));
        rxx[k++] = 2*bsequence_correlate(bs1, bs2)-n;
    }

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
    msequence_destroy(ms);

    // 
    // generate auto-correlation plot
    //

    // open output file
    FILE * fid = fopen(filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0], filename);
        exit(1);
    }
    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1:%u];\n", n+1);
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'delay (number of samples)'\n");
    fprintf(fid,"set nokey # disable legned\n");
    //fprintf(fid,"set grid xtics ytics\n");
    //fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# sequence\n");
    fprintf(fid,"set ylabel 'sequence'\n");
    fprintf(fid,"set yrange [-0.1:1.1]\n");
    fprintf(fid,"plot '-' using 1:2 with steps linetype 1 linewidth 4 linecolor rgb '%s'\n",LIQUID_DOC_COLOR_BLUE);
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %6u\n", i, sequence[i]);
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# auto-correlation\n");
    fprintf(fid,"set ylabel 'auto-correlation'\n");
    fprintf(fid,"set yrange [%f:1.1]\n", -5.0f / (float)n);
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s'\n",LIQUID_DOC_COLOR_GREEN);
    for (i=0; i<n; i++) {
        fprintf(fid,"  %6u %12.4e\n", i, (float)rxx[i] / (float)n);
    }
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");

    // close output file
    fclose(fid);
    printf("results written to %s.\n", filename);

    return 0;
}


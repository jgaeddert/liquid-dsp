//
// nco_pll_example.c
//
// This example demonstrates how the use the nco/pll object
// (numerically-controlled oscillator with phase-locked loop) interface for
// tracking to a complex sinusoid.  The loop bandwidth, phase offset, and
// other parameter can be specified via the command-line interface.
//

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_SINCOS "figures.gen/nco_pll_sincos.gnu"
#define OUTPUT_FILENAME_ERROR  "figures.gen/nco_pll_error.gnu"

// print usage/help message
void usage()
{
    printf("nco_pll [options]\n");
    printf("  u/h   : print usage\n");
    printf("  b     : pll bandwidth, default: 0.01\n");
    printf("  n     : number of samples, default: 512\n");
    printf("  p     : phase offset (radians), default: 0\n");
    printf("  f     : frequency offset (radians), default: 0.3\n");
}

int main(int argc, char*argv[]) {
    // parameters
    float phase_offset = 0;
    float frequency_offset = 0.3f;
    float pll_bandwidth = 0.01f;
    unsigned int n=512;     // number of iterations

    int dopt;
    while ((dopt = getopt(argc,argv,"uhb:n:p:f:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':   usage();    return 0;
        case 'b':   pll_bandwidth = atof(optarg);   break;
        case 'n':   n = atoi(optarg);               break;
        case 'p':   phase_offset = atof(optarg);    break;
        case 'f':   frequency_offset= atof(optarg); break;
        default:
            exit(1);
        }
    }

    // objects
    nco_crcf nco_tx = nco_crcf_create(LIQUID_VCO);
    nco_crcf nco_rx = nco_crcf_create(LIQUID_VCO);

    // initialize objects
    nco_crcf_set_phase(nco_tx, phase_offset);
    nco_crcf_set_frequency(nco_tx, frequency_offset);
    nco_crcf_pll_set_bandwidth(nco_rx, pll_bandwidth);

    // generate input
    float complex x[n];
    float complex y[n];
    float phase_error[n];

    unsigned int i;
    for (i=0; i<n; i++) {
        // generate complex sinusoid
        nco_crcf_cexpf(nco_tx, &x[i]);

        // update nco
        nco_crcf_step(nco_tx);
    }

    // run loop
    for (i=0; i<n; i++) {
        // generate complex sinusoid
        nco_crcf_cexpf(nco_rx, &y[i]);

        // compute phase error
        phase_error[i] = cargf(x[i]*conjf(y[i]));

        // update pll
        nco_crcf_pll_step(nco_rx, phase_error[i]);

        // update rx nco object
        nco_crcf_step(nco_rx);
    }
    nco_crcf_destroy(nco_tx);
    nco_crcf_destroy(nco_rx);

    // 
    // write result to file
    //
    FILE * fid = NULL;

    // open/initialize output file
    fid = fopen(OUTPUT_FILENAME_SINCOS,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_SINCOS);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-1.2:1.2]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'real'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'input',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'nco'\n",      LIQUID_DOC_COLOR_BLUE);
    // export output
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, crealf(x[i]));
    fprintf(fid,"e\n");
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, crealf(y[i]));
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'imag'\n");
    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'input',\\\n", LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"     '-' using 1:2 with lines linetype 1 linewidth 2 linecolor rgb '%s' title 'nco'\n",      LIQUID_DOC_COLOR_GREEN);
    // export output
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, cimagf(x[i]));
    fprintf(fid,"e\n");
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, cimagf(y[i]));
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");

    // close output file
    fclose(fid);


    fid = fopen(OUTPUT_FILENAME_ERROR,"w");

    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_ERROR);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:%u];\n",n);
    fprintf(fid,"set yrange [-3.2:3.2]\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set ylabel 'phase error [radians]'\n");
    fprintf(fid,"set key top right nobox\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n",LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"plot '-' using 1:2 with lines linetype 1 linewidth 4 linecolor rgb '%s' title ''\n", LIQUID_DOC_COLOR_RED);
    for (i=0; i<n; i++)
        fprintf(fid,"%6u %12.4e\n", i, phase_error[i]);
    fprintf(fid,"e\n");

    fclose(fid);

    printf("done.\n");
    return 0;
}

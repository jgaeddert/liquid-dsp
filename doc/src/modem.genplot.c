// modem.genplot.c
//
// Generates a gnuplot file for exporting a modem figure.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("modem.genplot [options]\n");
    printf("  u/h   : print usage\n");
    printf("  g     : specify gnuplot version\n");
    printf("  t     : specify gnuplot terminal (e.g. pdf, jpg, eps)\n");
    printf("  d     : input data filename\n");
    printf("  f     : output gnuplot filename\n");
    printf("  m     : modulation scheme (psk default)\n");
    liquid_print_modulation_schemes();
}

int main(int argc, char*argv[]) {
    // create mod/demod objects
    float gnuplot_version=0.0;
    enum {GNUPLOT_TERM_PNG=0,
          GNUPLOT_TERM_JPG,
          GNUPLOT_TERM_EPS
    } gnuplot_term = 0;
    char input_filename[256] = "datafile.dat";
    char output_filename[256] = "modem.gnu";
    //char figure_title[256] = "constellation";
    modulation_scheme ms = LIQUID_MODEM_QPSK;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhg:t:d:f:m:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'g':   gnuplot_version = atof(optarg);         break;
        case 't':   gnuplot_term = atoi(optarg);            break;
        case 'd':   strncpy(input_filename,optarg,256);     break;
        case 'f':   strncpy(output_filename,optarg,256);    break;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported modulation scheme \"%s\"\n", argv[0], optarg);
                return 1;
            }
            break;
        default:
            exit(1);
        }
    }

    // TODO : validate input
    if (ms == LIQUID_MODEM_UNKNOWN || ms >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"error: %s, invalid modulation scheme \n", argv[0]);
        return 1;
    }

    // derived options/values
    unsigned int bps = modulation_types[ms].bps;
    int plot_labels = (gnuplot_version > 4.1) && (bps < 8);
    int plot_long_labels = 0;

    unsigned int i;

    // generate modem, compute constellation, derive plot style accordingly
    unsigned int M = 1<<bps;
    float complex constellation[M];
    modem q = modem_create(ms);
    for (i=0; i<M; i++)
        modem_modulate(q, i, &constellation[i]);
    modem_destroy(q);

    float range = 1.5f;
    enum {AXES_POLAR, AXES_CART} axes = AXES_POLAR;

    switch (ms) {
    // Phase-shift keying (PSK)
    case LIQUID_MODEM_PSK2:     
    case LIQUID_MODEM_PSK4:     
    case LIQUID_MODEM_PSK8:     
    case LIQUID_MODEM_PSK16:    
    case LIQUID_MODEM_PSK32:    
    case LIQUID_MODEM_PSK64:    
    case LIQUID_MODEM_PSK128:   
    case LIQUID_MODEM_PSK256:   

    // Differential phase-shift keying (DPSK)
    case LIQUID_MODEM_DPSK2:    
    case LIQUID_MODEM_DPSK4:    
    case LIQUID_MODEM_DPSK8:    
    case LIQUID_MODEM_DPSK16:   
    case LIQUID_MODEM_DPSK32:   
    case LIQUID_MODEM_DPSK64:   
    case LIQUID_MODEM_DPSK128:  
    case LIQUID_MODEM_DPSK256:  
        range = 1.2f;
        axes = AXES_POLAR;
        break;

    // amplitude-shift keying (ASK)
    case LIQUID_MODEM_ASK2:     range = 1.2f;   axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK4:     range = 1.5f;   axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK8:     range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK16:    range = 2.00f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK32:    range = 1.5f;   axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK64:    range = 1.5f;   axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK128:   range = 1.5f;   axes = AXES_CART;   break;
    case LIQUID_MODEM_ASK256:   range = 1.5f;   axes = AXES_CART;   break;

    // rectangular quadrature amplitude-shift keying (QAM)
    case LIQUID_MODEM_QAM4:     range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_QAM8:     range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_QAM16:    range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_QAM32:    range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_QAM64:    range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_QAM128:   range = 1.75f;  axes = AXES_CART;   break;
    case LIQUID_MODEM_QAM256:   range = 1.75f;  axes = AXES_CART;   break;

    // amplitude phase-shift keying (APSK)
    case LIQUID_MODEM_APSK4:    range = 1.5f; axes = AXES_POLAR;    break;
    case LIQUID_MODEM_APSK8:    range = 1.5f; axes = AXES_POLAR;    break;
    case LIQUID_MODEM_APSK16:   range = 1.5f; axes = AXES_POLAR;    break;
    case LIQUID_MODEM_APSK32:   range = 1.5f; axes = AXES_POLAR;    break;
    case LIQUID_MODEM_APSK64:   range = 1.5f; axes = AXES_POLAR;    break;
    case LIQUID_MODEM_APSK128:  range = 1.5f; axes = AXES_POLAR;    break;
    case LIQUID_MODEM_APSK256:  range = 1.5f; axes = AXES_POLAR;    break;

    // specific modems
    case LIQUID_MODEM_BPSK:      range = 1.5f; axes = AXES_POLAR;   break;
    case LIQUID_MODEM_QPSK:      range = 1.5f; axes = AXES_POLAR;   break;
    case LIQUID_MODEM_OOK:       range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_SQAM32:    range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_SQAM128:   range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_V29:       range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_ARB16OPT:  range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_ARB32OPT:  range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_ARB64OPT:  range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_ARB128OPT: range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_ARB256OPT: range = 1.5f; axes = AXES_CART;    break;
    case LIQUID_MODEM_ARB64VT:   range = 2.0f; axes = AXES_CART;    break;
    default:
        fprintf(stderr,"error: %s, invalid modulation scheme\n", argv[0]);
        exit(1);
    }
    

    // determine minimum distance between any two points
    float dmin = 0.0f;
    for (i=0; i<M; i++) {
        unsigned int j;
        for (j=i+1; j<M; j++) {
            float d = cabsf(constellation[i] - constellation[j]);

            if ( (d < dmin) || (i==0 && j==1) )
                dmin = d;
        }
    }
    plot_long_labels = dmin < 0.34 ? 0 : 1;

    // write output file
    FILE * fid = fopen(output_filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0],output_filename);
        exit(1);
    }

    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", output_filename);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    // TODO : set range according to scheme
    fprintf(fid,"set xrange [-%4.2f:%4.2f]\n",range,range);
    fprintf(fid,"set yrange [-%4.2f:%4.2f]\n",range,range);
    fprintf(fid,"set size square\n");
    //fprintf(fid,"set title \"%s\"\n", figure_title);
    fprintf(fid,"set xlabel \"I\"\n");
    fprintf(fid,"set ylabel \"Q\"\n");
    fprintf(fid,"set nokey # disable legned\n");
    // TODO : set grid type (e.g. polar) according to scheme
    //
    if (axes == AXES_POLAR)
        fprintf(fid,"set grid polar\n");
    else
        fprintf(fid,"set grid xtics ytics\n");
    
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set pointsize 1.0\n");
    if (!plot_labels) {
        // do not print labels
        fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080'\n",input_filename);
    } else {
        // print labels
        fprintf(fid,"xoffset = %8.6f\n", plot_long_labels ? 0.0   : 0.045f);
        fprintf(fid,"yoffset = %8.6f\n", plot_long_labels ? 0.06f : 0.045f);
        unsigned int label_line = plot_long_labels ? 3 : 4;
        fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080',\\\n",input_filename);
        fprintf(fid,"     '%s' using ($1+xoffset):($2+yoffset):%u with labels font 'arial,10'\n", input_filename,label_line);
    }

    // close it up
    fclose(fid);

    return 0;
}

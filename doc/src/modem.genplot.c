// modem.genplot.c
//
// Generates a gnuplot file for exporting a modem figure.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <liquid/liquid.h>

// print usage/help message
void usage()
{
    printf("modem.genplot [options]\n");
    printf("  u/h   : print usage\n");
    printf("  g     : specify gnuplot version\n");
    printf("  t     : specify gnuplot terminal (e.g. pdf, jpg, eps)\n");
    printf("  d     : input data filename\n");
    printf("  f     : output gnuplot filename\n");
    printf("  T     : figure title\n");
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
    char figure_title[256] = "constellation";

    int dopt;
    while ((dopt = getopt(argc,argv,"uhg:t:d:f:T:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'g':   gnuplot_version = atof(optarg);         break;
        case 't':   gnuplot_term = atoi(optarg);            break;
        case 'd':   strncpy(input_filename,optarg,256);     break;
        case 'f':   strncpy(output_filename,optarg,256);    break;
        case 'T':   strncpy(figure_title,optarg,256);       break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            usage();
            return 1;
        }
    }

    // write output file
    FILE * fid = fopen(output_filename,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0],output_filename);
        exit(1);
    }

    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", output_filename);
    fprintf(fid,"# invoked as :");
    unsigned int i;
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1.5:1.5]\n");
    fprintf(fid,"set yrange [-1.5:1.5]\n");
    fprintf(fid,"set size square\n");
    fprintf(fid,"set title \"%s\"\n", figure_title);
    fprintf(fid,"set xlabel \"I\"\n");
    fprintf(fid,"set ylabel \"Q\"\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics lc rgb '#999999' lw 1\n");
    fprintf(fid,"set pointsize 1.0\n");
    //fprintf(fid,"set grid\n");
    if (gnuplot_version < 4.2f) {
        fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080'\n",input_filename);
    } else {
        fprintf(fid,"xoffset = 0.06\n");
        fprintf(fid,"yoffset = 0.06\n");
        fprintf(fid,"plot '%s' using 1:2 with points pointtype 7 linecolor rgb '#004080',\\\n",input_filename);
        fprintf(fid,"     '%s' using ($1+xoffset):($2+yoffset):4 with labels font 'arial,10'\n", input_filename);
    }

    // close it up
    fclose(fid);

    return 0;
}

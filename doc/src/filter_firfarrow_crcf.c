//
// filter_firfarrow_crcf.c
//
// Demonstrates the functionality of the finite impulse response Farrow
// filter for arbitrary fractional sample group delay.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_LBL "figures.gen/filter_firfarrow_labels.dat"
#define OUTPUT_FILENAME_GNU "figures.gen/filter_firfarrow_groupdelay.gnu"

int main(int argc, char*argv[])
{
    // options
    unsigned int h_len=19;  // filter length
    unsigned int p=5;       // polynomial order
    float fc=0.45f;         // filter cutoff
    float As=60.0f;         // stop-band attenuation [dB]
    unsigned int m=9;       // number of delays to evaluate
    unsigned int nfft=256;  // number of frequency points

    // derived values
    float tau = ((float)h_len-1)/2.0f;  // nominal filter delay

    firfarrow_crcf q = firfarrow_crcf_create(h_len, p, fc, As);

    // arrays
    float mu_vect[m];
    float f[nfft];
    float groupdelay[m*nfft];

    unsigned int i;
    unsigned int n;

    //
    for (i=0; i<m; i++)
        mu_vect[i] = ((float)i)/((float)m-1) - 0.5f;

    //
    for (n=0; n<nfft; n++)
        f[n] = 0.5f * (float)n / (float)nfft;

    // compute delay
    for (i=0; i<m; i++) {
        firfarrow_crcf_set_delay(q,mu_vect[i]);

        for (n=0; n<nfft; n++) {
            // compute delay
            float g = firfarrow_crcf_groupdelay(q,f[n]);

            // store result to array
            matrix_access(groupdelay,nfft,m,n,i) = g;
        }
    }

    // destroy object
    firfarrow_crcf_destroy(q);


    // 
    // export output file(s)
    //
    FILE * fid;

    // 
    // data file(s)
    //
    char filename_dat[64];
    for (i=0; i<m; i++) {
        sprintf(filename_dat,"figures.gen/filter_firfarrow_crcf_%u.dat", i);
        fid = fopen(filename_dat,"w");

        if (!fid) {
            fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], filename_dat);
            exit(1);
        }

        fprintf(fid,"# %s: auto-generated file\n\n", filename_dat);

        // print results
        for (n=0; n<nfft; n++) {
            fprintf(fid,"  %12.4e %12.4e %12.4e\n",
                        f[n],
                        tau + mu_vect[i],
                        matrix_access(groupdelay,nfft,m,n,i));
        }

        fclose(fid);
        printf("results written to %s\n", filename_dat);
    }

    // 
    // labels file
    //
    fid = fopen(OUTPUT_FILENAME_LBL,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_LBL);
    for (i=0; i<m; i++) {
        fprintf(fid," %12.4e %12.4e \"mu=%6.3f\"\n", 0.3,
                        tau + mu_vect[i] + 0.03f, mu_vect[i]);

    }
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_LBL);

    // 
    // group delay gnuplot file
    //
    fid = fopen(OUTPUT_FILENAME_GNU,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_GNU);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:0.5]\n");
    fprintf(fid,"set yrange [%f:%f]\n", tau-1.0f, tau+1.0f);
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Group Delay'\n");
    fprintf(fid,"set notitle\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);

    fprintf(fid,"# real\n");
    fprintf(fid,"plot \\\n");
    for (i=0; i<m; i++) {
        sprintf(filename_dat,"figures.gen/filter_firfarrow_crcf_%u.dat", i);
        fprintf(fid,"  '%s' using 1:3 with lines linewidth 3 linecolor rgb '#000000' notitle,\\\n",filename_dat);
        fprintf(fid,"  '%s' using 1:2 with lines linewidth 1 linecolor rgb '#000000' notitle,\\\n",filename_dat);
    }
    fprintf(fid,"  '%s' using 1:2:3 with labels font 'arial,10' notitle\n", OUTPUT_FILENAME_LBL);

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME_GNU);

    printf("done.\n");
    return 0;
}


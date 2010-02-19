// 
// filter_iirdes.c
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("iirdes_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  t     : filter type: [butter], cheby1, cheby2, ellip, bessel\n");
    printf("  n     : filter order\n");
    printf("  r     : passband ripple [dB]\n");
    printf("  s     : stopband attenuation [dB]\n");
    printf("  f     : passband cut-off [0,0.5]\n");
    printf("  w     : fft size [1024]\n");
    printf("  g     : output gnuplot filename (required)\n");
}


int main(int argc, char*argv[]) {
    // options
    unsigned int n=6;       // filter order
    float fc = 0.25f;       // cutoff
    float slsl = 60.0f;     // stopband attenuation [dB]
    float ripple = 1.0f;    // passband ripple [dB]
    unsigned int nfft=1024; // output fft size
    char gnuplot_filename[256];
    bool gnuplot_filename_given = false;

    // filter type
    enum {  IIRDES_EXAMPLE_BUTTER=0,
            IIRDES_EXAMPLE_CHEBY1,
            IIRDES_EXAMPLE_CHEBY2,
            IIRDES_EXAMPLE_ELLIP,
            IIRDES_EXAMPLE_BESSEL
    } type = 0;

    int dopt;
    while ((dopt = getopt(argc,argv,"uht:n:r:s:f:w:g:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 't':
            if (strcmp(optarg,"butter")==0) {
                type = IIRDES_EXAMPLE_BUTTER;
            } else if (strcmp(optarg,"cheby1")==0) {
                type = IIRDES_EXAMPLE_CHEBY1;
            } else if (strcmp(optarg,"cheby2")==0) {
                type = IIRDES_EXAMPLE_CHEBY2;
            } else if (strcmp(optarg,"ellip")==0) {
                type = IIRDES_EXAMPLE_ELLIP;
            } else if (strcmp(optarg,"bessel")==0) {
                type = IIRDES_EXAMPLE_BESSEL;
            } else {
                fprintf(stderr,"error: iirdes_example, unknown filter type \"%s\"\n", optarg);
                usage();
                exit(1);
            }
            break;
        case 'n': n = atoi(optarg);         break;
        case 'r': ripple = atof(optarg);    break;
        case 's': slsl = atof(optarg);      break;
        case 'f': fc = atof(optarg);        break;
        case 'w': nfft = atoi(optarg);      break;
        case 'g':
            strncpy(gnuplot_filename,optarg,256);
            gnuplot_filename_given = true;
            break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            usage();
            return 1;
        }
    }

    // fail if no filename is given
    if (!gnuplot_filename_given) {
        fprintf(stderr,"error: %s, gnuplot filename not specified\n", argv[0]);
        usage();
        return 1;
    }

    // normalize nfft size
    nfft = 1 << liquid_nextpow2(nfft);
    unsigned int nfft2 = nfft/2;

    // number of analaog poles/zeros
    unsigned int npa = n;
    unsigned int nza;

    // analog poles/zeros/gain
    float complex pa[n];
    float complex za[n];
    float complex ka;

    unsigned int r = n%2;
    unsigned int L = (n-r)/2;

    unsigned int i;
    // specific filter variables
    float epsilon, Gp, Gs, ep, es;

    switch (type) {
    case IIRDES_EXAMPLE_BUTTER:
        printf("Butterworth filter design:\n");
        nza = 0;
        butter_azpkf(n,fc,za,pa,&ka);
        break;
    case IIRDES_EXAMPLE_CHEBY1:
        printf("Cheby-I filter design:\n");
        nza = 0;
        epsilon = sqrtf( powf(10.0f, ripple / 10.0f) - 1.0f );
        cheby1_azpkf(n,fc,epsilon,za,pa,&ka);
        break;
    case IIRDES_EXAMPLE_CHEBY2:
        printf("Cheby-II filter design:\n");
        nza = 2*L;
        epsilon = powf(10.0f, -slsl/20.0f);
        cheby2_azpkf(n,fc,epsilon,za,pa,&ka);
        break;
    case IIRDES_EXAMPLE_ELLIP:
        printf("elliptic filter design:\n");
        nza = 2*L;
        Gp = powf(10.0f, -ripple  / 20.0f);
        Gs = powf(10.0f, -slsl    / 20.0f);
        printf("  Gp = %12.8f\n", Gp);
        printf("  Gs = %12.8f\n", Gs);

        // epsilon values
        ep = sqrtf(1.0f/(Gp*Gp) - 1.0f);
        es = sqrtf(1.0f/(Gs*Gs) - 1.0f);

        ellip_azpkf(n,fc,ep,es,za,pa,&ka);
        break;
    case IIRDES_EXAMPLE_BESSEL:
        printf("Bessel filter design:\n");
        bessel_azpkf(n,za,pa,&ka);
        nza = 0;
        break;
    default:
        fprintf(stderr,"error: iirdes_example: unknown type\n");
        exit(1);
    }

    // complex digital poles/zeros/gain
    float complex zd[n];
    float complex pd[n];
    float complex kd;
    float m = 1.0f / tanf(M_PI * fc);
    bilinear_zpkf(za,    nza,
                  pa,    npa,
                  ka,    m,
                  zd, pd, &kd);

    // second-order sections
    float A[3*(L+r)];
    float B[3*(L+r)];

    // convert complex digital poles/zeros/gain into second-
    // order sections form
    iirdes_dzpk2sosf(zd,pd,n,kd,B,A);

    // compute spectrum
    float complex H[nfft];
    float complex H0[nfft];
    for (i=0; i<nfft; i++) H[i] = 1.;

    unsigned int j;
    for (i=0; i<L+r; i++) {
        liquid_doc_freqz(&B[3*i], 3, &A[3*i], 3, nfft, H0);
        for (j=0; j<nfft; j++)
            H[j] *= H0[j];
    }
    // ensure response is not zero
    float eps = 1e-12f;
    for (j=0; j<nfft; j++) {
        if(cabsf(H[j]) < eps)
            H[j] = eps;
    }

    // compute group delay
    float groupdelay[nfft];
    for (i=0; i<nfft; i++) {
        float f = ((float)i/(float)nfft);
        if (i >= nfft2)
            f = f - 1.0f;

        groupdelay[i] = 0.0f;
        for (j=0; j<L+r; j++)
            groupdelay[i] += iir_group_delay(&B[3*j],3,&A[3*j],3,f) - 2.0f;
    }

    // open output file
    FILE*fid = fopen(gnuplot_filename,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", gnuplot_filename);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:0.5];\n");
    fprintf(fid,"set yrange [-80:5]\n");
    fprintf(fid,"set size ratio 0.6\n");
    //fprintf(fid,"set title 'filter design'\n");
    fprintf(fid,"set xlabel 'Normalized Frequency'\n");
    fprintf(fid,"set ylabel 'Power Spectral Density [dB]'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '-' using 2:3 with lines linetype 1 linewidth 4 linecolor rgb '%s'\n",LIQUID_DOC_COLOR_BLUE);
    fprintf(fid,"#    i   freq         PSD  \n");
    for (i=0; i<nfft2; i++) {
        float f = (float)i/(float)nfft;
        fprintf(fid,"  %4u %12.4e %12.4e\n", i, f, 20*log10(cabsf(H[i])));
    }

#if 0
    // print digital z/p/k
    fprintf(fid,"zd = zeros(1,n);\n");
    fprintf(fid,"pd = zeros(1,n);\n");
    for (i=0; i<n; i++) {
        fprintf(fid,"  zd(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(zd[i]), cimagf(zd[i]));
        fprintf(fid,"  pd(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(pd[i]), cimagf(pd[i]));
    }
#endif
    fclose(fid);
    //printf("results written to %s.\n", gnuplot_filename);

    return 0;
}


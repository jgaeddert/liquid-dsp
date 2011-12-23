// 
// filter_iirdes.c
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
    printf("filter_iirdes [options]\n");
    printf("  u/h   : print usage\n");
    printf("  t     : filter type: [butter], cheby1, cheby2, ellip, bessel\n");
    printf("  n     : filter order\n");
    printf("  r     : passband ripple [dB]\n");
    printf("  s     : stopband attenuation [dB]\n");
    printf("  f     : passband cut-off [0,0.5]\n");
    printf("  w     : fft size [1024]\n");
    printf("output filenames are generated from the filter type:\n");
    printf("  figures.gen/filter_iirdes_<type>_psd.gnu (power spectral density)\n");
    printf("  figures.gen/filter_iirdes_<type>_zpk.gnu (zeros/poles/gain)\n");
}

int main(int argc, char*argv[]) {
    // options
    unsigned int n=6;       // filter order
    float fc = 0.25f;       // cutoff
    float f0 = 0.25f;       // center frequency
    float slsl = 60.0f;     // stopband attenuation [dB]
    float ripple = 1.0f;    // passband ripple [dB]
    unsigned int nfft=1024; // output fft size
    char filter_str[64];

    // filter type
    liquid_iirdes_filtertype type = LIQUID_IIRDES_BUTTER;

    int dopt;
    while ((dopt = getopt(argc,argv,"uht:n:r:s:f:w:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 't':
            // copy filter type string
            strncpy(filter_str,optarg,64);
            filter_str[63] = '\0';

            if (strcmp(optarg,"butter")==0) {
                type = LIQUID_IIRDES_BUTTER;
            } else if (strcmp(optarg,"cheby1")==0) {
                type = LIQUID_IIRDES_CHEBY1;
            } else if (strcmp(optarg,"cheby2")==0) {
                type = LIQUID_IIRDES_CHEBY2;
            } else if (strcmp(optarg,"ellip")==0) {
                type = LIQUID_IIRDES_ELLIP;
            } else if (strcmp(optarg,"bessel")==0) {
                type = LIQUID_IIRDES_BESSEL;
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
        default:
            exit(1);
        }
    }

    // configure filenames
    char filename_psd[256] = "figures.gen/filter_";
    strcat(filename_psd, filter_str);
    strcat(filename_psd, "_psd.gnu");

    char filename_zpk[256] = "figures.gen/filter_";
    strcat(filename_zpk, filter_str);
    strcat(filename_zpk, "_zpk.gnu");

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
    float complex k0 = 1.0f;

    unsigned int r = n%2;
    unsigned int L = (n-r)/2;

    unsigned int i;
    // specific filter variables
    float epsilon=0, Gp, Gs, ep, es;

    switch (type) {
    case LIQUID_IIRDES_BUTTER:
        printf("Butterworth filter design:\n");
        nza = 0;
        k0 = 1.0f;
        butter_azpkf(n,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY1:
        printf("Cheby-I filter design:\n");
        nza = 0;
        epsilon = sqrtf( powf(10.0f, ripple / 10.0f) - 1.0f );
        k0 = r ? 1.0f : 1.0f / sqrtf(1.0f + epsilon*epsilon);
        cheby1_azpkf(n,epsilon,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY2:
        printf("Cheby-II filter design:\n");
        nza = 2*L;
        epsilon = powf(10.0f, -slsl/20.0f);
        k0 = 1.0f;
        cheby2_azpkf(n,epsilon,za,pa,&ka);
        break;
    case LIQUID_IIRDES_ELLIP:
        printf("elliptic filter design:\n");
        nza = 2*L;
        Gp = powf(10.0f, -ripple  / 20.0f);
        Gs = powf(10.0f, -slsl    / 20.0f);
        //printf("  Gp = %12.8f\n", Gp);
        //printf("  Gs = %12.8f\n", Gs);

        // epsilon values
        ep = sqrtf(1.0f/(Gp*Gp) - 1.0f);
        es = sqrtf(1.0f/(Gs*Gs) - 1.0f);
        k0 = r ? 1.0f : 1.0f / sqrtf(1.0f + epsilon*epsilon);

        ellip_azpkf(n,ep,es,za,pa,&ka);
        break;
    case LIQUID_IIRDES_BESSEL:
        printf("Bessel filter design:\n");
        bessel_azpkf(n,za,pa,&ka);
        nza = 0;
        k0 = 1.0f;
        break;
    default:
        fprintf(stderr,"error: iirdes_example: unknown type\n");
        exit(1);
    }

    // complex digital poles/zeros/gain
    float complex zd[n];
    float complex pd[n];
    float complex kd;
    float m = iirdes_freqprewarp(LIQUID_IIRDES_LOWPASS,fc,f0);
    bilinear_zpkf(za,    nza,
                  pa,    npa,
                  k0,    m,
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

    // 
    // generate plots
    //
    FILE * fid = NULL;

    // 
    // generate spectrum plot
    //

    // open output file
    fid = fopen(filename_psd,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0], filename_psd);
        exit(1);
    }
    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename_psd);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [0:0.5];\n");
    fprintf(fid,"set yrange [-80:5]\n");
    fprintf(fid,"set size ratio 0.5\n");
    fprintf(fid,"set size 0.8\n");
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
    fclose(fid);
    //printf("results written to %s.\n", filename_psd);

    // 
    // generate zeros/poles/gain plot
    //

    // write output file
    fid = fopen(filename_zpk,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open file \"%s\" for writing.\n", argv[0], filename_zpk);
        exit(1);
    }
    // print header
    fprintf(fid,"# %s : auto-generated file (do not edit)\n", filename_zpk);
    fprintf(fid,"# invoked as :");
    for (i=0; i<argc; i++)
        fprintf(fid," %s",argv[i]);
    fprintf(fid,"\n");
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [-1.05:1.05]\n");
    fprintf(fid,"set yrange [-1.05:1.05]\n");
    fprintf(fid,"set size ratio 1\n");
    fprintf(fid,"set size 0.8\n");
    //fprintf(fid,"set title \"%s\"\n", figure_title);
    //fprintf(fid,"set xlabel 'in-phase'\n");
    //fprintf(fid,"set ylabel 'quadrature-phase'\n");
    //fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid polar\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' linewidth 1\n",LIQUID_DOC_COLOR_GRAY);
    fprintf(fid,"set pointsize 1.5\n");
    fprintf(fid,"unset border # disable axis box\n");
    fprintf(fid,"set xtics (0,0.5,1) axis nomirror\n");
    fprintf(fid,"unset ytics\n");

    fprintf(fid,"plot '-' using 1:2 with points pointtype 6 linewidth 5 linecolor rgb '%s' title 'zeros',\\\n", LIQUID_DOC_COLOR_PURPLE);
    fprintf(fid,"     '-' using 1:2 with points pointtype 2 linewidth 5 linecolor rgb '%s' title 'poles'\n",    LIQUID_DOC_COLOR_GREEN);
    // print digital zeros
    for (i=0; i<n; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(zd[i]), cimagf(zd[i]));
    fprintf(fid,"e\n");
    // print digital poles
    for (i=0; i<n; i++)
        fprintf(fid,"  %12.4e %12.4e\n", crealf(pd[i]), cimagf(pd[i]));
    fprintf(fid,"e\n");

    // close it up
    fclose(fid);


    return 0;
}


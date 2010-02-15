// iirdes_example.c
//
// Tests infinite impulse reponse (IIR) filter design.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_example.m"

// print usage/help message
void usage()
{
    printf("irdes_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  t     : filter type: [butter], cheby1, cheby2, ellip, bessel\n");
    printf("  n     : filter order\n");
    printf("  o     : format [sos], tf\n");
#if 0
    printf("  r     : passband ripple [dB]\n");
    printf("  s     : stopband attenuation [dB]\n");
    printf("  f     : passband cut-off [0,0.5]\n");
#endif
    printf("  ...\n");
}


int main(int argc, char*argv[]) {
    // options
    unsigned int n=6;   // filter order
    float fc = 0.30f;   // cutoff
    float slsl = 60.0f; // stopband attenuation [dB]
    float ripple = 1.0f;// passband ripple [dB]

    // filter type
    enum {  IIRDES_EXAMPLE_BUTTER=0,
            IIRDES_EXAMPLE_CHEBY1,
            IIRDES_EXAMPLE_CHEBY2,
            IIRDES_EXAMPLE_ELLIP,
            IIRDES_EXAMPLE_BESSEL
    } type = 0;

    // output format
    enum {  IIRDES_EXAMPLE_SOS=0,
            IIRDES_EXAMPLE_TF
    } format = 0;

    int dopt;
    while ((dopt = getopt(argc,argv,"uht:n:o:")) != EOF) {
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
        case 'n':
            n = atoi(optarg);
            break;
        case 'o':
            if (strcmp(optarg,"sos")==0) {
                format = IIRDES_EXAMPLE_SOS;
            } else if (strcmp(optarg,"tf")==0) {
                format = IIRDES_EXAMPLE_TF;
            } else {
                fprintf(stderr,"error: iirdes_example, unknown output format \"%s\"\n", optarg);
                usage();
                exit(1);
            }
            break;
        default:
            fprintf(stderr,"error: iirdes_example, unknown option %s\n", optarg);
            usage();
            return 1;
        }
    }

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
    float b[n+1];       // numerator
    float a[n+1];       // denominator

    // second-order sections
    float A[3*(L+r)];
    float B[3*(L+r)];

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

    printf("poles (analog):\n");
    for (i=0; i<npa; i++)
        printf("  pa[%3u] = %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));
    printf("zeros (analog):\n");
    for (i=0; i<nza; i++)
        printf("  za[%3u] = %12.8f + j*%12.8f\n", i, crealf(za[i]), cimagf(za[i]));
    printf("gain (analog):\n");
    printf("  ka : %12.8f + j*%12.8f\n", crealf(ka), cimagf(ka));

    // complex digital poles/zeros/gain
    float complex zd[n];
    float complex pd[n];
    float complex kd;
    float m = 1.0f / tanf(M_PI * fc);
    iirdes_zpka2df(za,    nza,
                   pa,    npa,
                   ka,    m,
                   zd, pd, &kd);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"r=%u;\n", r);
    fprintf(fid,"L=%u;\n", L);
    fprintf(fid,"nfft=1024;\n");

    if (format == IIRDES_EXAMPLE_TF) {
        // convert complex digital poles/zeros/gain into transfer function
        iirdes_dzpk2tff(zd,pd,n,kd,b,a);

        // print coefficients
        for (i=0; i<=n; i++) printf("a(%3u) = %12.8f;\n", i+1, a[i]);
        for (i=0; i<=n; i++) printf("b(%3u) = %12.8f;\n", i+1, b[i]);

        fprintf(fid,"a = zeros(1,n+1);\n");
        fprintf(fid,"b = zeros(1,n+1);\n");
        for (i=0; i<=n; i++) {
            fprintf(fid,"a(%3u) = %12.4e;\n", i+1, a[i]);
            fprintf(fid,"b(%3u) = %12.4e;\n", i+1, b[i]);
        }
        fprintf(fid,"\n");
        fprintf(fid,"H = fft(b,nfft)./fft(a,nfft);\n");
        fprintf(fid,"H = fftshift(H);\n");
    } else {
        // convert complex digital poles/zeros/gain into second-
        // order sections form
        iirdes_dzpk2sosf(zd,pd,n,kd,B,A);

        unsigned int j;
        for (i=0; i<L+r; i++) {
            for (j=0; j<3; j++) {
                fprintf(fid,"B(%3u,%3u) = %16.8e;\n", i+1, j+1, B[3*i+j]);
                fprintf(fid,"A(%3u,%3u) = %16.8e;\n", i+1, j+1, A[3*i+j]);
            }
        }
        fprintf(fid,"\n");
        fprintf(fid,"H = ones(1,nfft);\n");
        fprintf(fid,"for i=1:(L+r),\n");
        fprintf(fid,"    H = H .* fft(B(i,:),nfft)./fft(A(i,:),nfft);\n");
        fprintf(fid,"end;\n");
        fprintf(fid,"H = fftshift(H);\n");
    }

    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1),\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)),'-','Color',[0.5 0 0],'LineWidth',2);\n");
    fprintf(fid,"  axis([0.0 0.5 -4 1]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Filter PSD [dB]');\n");
    fprintf(fid,"subplot(2,1,2),\n");
    fprintf(fid,"  plot(f,20*log10(abs(H)),'-','Color',[0.5 0 0],'LineWidth',2);\n");
    fprintf(fid,"  axis([0.0 0.5 -100 10]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('Normalized Frequency');\n");
    fprintf(fid,"  ylabel('Filter PSD [dB]');\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


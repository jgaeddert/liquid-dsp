// iirdes_example.c
//
// Tests infinite impulse reponse (IIR) filter design.
//

#include <stdlib.h>
#include <stdio.h>
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
    float fc = 0.25f;   // cutoff
    float slsl = 60.0f; // stopband attenuation [dB]
    float ripple = 0.5f;// passband ripple [dB]
    enum {  IIRDES_EXAMPLE_BUTTER=0,
            IIRDES_EXAMPLE_CHEBY1,
            IIRDES_EXAMPLE_CHEBY2,
            IIRDES_EXAMPLE_ELLIP,
            IIRDES_EXAMPLE_BESSEL
    } type = 0;

    int dopt;
    while ((dopt = getopt(argc,argv,"uht:n:")) != EOF) {
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
                fprintf(stderr,"error: iirdes_example, unknown fileter type \"%s\"\n", optarg);
                usage();
                exit(1);
            }
            break;
        case 'n':
            n = atoi(optarg);
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

    // convert complex digital poles/zeros/gain into transfer function
    iirdes_dzpk2tff(zd,pd,n,kd,b,a);

    // print coefficients
    for (i=0; i<=n; i++) printf("a(%3u) = %12.8f;\n", i+1, a[i]);
    for (i=0; i<=n; i++) printf("b(%3u) = %12.8f;\n", i+1, b[i]);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\nclear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"a = zeros(1,n+1);\n");
    fprintf(fid,"b = zeros(1,n+1);\n");
    for (i=0; i<=n; i++) {
        fprintf(fid,"a(%3u) = %12.4e;\n", i+1, a[i]);
        fprintf(fid,"b(%3u) = %12.4e;\n", i+1, b[i]);
    }
    fprintf(fid,"\n");
    fprintf(fid,"freqz(b,a);\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


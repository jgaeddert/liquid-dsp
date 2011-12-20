// 
// random_histogram.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"
#include "liquid.doc.h"

// print usage/help message
void usage()
{
    printf("random_histogram_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  f     : output filename\n");
    printf("  N     : number of trials\n");
    printf("  n     : number of histogram bins\n");
    printf("  d     : distribution: {uniform, normal, exp, weib, gamma, nak, rice}\n");
    printf("  e     : eta    NORMAL: mean\n");
    printf("  s     : sigma  NORMAL: standard deviation\n");
    printf("  l     : lambda EXPONENTIAL: decay factor\n");
    printf("  a     : alpha  WEIBULL: shape\n");
    printf("  b     : beta   WEIBULL: spread\n");
    printf("  g     : gamma  WEIBULL: threshold\n");
    printf("  A     : alpha  GAMMA: shape\n");
    printf("  B     : beta   GAMMA: spread\n");
    printf("  m     : m      NAKAGAMI: shape\n");
    printf("  o     : omega  NAKAGAMI: spread\n");
    printf("  K     : K      RICE-K: spread\n");
    printf("  O     : omega  RICE-K: spread\n");
}

int main(int argc, char*argv[])
{
    unsigned long int num_trials = 100000; // number of trials
    unsigned int num_bins = 20;
    enum {
        UNIFORM=0,
        NORMAL,
        EXPONENTIAL,
        WEIBULL,
        GAMMA,
        NAKAGAMIM,
        RICEK
    } distribution=0;
    char filename[256] = "";

    // distribution parameters
    float eta = 0.0f;       // NORMAL: mean
    float sigma = 1.0f;     // NORMAL: standard deviation
    float lambda = 3.0f;    // EXPONENTIAL: decay factor
    float alphaw = 1.5f;    // WEIBULL: shape
    float betaw = 1.0f;     // WEIBULL: spread
    float gammaw = 1.0f;    // WEIBULL: threshold
    float alphag = 4.5f;    // GAMMA: shape
    float betag = 1.0f;     // GAMMA: spread
    float m = 4.5f;         // NAKAGAMI: shape factor
    float omeganak = 1.0f;  // NAKAGMAI: spread factor
    float K = 4.0f;         // RICE-K: K-factor (shape)
    float omegarice = 1.0f; // RICE-K: spread factor

    int dopt;
    while ((dopt = getopt(argc,argv,"uhf:n:d:e:s:l:a:b:g:A:B:m:o:K:O:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'f': strncpy(filename,optarg,255); filename[255]='\0'; break;
        case 'N': num_trials = atoi(optarg); break;
        case 'n': num_bins = atoi(optarg); break;
        case 'd':
            if      (strcmp(optarg,"uniform")==0)   distribution = UNIFORM;
            else if (strcmp(optarg,"normal")==0)    distribution = NORMAL;
            else if (strcmp(optarg,"exp")==0)       distribution = EXPONENTIAL;
            else if (strcmp(optarg,"weib")==0)      distribution = WEIBULL;
            else if (strcmp(optarg,"gamma")==0)     distribution = GAMMA;
            else if (strcmp(optarg,"nak")==0)       distribution = NAKAGAMIM;
            else if (strcmp(optarg,"rice")==0)      distribution = RICEK;
            else {
                fprintf(stderr,"error: %s, unknown/unsupported distribution '%s'\n", argv[0], optarg);
                exit(1);
            }
            break;
        case 'e': eta       = atof(optarg); break;
        case 's': sigma     = atof(optarg); break;
        case 'l': lambda    = atof(optarg); break;
        case 'a': alphaw    = atof(optarg); break;
        case 'b': betaw     = atof(optarg); break;
        case 'g': gammaw    = atof(optarg); break;
        case 'A': alphag    = atof(optarg); break;
        case 'B': betag     = atof(optarg); break;
        case 'm': m         = atof(optarg); break;
        case 'o': omeganak  = atof(optarg); break;
        case 'K': K         = atof(optarg); break;
        case 'O': omegarice = atof(optarg); break;
        default:
            exit(1);
        }
    }

    float xmin = 0.0f;
    float xmax = 1.0f;

    unsigned long int i;

    // make a guess at the histogram range so we don't need to
    // store all the generated random variables in a giant array.
    if (distribution == UNIFORM) {
        xmin =  0.0f;
        xmax =  1.0f;
    } else if (distribution == NORMAL) {
        xmin = eta - 4.0f*sigma;
        xmax = eta + 4.0f*sigma;
    } else if (distribution == EXPONENTIAL) {
        xmin = 0.0f;
        xmax = 7.0f / lambda;
    } else if (distribution == WEIBULL) {
        xmin = gammaw;
        xmax = gammaw + betaw*powf( -logf(1e-3f), 1.0f/alphaw );
    } else if (distribution == GAMMA) {
        xmin = 0.0f;
        xmax = 6.5 * betag + 2.0*alphag;
    } else if (distribution == NAKAGAMIM) {
        xmin = 0.0f;
        xmax = 2.0f * sqrtf(omeganak);
    } else if (distribution == RICEK) {
        xmin = 0.0f;
        xmax = 2.3f * sqrtf(omegarice);
    } else {
        fprintf(stderr, "error: %s, unknown/unsupported distribution\n", argv[0]);
        exit(1);
    }

    //
    float xspan = xmax - xmin;
    float bin_width = (xmax - xmin) / (num_bins);

    // initialize histogram
    unsigned int hist[num_bins];
    for (i=0; i<num_bins; i++)
        hist[i] = 0;

    // generate random variables
    float x = 0.0f;
    for (i=0; i<num_trials; i++) {
        switch (distribution) {
        case UNIFORM:     x = randf(); break;
        case NORMAL:      x = sigma*randnf() + eta; break;
        case EXPONENTIAL: x = randexpf(lambda); break;
        case WEIBULL:     x = randweibf(alphaw,betaw,gammaw); break;
        case GAMMA:       x = randgammaf(alphag,betag); break;
        case NAKAGAMIM:   x = randnakmf(m,omeganak); break;
        case RICEK:       x = randricekf(K,omegarice); break;
        default:
            fprintf(stderr,"error: %s, unknown/unsupported distribution\n", argv[0]);
            exit(1);
        }

        // compute bin index
        unsigned int index;
        float ihat = num_bins * (x - xmin) / (xmax - xmin);
        if (ihat < 0.0f)
            index = 0;
        else
            index = (unsigned int)ihat;
        
        if (index >= num_bins)
            index = num_bins-1;

        hist[index]++;
    }

    float x_pdf[num_bins];  // PDF
    float x_cdf[num_bins];  // CDF
    float h[num_bins];      // emperical PDF (histogram)
    float H[num_bins];      // emperical CDF

    for (i=0; i<num_bins; i++) {
        x_pdf[i] = xmin + ((float)i + 0.5f)*bin_width;
        h[i] = (float)(hist[i]) / (num_trials * bin_width);
        //fprintf(fid,"xh(%3lu) = %12.4e; h(%3lu) = %12.4e;\n", i+1, x[i], i+1, h[i]);

        x_cdf[i] = xmin + ((float)i + 1.0f)*bin_width;
        H[i] = (i==0) ? 0.0f : H[i-1] + h[i];
        //fprintf(fid,"xH(%3lu) = %12.4e; H(%3lu) = %12.4e;\n", i+1, X[i], i+1, H[i]);
    }

    // compute expected distribution
    unsigned int num_steps = 100;
    float xstep = (xmax - xmin) / (num_steps - 1);
    float x_hat[num_steps]; //
    float f[num_steps];     // expected PDF
    float F[num_steps];     // expected CDF
    float fmax = 0.0f;
    for (i=0; i<num_steps; i++) {
        x = xmin + i*xstep;
        x_hat[i] = x;
        switch (distribution) {
        case UNIFORM:
            f[i] = randf_pdf(x);
            F[i] = randf_cdf(x);
            break;
        case NORMAL:
            f[i] = randnf_pdf(x,eta,sigma);
            F[i] = randnf_cdf(x,eta,sigma);
            break;
        case EXPONENTIAL:
            f[i] = randexpf_pdf(x,lambda);
            F[i] = randexpf_cdf(x,lambda);
            break;
        case WEIBULL:
            f[i] = randweibf_pdf(x,alphaw,betaw,gammaw);
            F[i] = randweibf_cdf(x,alphaw,betaw,gammaw);
            break;
        case GAMMA:
            f[i] = randgammaf_pdf(x,alphag,betag);
            F[i] = randgammaf_cdf(x,alphag,betag);
            break;
        case NAKAGAMIM:
            f[i] = randnakmf_pdf(x,m,omeganak);
            F[i] = randnakmf_cdf(x,m,omeganak);
            break;
        case RICEK:
            f[i] = randricekf_pdf(x,K,omegarice);
            F[i] = randricekf_cdf(x,K,omegarice);
            break;
        default:
            fprintf(stderr,"error: %s, unknown/unsupported distribution\n", argv[0]);
            exit(1);
        }

        if (f[i] > fmax) fmax = f[i];
    }

    // 
    // open/initialize output file
    //
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", filename);
    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set xrange [%f:%f];\n", xmin-0.1f*xspan, xmax+0.1f*xspan);
    fprintf(fid,"set yrange [0:%f]\n", 1.2*fmax);
    fprintf(fid,"set size 0.8\n");
    fprintf(fid,"set size ratio 0.8\n");
    fprintf(fid,"set xlabel 'x'\n");
    fprintf(fid,"set ylabel 'Probability Density'\n");
    fprintf(fid,"set key top right nobox\n");
    //fprintf(fid,"set ytics -5,1,5\n");
    //fprintf(fid,"set grid xtics ytics\n");
    //fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set boxwidth %f\n", 0.9f*bin_width);
    fprintf(fid,"plot '-' using 1:2 with boxes fs solid linecolor rgb '#ccddee' title 'histogram',\\\n");
    fprintf(fid,"     '-' using 1:2 with lines linewidth 5 linecolor rgb '#004080' title 'true PDF'\n");

    // print emperical (histogram)
    for (i=0; i<num_bins; i++)
        fprintf(fid,"  %12.8f %12.4e %12.8f %12.4e\n", x_pdf[i], h[i], x_cdf[i], H[i]);
    fprintf(fid,"e\n");

    // print expected
    for (i=0; i<num_steps; i++)
        fprintf(fid,"  %12.8f %12.4e %12.4e\n", x_hat[i], f[i], F[i]);
    fprintf(fid,"e\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}


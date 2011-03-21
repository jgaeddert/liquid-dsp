// 
// random_histogram_example.c
//
// This example tests the random number generators for different
// distributions.
//

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

#define OUTPUT_FILENAME "random_histogram_example.m"

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

    distribution = NAKAGAMIM;

    float xmin = 0.0f;
    float xmax = 1.0f;

    unsigned long int i;

    if (distribution == UNIFORM) {
        xmin =  0.0f;
        xmax =  1.0f;
    } else if (distribution == NORMAL) {
        xmin = -3.0f;
        xmax =  3.0f;
    } else if (distribution == EXPONENTIAL) {
        xmin = 0.0f;
        xmax = 3.0f;
    } else if (distribution == WEIBULL) {
        xmin = 0.0f;
        xmax = 4.0f;
    } else if (distribution == GAMMA) {
        xmin = 0.0f;
        xmax = 14.0f;
    } else if (distribution == NAKAGAMIM) {
        xmin = 0.0f;
        xmax = 2.0f;
    } else if (distribution == RICEK) {
        xmin = 0.0f;
        xmax = 2.2f;
    } else {
        fprintf(stderr, "error: %s, unknown/unsupported distribution\n", argv[0]);
        exit(1);
    }

    //
    //float xspan = xmax - xmin;
    float bin_width = (xmax - xmin) / (num_bins);

    // initialize histogram
    unsigned int hist[num_bins];
    for (i=0; i<num_bins; i++)
        hist[i] = 0;

    // generate random variables
    float x = 0.0f;
    for (i=0; i<num_trials; i++) {
        switch (distribution) {
        case UNIFORM:   x = randf(); break;
        case NORMAL:    x = randnf(); break;
        case EXPONENTIAL: x = randexpf(3.0f); break;
        case WEIBULL:   x = randweibf(1.0f,2.0f,0.0f); break;
        case GAMMA:     x = randgammaf(4.5f,1.0f); break;
        case NAKAGAMIM: x = randnakmf(4.5f,1.0f); break;
        case RICEK:     x = randricekf(4.0f,1.0f); break;
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

    // compute expected distribution
    unsigned int num_steps = 100;
    float xstep = (xmax - xmin) / (num_steps - 1);
    float f[num_steps];
    for (i=0; i<num_steps; i++) {
        x = xmin + i*xstep;
        switch (distribution) {
        case UNIFORM:   f[i] = randf_pdf(x); break;
        case NORMAL:    f[i] = randnf_pdf(x,0.0f,1.0f); break;
        case EXPONENTIAL: f[i] = randexpf_pdf(x,3.0f); break;
        case WEIBULL:   f[i] = randweibf_pdf(x,1.0f,2.0f,0.0f); break;
        case GAMMA:     f[i] = randgammaf_pdf(x,4.5f,1.0f); break;
        case NAKAGAMIM: f[i] = randnakmf_pdf(x,4.5f,1.0f); break;
        case RICEK:     f[i] = randricekf_pdf(x,4.0f,1.0f); break;
        default:
            fprintf(stderr,"error: %s, unknown/unsupported distribution\n", argv[0]);
            exit(1);
        }
    }

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"xmin = %12.4e;\n", xmin);
    fprintf(fid,"xmax = %12.4e;\n", xmax);
    fprintf(fid,"xspan = xmax - xmin;\n");

    for (i=0; i<num_bins; i++) {
        x = xmin + ((float)i + 0.5f)*bin_width;
        float h = (float)(hist[i]) / (num_trials * bin_width);
        fprintf(fid,"xh(%3lu) = %12.4e; h(%3lu) = %12.4e;\n", i+1, x, i+1, h);
    }

    for (i=0; i<num_steps; i++) {
        x = xmin + i*xstep;
        fprintf(fid,"xf(%3lu) = %12.4e; f(%3lu) = %12.4e;\n", i+1, x, i+1, f[i]);
    }

    // plot results
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(xh,h,'x', xf,f,'-');\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('f_x(x)');\n");
    fprintf(fid,"axis([(xmin-0.1*xspan) (xmax+0.1*xspan) 0 1.1*max([h f])]);\n");
    fprintf(fid,"legend('histogram','true PDF',1);\n");

    fclose(fid);
    printf("results written to %s.\n",OUTPUT_FILENAME);


    printf("done.\n");
    return 0;
}


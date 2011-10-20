// 
// ann_maxnet_example.c
//
// Artificial neural network (ann) maxnet example. This example 
// demonstrates the functionality of the ann maxnet by training
// a network to recognize and separate two input patterns in a
// 2-dimensional plane.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "ann_maxnet_example.m"

// print usage/help message
void usage()
{
    printf("ann_maxnet_example:\n");
    printf("  u/h   : print usage/help\n");
    printf("  c     : number of classes (at least 2), default: 3\n");
    printf("  s     : class distribution spread (s >= 0), default: 0.2\n");
    printf("  t     : number of training epochs, default: 1000\n");
    printf("  n     : number of neurons in hidden layer, default: 6\n");
    printf("  p     : number of patterns to train, default: 250\n");
}

int main(int argc, char*argv[]) {
    // set random seed
    srand(time(NULL));

    // options
    unsigned int num_classes = 3;       // number of classes
    float spread = 0.2f;                // spreading factor
    unsigned int num_trials = 1000;     // number of training epochs
    unsigned int num_hidden = 6;        // number of hidden neurons
    unsigned int num_patterns = 250;    // number of training patterns

    int dopt;
    while ((dopt = getopt(argc,argv,"uhc:s:t:n:p:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'c': num_classes = atoi(optarg);   break;
        case 's': spread = atof(optarg);        break;
        case 't': num_trials = atoi(optarg);    break;
        case 'n': num_hidden = atoi(optarg);    break;
        case 'p': num_patterns = atoi(optarg);  break;
        default:
            usage();
            return 1;
        }
    }

    // validate input
    if (num_classes < 2) {
        fprintf(stderr,"error: %s, must have at least 2 classes\n", argv[0]);
        usage();
        exit(1);
    } else if (spread < 0) {
        fprintf(stderr,"error: %s, spreading factor must be positive\n", argv[0]);
        usage();
        exit(1);
    }

    // create network structure:
    //      2 inputs, 6 hidden neurons, 1 output
    unsigned int structure[3] = {2, num_hidden, 1};

    // input sequence in 2-dimensional plane
    float x[2*num_patterns];

    // classification
    unsigned int class[num_patterns];

    // generate random sample points
    unsigned int i;
    for (i=0; i<num_patterns; i++) {
        // assign class randomly
        class[i] = rand() % num_classes;

        // (x,y) point lies on circle with small amount of noise
        float theta = class[i] / (float)num_classes * 2 * M_PI;
        x[2*i+0] = cosf(theta) + randnf()*spread;
        x[2*i+1] = sinf(theta) + randnf()*spread;
    }

    // create network and initialize weights randomly
    maxnet q = maxnet_create(num_classes, structure, 3);
    maxnet_print(q);

    float x_test[2];
    float y_test[num_classes];
    unsigned int c_test;

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    unsigned int n;
    unsigned int num_errors;    // number of classification errors
    for (n=0; n<num_trials; n++) {
        // compute error
        float rmse = maxnet_compute_rmse(q,x,class,num_patterns);
        fprintf(fid,"e(%6u) = %16.8e;\n", n+1, rmse);

        // periodically evaluate the network and print the results
        if ((n%100)==0) {
            num_errors=0;
            for (i=0; i<num_patterns; i++) {
                maxnet_evaluate(q,&x[i*2],y_test,&c_test);
                num_errors += c_test == class[i] ? 0 : 1;
            }
            printf("epoch %6u : %12.8f (classification errors: %6u / %6u)\n",
                    n, rmse, num_errors, num_patterns);

            if (num_errors == 0)
                break;
        }

        // run training algorithm
        for (i=0; i<num_patterns; i++)
            maxnet_train(q,&x[i*2],class[i]);
    }

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(e,'-','LineWidth',2);\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('RMS error');\n");

    fprintf(fid,"num_classes = %u;\n", num_classes);
    for (i=0; i<num_patterns; i++) {
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;  class(%3u) = %3u;\n", i+1, x[2*i+0], x[2*i+1], i+1, class[i]);
    }
    for (i=0; i<num_classes; i++) {
        fprintf(fid,"i_%u = find(class==%u);\n", i, i);
    }

    // compute discriminating regions
    printf("computing discriminating regions...\n");
    unsigned int nx = 501;
    float xmin = -2.0f;
    float xmax =  2.0f;
    float dx = (xmax - xmin) / (float)(nx-1);
    unsigned int j;
    unsigned int c_test0;
    n=0;
    for (i=0; i<nx; i++) {
        x_test[0] = xmin + dx*i;
        x_test[1] = xmin;
        maxnet_evaluate(q,x_test,y_test,&c_test0);

        for (j=0; j<nx; j++) {
            x_test[1] = xmin + dx*j;
            maxnet_evaluate(q,x_test,y_test,&c_test);

            if (c_test != c_test0) {
                c_test0 = c_test;
                fprintf(fid,"d(%3u) = %12.8f + j*%12.8f;\n", n+1, x_test[0], x_test[1]);
                n++;
            }
        }
        if (n > 10000) {
            fprintf(stderr,"warning: %s, number of discrimination points too large (bailing)\n", argv[0]);
            break;
        }
    }
    printf("done.\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    for (i=0; i<num_classes; i++)
        fprintf(fid,"plot(real(x(i_%u)), imag(x(i_%u)), 's', 'Color', rand(1,3));\n", i, i);
    if (n>0)
        fprintf(fid,"plot(d,'sk','MarkerSize',2);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('x_0');\n");
    fprintf(fid,"ylabel('x_1');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"axis square;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
 
    maxnet_destroy(q);

    printf("done.\n");
    return 0;
}


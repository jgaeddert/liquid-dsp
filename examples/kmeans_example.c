//
// K-means example
//

#include <stdio.h>
#include <math.h>
#include "liquid.experimental.h"

#define OUTPUT_FILENAME "kmeans_example.m"

int main() {
    // options
    unsigned int m = 5;     // number of true centroids
    unsigned int k = m;     // number of estimated centroids
    unsigned int n = 16;    // number of samples per centroid

    // derived values
    unsigned int num_samples = n*m; // total number of samples
    float complex x[num_samples];   // complex samples array
    float complex s[m];             // true centroid array
    float complex s_hat[m];         // estimated centroid array

    unsigned int i;

    // initialize true centroids (randomly)
    for (i=0; i<m; i++)
        crandnf(&s[i]);

    unsigned int j;
    for (i=0; i<m; i++) {
        for (j=0; j<n; j++) {
            x[i*n + j] = s[i] + (randnf() + _Complex_I*randnf())/sqrtf(m);
        }
    }

    // initialize estimated centroids (randomly)
    for (i=0; i<m; i++)
        s_hat[i] = (randnf() + _Complex_I*randnf())/k;

    // run k-means algorithm
    kmeans(x,num_samples,s_hat,k);

    for (i=0; i<k; i++)
        printf("k[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(s_hat[i]), cimagf(s_hat[i]));

    // find associated samples
    unsigned int index[num_samples];
    unsigned int imin=0;
    float d, dmin=0;
    for (i=0; i<num_samples; i++) {
        
        for (j=0; j<k; j++) {
            d = cabsf(x[i] - s_hat[j]);
            if (j==0 || d<dmin) {
                dmin = d;
                imin = j;
            }
        }

        // save index of associated centroid
        index[i] = imin;
    }

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %3u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(x[i]),cimagf(x[i]));
    }
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"kmin(%3u) = %3u;\n", i+1, index[i]+1);
    }

    for (i=0; i<m; i++)
        fprintf(fid,"s(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(s[i]),cimagf(s[i]));
    for (i=0; i<k; i++)
        fprintf(fid,"s_hat(%3u) = %12.8f + j*%12.8f;\n",i+1,crealf(s_hat[i]),cimagf(s_hat[i]));

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(x,'x','Color',[0.7 0.7 0.7]);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"for i=1:n,\n");
    fprintf(fid,"    plot([x(i) s_hat(kmin(i))],'-','Color',[0.7 0.7 0.7]);\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"plot(s,'or');\n");
    fprintf(fid,"plot(s_hat,'sg');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('y');\n");
    fprintf(fid,"axis square;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


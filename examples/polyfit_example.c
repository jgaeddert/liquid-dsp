// 
// polyfit_example.c
//
// Test polynomial fit to sample data.
//

#include <stdio.h>

#include "liquid.h"

#define OUTPUT_FILENAME "polyfit_example.m"

int main() {

    unsigned int n=15;      // number of samples
    unsigned int order=2;   // polynomial order

    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");

    // initialize data vectors
    float x[n];
    float y[n];
    unsigned int i;
    for (i=0; i<n; i++) {
        float v = (float)(i) - (float)(n)/2 + 0.5f;
        x[i] = v;
        y[i] = v*v + randnf();

        printf("x : %12.8f, y : %12.8f\n", x[i], y[i]);
        fprintf(fid,"x(%3u) = %12.4e; y(%3u) = %12.4e;\n", i+1, x[i], i+1, y[i]);
    }

    // compute coefficients
    unsigned int k=order+1;
    float p[k];
    polyf_fit(x,y,n,p,k);

    for (i=0; i<k; i++) {
        printf("p[%3u] = %12.4e\n", i, p[i]);
        // print polynomial coefficients vector in reverse order
        fprintf(fid,"p(%3u) = %12.4e;\n", i+1, p[k-i-1]);
    }

    // evaluate polynomial
    float xmin = -7.5f;
    float xmax =  7.5f;
    unsigned int num_steps = 64;
    float dx = (xmax-xmin)/(num_steps-1);
    float xtest = xmin;
    float ytest;
    for (i=0; i<num_steps; i++) {
        ytest = polyf_val(p,k,xtest);
        fprintf(fid,"xtest(%3u) = %12.4e; ytest(%3u) = %12.4e;\n", i+1, xtest, i+1, ytest);
        xtest += dx;
    }
    
    // plot results
    fprintf(fid,"plot(x,y,'x',xtest,ytest,'-');\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('y, p^{(%u)}(x)');\n", order);
    fprintf(fid,"legend('data','poly-fit',0);\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


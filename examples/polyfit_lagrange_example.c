// 
// polyfit_lagrange_example.c
//

#include <stdio.h>

#include "liquid.h"

#define OUTPUT_FILENAME "polyfit_lagrange_example.m"

int main() {
    unsigned int n=5;      // number of samples

    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");

    // initialize data vectors
    float x[n];
    float y[n];
    unsigned int i;
    for (i=0; i<n; i++) {
        float v = (float)(i) - (float)(n)/2 + 0.5f;
        v = (float)i / (float)(n-1);
        x[i] = v;
        y[i] = randnf();

        printf("x : %12.8f, y : %12.8f\n", x[i], y[i]);
        fprintf(fid,"x(%3u) = %12.4e; y(%3u) = %12.4e;\n", i+1, x[i], i+1, y[i]);
    }

    // compute coefficients
    float p[n];
    fpolyfit_lagrange(x,y,n,p);

    for (i=0; i<n; i++) {
        printf("p[%3u] = %12.4e\n", i, p[i]);
        // print polynomial coefficients vector in reverse order
        fprintf(fid,"p(%3u) = %12.4e;\n", i+1, p[n-i-1]);
    }

    // test interpolation method
    float x0 = 0.23f, y0;
    float x1 = 0.77f, y1;
    fpoly_interp_lagrange(x,y,n,x0,&y0);
    fpoly_interp_lagrange(x,y,n,x1,&y1);
    fprintf(fid,"x0 = %12.4e; y0 = %12.4e;\n", x0, y0);
    fprintf(fid,"x1 = %12.4e; y1 = %12.4e;\n", x1, y1);
    
    // evaluate polynomial
    float xdel = n < 3 ? 0.1f : 0.02f /(float)n;
    float xmin = 0.0f - xdel;
    float xmax = 1.0f + xdel;
    unsigned int num_steps = 64;
    float dx = (xmax-xmin)/(num_steps-1);
    float xtest = xmin;
    float ytest;
    for (i=0; i<num_steps; i++) {
        ytest = fpolyval(p,n,xtest);
        fprintf(fid,"xtest(%3u) = %12.4e; ytest(%3u) = %12.4e;\n", i+1, xtest, i+1, ytest);
        xtest += dx;
    }

    // plot results
    fprintf(fid,"plot(x,y,'s',xtest,ytest,'-',[x0 x1],[y0 y1],'rx');\n");
    fprintf(fid,"xlabel('x');\n");
    fprintf(fid,"ylabel('y, p^{(%u)}(x)');\n", n);
    fprintf(fid,"legend('data','poly-fit','interp',0);\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


// 
// Test polynomial fit
//

#include <stdio.h>
#include <string.h>
//#include <math.h>
#include "liquid.h"

#define OUTPUT_FILENAME "polyfit_example.m"

int main() {

    unsigned int n=15;      // number of samples
    unsigned int order=3;   // polynomial order
    float x[n];
    float y[n];

    FILE * fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");

    unsigned int i;
    for (i=0; i<n; i++) {
        float v = (float)(i) - (float)(n)/2 + 0.5f;
        x[i] = v;
        y[i] = v*v + 0.1*v*v*v + randnf();

        printf("x : %12.8f, y : %12.8f\n", x[i], y[i]);
        fprintf(fid,"x(%3u) = %12.4e; y(%3u) = %12.4e;\n", i+1, x[i], i+1, y[i]);
    }

    // derived values
    unsigned int k=order+1;

    // ...
    float X[n*k];
    unsigned int r,c;
    float v;
    for (r=0; r<n; r++) {
        v = 1;
        for (c=0; c<k; c++) {
            matrix_access(X,n,k,r,c) = v;
            v *= x[r];
        }
    }

    // compute transpose of X
    float Xt[k*n];
    memmove(Xt,X,k*n*sizeof(float));
    fmatrix_trans(Xt,n,k);

    printf("\nX : \n");
    fmatrix_print(X,n,k);

    printf("\n[X'] : \n");
    fmatrix_print(Xt,k,n);

    // compute [X']*y
    float Xty[k];
    fmatrix_mul(Xt,k,n,y,n,1,Xty,k,1);
    printf("\n[X']*y : \n");
    fmatrix_print(Xty,k,1);

    // compute [X']*X
    float X2[k*k];
    fmatrix_mul(Xt,k,n,X,n,k,X2,k,k);
    printf("\n[X']*X : \n");
    fmatrix_print(X2,k,k);

    // compute inv([X']*X)
    float G[k*k];
    memmove(G,X2,k*k*sizeof(float));
    fmatrix_inv(G,k,k);
    fmatrix_print(G,k,k);

    // compute coefficients
    float a[k];
    fmatrix_mul(G,k,k,Xty,k,1,a,k,1);

    for (i=0; i<k; i++) {
        printf("a[%3u] = %12.8f\n", i, a[i]);
        fprintf(fid,"p(%3u) = %12.4e;\n", i+1, a[k-i-1]);
    }
    
    fprintf(fid,"xmin = min(x);\n");
    fprintf(fid,"xmax = max(x);\n");
    fprintf(fid,"dx   = (xmax-xmin)/99;\n");
    fprintf(fid,"xt   = xmin:dx:xmax;\n");
    fprintf(fid,"yt   = polyval(p,xt);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(x,y,'x', xt,yt,'-');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


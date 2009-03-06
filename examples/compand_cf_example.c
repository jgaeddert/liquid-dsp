//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define DEBUG_FILENAME "compand_cf_example.m"

int main() {
    // options
    float mu=255.0f;

    // open debug file
    FILE * fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    float dr = 0.1f;    // radius step size
    float dtheta=0.1f;  // phase step size
    unsigned int n=0;
    float complex x, y, z;
    float r=0.0f, theta;
    while (r < 1.0f) {
        theta = 0.0f;

        while (theta < 2*M_PI) {
            x = r*cosf(theta) + _Complex_I*r*sinf(theta);
            compress_cf_mulaw(x,mu,&y);
            expand_cf_mulaw(y,mu,&z);
            //printf("%8.4f > %8.4f > %8.4f\n", x, y, z);

            fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(x), cimagf(x));
            fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(y), cimagf(y));
            fprintf(fid,"z(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(z), cimagf(z));

            n++;
            theta += dtheta;
        }

        r += dr;
    }

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot3(real(x),imag(x),real(y),'bx',real(x),imag(x),imag(y),'rx');\n");
    fprintf(fid,"xlabel('x: real');\n");
    fprintf(fid,"ylabel('x: imag');\n");
    fprintf(fid,"legend('y: real','y: imag',1);\n");
    fprintf(fid,"box off;\n");
    fprintf(fid,"view(3);\n");
    fprintf(fid,"axis([-1 1 -1 1 -1 1]);\n");

    // close debug file
    fclose(fid);
    printf("results wrtten to %s\n", DEBUG_FILENAME);
    printf("done.\n");
    return 0;
}


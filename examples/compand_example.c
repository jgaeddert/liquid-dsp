//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.h"

#define DEBUG_FILENAME "compand_example.m"

int main() {
    // options
    unsigned int n=100;
    float mu=255.0f;

    // open debug file
    FILE * fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    float x=-1.0f;
    float y, z;
    float dx = 2/(float)(n+1);
    unsigned int i;
    for (i=0; i<n; i++) {
        y = compress_mulaw(x,mu);
        z = expand_mulaw(y,mu);
        //printf("%8.4f > %8.4f > %8.4f\n", x, y, z);

        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x);
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y);
        fprintf(fid,"z(%3u) = %12.4e;\n", i+1, z);

        x += dx;
    }

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(x,y,'-b',x,z,'-r');\n");
    fprintf(fid,"axis square\n");

    // close debug file
    fclose(fid);
    printf("results wrtten to %s\n", DEBUG_FILENAME);
    printf("done.\n");
    return 0;
}


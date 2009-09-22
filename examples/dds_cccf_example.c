//
// Arbitrary resampler
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "dds_cccf_example.m"

int main() {
    // options
    float fc0=0.0f;             // input frequency
    float fc1=0.0f;             // output frequency
    float r=8.0f;               // resampling rate (output/input)
    //unsigned int n=128;         // number of input samples

    // create resampler
    dds_cccf q = dds_cccf_create(fc0,fc1,r);
    dds_cccf_print(q);

    float complex y[256];
    unsigned int nw;
    dds_cccf_execute(q, 0, y, &nw);

    // open/initialize output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"r=%12.8f;\n", r);


    // output results
    fprintf(fid,"\n\n");

    fclose(fid);
    printf("results written to %s\n",OUTPUT_FILENAME);

    // clean up allocated objects
    dds_cccf_destroy(q);

    printf("done.\n");
    return 0;
}

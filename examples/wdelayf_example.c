const char __docstr__[] = "Demonstration of wdelayf object";

#include <stdio.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "wdelayf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, delay,       10, 'm', "delay [samples]", NULL);
    liquid_argparse_add(unsigned, num_samples, 64, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // create wdelay, all elements initialized to 0
    wdelayf w = wdelayf_create(delay);
    float y; // output

    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"delay = %u;\n", delay);

    // push several elements
    unsigned int i;
    float x;
    for (i=0; i<num_samples; i++) {
        if (i==0)   x = 1.0f;
        else        x = 0.0f;

        wdelayf_push(w, x);
        wdelayf_read(w, &y);

        printf("%4u : %12.8f\n", i, y);
        fprintf(fid,"x(%4u) = %12.8f; y(%4u) = %12.8f;\n", i+1, x, i+1, y);
    }
    wdelayf_print(w);

    fprintf(fid,"\n\n");
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,t,y);\n");
    fclose(fid);
    printf("results written to %s.\n", filename);

    // clean it up
    wdelayf_destroy(w);

    printf("done.\n");
    return 0;
}



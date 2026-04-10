const char __docstr__[] = "Demonstration of filter based on order statistics";

#include <stdio.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "ordfilt_rrrf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_samples, 2400, 'N', "number of samples to generate", NULL);
    liquid_argparse_add(unsigned, n,            101, 'n', "filter length", NULL);
    liquid_argparse_add(unsigned, k,              5, 'k', "order statistic", NULL);
    liquid_argparse_parse(argc,argv);

    // arrays
    LIQUID_VLA(float, x, num_samples);   // filter input
    LIQUID_VLA(float, y, num_samples);   // filter output

    // generate input tone with offset noise
    unsigned int i;
    for (i=0; i<num_samples; i++)
        x[i] = -cosf(2*M_PI*(float)i/(float)num_samples) + fabsf(randnf());

    // create object
    ordfilt_rrrf q = ordfilt_rrrf_create(n,k);
    ordfilt_rrrf_print(q);

    // apply filter
    ordfilt_rrrf_execute_block(q,x,num_samples,y);

    // destroy filter object
    ordfilt_rrrf_destroy(q);

    // export results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% ordfilt_rrrf_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    
    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x[i], i+1, y[i]);

    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,'Color',[0.3 0.3 0.3],...\n");
    fprintf(fid,"     t,y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('signals');\n");
    fprintf(fid,"axis([0 num_samples -4 4]);\n");
    fprintf(fid,"legend('noise','filtered noise');");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}


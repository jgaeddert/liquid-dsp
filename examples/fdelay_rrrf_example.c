// Demonstrate fdelay object to add arbitrary fractional delays.
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include "liquid.h"
#define OUTPUT_FILENAME "fdelay_rrrf_example.m"

int main() {
    // options
    unsigned int nmax       = 200;  // maximum delay
    unsigned int m          =  12;  // filter semi-length
    unsigned int npfb       =  10;  // fractional delay resolution
    unsigned int num_samples= 400;  // number of samples to run
    float        delay      = 7.7;  // requested delay

    // design filter from prototype and scale to bandwidth
    fdelay_rrrf q = fdelay_rrrf_create(nmax, m, npfb);
    fdelay_rrrf_set_delay(q, delay);
    fdelay_rrrf_print(q);

    // generate impulse and propagate through object
    float x[num_samples];
    float y[num_samples];
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // generate input
        x[i] = i==0 ? 1.0f : 0.0f;

        // run filter
        fdelay_rrrf_push(q, x[i]);
        fdelay_rrrf_execute(q, &y[i]);
    }

    // measure delay
    int   imax;
    float ymax=0;
    for (i=0; i<num_samples; i++) {
        if (i==0 || fabsf(y[i]) > ymax) {
            imax = i;
            ymax = fabsf(y[i]);
        }
    }
    // TODO: interpolate
    // assert imax > 0 && imax < nmax-1
    float yneg=y[imax-1], y0=y[imax], ypos=y[imax+1];
    float a     =  0.5f*(ypos + yneg) - y0;
    float b     =  0.5f*(ypos - yneg);
    float c     =  y0;
    float tau   = -b / (2.0f*a); //-0.5f*(ypos - yneg) / (ypos + yneg - 2*y0);
    printf("imax=%d, ymax=%f, delay=%f\n", imax-m, ymax, imax-m+tau);

    // destroy filter object
    fdelay_rrrf_destroy(q);

    // plot results to output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"delay=%f; m=%u; n=%u;\n",delay,m,num_samples);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e;\n", i+1, x[i]);
        fprintf(fid,"y(%4u) = %12.4e;\n", i+1, y[i]);
    }
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"imax=%u; a=%f; b=%f; c=%f; tau=%f;\n",imax,a,b,c,tau);
    fprintf(fid,"dt=[-1.2:0.01:1.2]; yp=polyval([a,b,c],dt);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,x,'-x','Color',[0 0.3 0.5],'LineWidth',1);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('input');\n");
    fprintf(fid,"  xlim([-10 10]);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t-m,y,'-x','Color',[0 0.3 0.5],'LineWidth',1,...\n");
    fprintf(fid,"       dt+imax-m,yp,'-r',...\n");
    fprintf(fid,"       tau+imax-m,polyval([a,b,c],tau),'or',...\n");
    fprintf(fid,"       [delay delay],[-0.2 1.2],'-r');\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('output');\n");
    fprintf(fid,"  xlim([-3 3]+delay);\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);
    return 0;
}


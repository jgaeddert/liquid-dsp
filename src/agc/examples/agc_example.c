//
// Automatic gain control test
//

#include <stdio.h>
#include "liquid.h"

#define DEBUG // writes results to agc_example.m

int main() {
    // options
    float etarget=500.0f;   // target energy
    float bt=0.01f;         // loop bandwidth
    unsigned int n=100;     // number of iterations
    unsigned int d=5;       // print every d iterations

    // create objects
    agc p = agc_create(etarget, bt);

    // print info
    printf("automatic gain control // target: %5.1f, loop bandwidth: %4.2e\n",etarget,bt);

#ifdef DEBUG
    FILE* fid = fopen("agc_example.m","w");
    fprintf(fid,"%% agc_example.m, auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
#endif

    unsigned int i;
    float complex x = 1.2f + _Complex_I*3.9f, y=x;
    for (i=0; i<n; i++) {
#ifdef DEBUG
        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, cabsf(y));
#endif
        if ( ((i+1)%d)==0 || i+1==n )
            printf("%3u: %8.3f\n", i+1, cabsf(y));

        agc_execute(p, x, &y);
    }

#ifdef DEBUG
    fprintf(fid,"\n\n%% print results\n");
    fprintf(fid,"n=%u;\nt=0:(n-1);\n",n);
    fprintf(fid,"e=%12.4e;\n",etarget);
    fprintf(fid,"figure;\nsemilogy([0 n],[e e],'-r',t,x,'-b','LineWidth',2);\n");
    fprintf(fid,"xlabel('time');\nylabel('output amplitude');\n");
    fprintf(fid,"title(['Automatic gain control, target=%5.1f, bt=%4.2e']);\n", etarget, bt);
    fprintf(fid,"grid on;\n");
    fprintf(fid,"\n\n");

    fclose(fid);
    printf("results written to agc_example.m\n");
#endif

    agc_destroy(p);

    printf("done.\n");
    return 0;
}


//
// optim_gradsearch.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.doc.h"

#define OUTPUT_FILENAME_DAT "figures.gen/optim_gradsearch.dat"
#define OUTPUT_FILENAME_GNU "figures.gen/optim_gradsearch.gnu"
#define OUTPUT_FILENAME_UTILITY_GNU "figures.gen/optim_gradsearch_utility.gnu"

int main() {
    unsigned int num_iterations = 2000; // number of iterations to run

    float optimum_vect[2];
    unsigned int i;
    optimum_vect[0] = -0.1f;
    optimum_vect[1] =  1.4f;

    //
    float v[num_iterations][2];
    float u[num_iterations];

    float optimum_utility;

    // create gradsearch object
    gradsearch gs = gradsearch_create(
        NULL, optimum_vect, 2, &rosenbrock, LIQUID_OPTIM_MINIMIZE, NULL);

    // execute search
    //optimum_utility = gradsearch_run(gs, num_iterations, -1e-6f);

    // execute search one iteration at a time
    for (i=0; i<num_iterations; i++) {
        optimum_utility = rosenbrock(NULL,optimum_vect,2);

        // save result
        v[i][0] = optimum_vect[0];
        v[i][1] = optimum_vect[1];
        u[i]    = optimum_utility;

        gradsearch_step(gs);

        if (((i+1)%100)==0)
            gradsearch_print(gs);
    }

    gradsearch_destroy(gs);

    // 
    // export output file(s)
    //
    FILE * fid;
    
    // 
    // export gradient search results
    //
    fid = fopen(OUTPUT_FILENAME_DAT,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_DAT);

    fprintf(fid,"# %12s %12s %12s %12s\n", "index", "x", "y", "utility");
    for (i=0; i<num_iterations; i++)
        fprintf(fid,"  %12u %12.8f %12.8f %12.4e\n", i, v[i][0], v[i][1], u[i]);

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME_DAT);



    // 
    // export surface figure
    //
    fid = fopen(OUTPUT_FILENAME_GNU,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_GNU);

    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set logscale z 10\n");
    fprintf(fid,"set view 40, 340, 1, 1\n");
    fprintf(fid,"set isosamples 60, 60\n");
    fprintf(fid,"set hidden3d offset 1 trianglepattern 3 undefined 1 altdiagonal bentover # hide background overlap\n");
    fprintf(fid,"set style data lines\n");
    fprintf(fid,"set ticslevel 0\n");

    fprintf(fid,"set xrange [ * : * ] noreverse nowriteback\n");
    fprintf(fid,"set yrange [ * : * ] noreverse nowriteback\n");
    fprintf(fid,"set zrange [ * : * ] noreverse nowriteback\n");
    fprintf(fid,"set size ratio 0.6\n");
    fprintf(fid,"set xlabel 'x'\n");
    fprintf(fid,"set ylabel 'y'\n");
    //fprintf(fid,"set zlabel 'rosenbrock'\n");
    fprintf(fid,"set format z '10^{%%L}'\n");
    fprintf(fid,"set nokey\n");
    fprintf(fid,"set contour base\n");
    fprintf(fid,"unset clabel\n");
    fprintf(fid,"set cntrparam levels 10\n");
    fprintf(fid,"splot [-1.5:1.5] [-0.5:1.5] (1-x)**2 + 100*(y - x**2)**2 with lines linecolor rgb '#cccccc',\\\n");
    fprintf(fid,"      '%s' using 2:3:(($4+1e-3)*1.1) with lines linewidth 2 linecolor rgb '#004080'\n", OUTPUT_FILENAME_DAT);
    //fprintf(fid,"      '%s' using 2:3:(1e-3) with lines linewidth 2 linecolor rgb '#004080'\n", OUTPUT_FILENAME_DAT);

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME_GNU);


    // 
    // export utility figure
    //
    fid = fopen(OUTPUT_FILENAME_UTILITY_GNU,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME_UTILITY_GNU);

    fprintf(fid,"reset\n");
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    fprintf(fid,"set logscale y 10\n");
    fprintf(fid,"set format y '10^{%%L}'\n");
    fprintf(fid,"set xrange [0:%u]\n", num_iterations);
    //fprintf(fid,"set yrange [0:1]\n");
    fprintf(fid,"set size ratio 0.4\n");
    fprintf(fid,"set xlabel 'Iteration'\n");
    fprintf(fid,"set ylabel 'Rosenbrock Utility'\n");
    fprintf(fid,"set nokey\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '%s' lw 1\n", LIQUID_DOC_COLOR_GRID);
    fprintf(fid,"plot '%s' using 1:4 with lines linewidth 2 linecolor rgb '#004080'\n", OUTPUT_FILENAME_DAT);

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME_UTILITY_GNU);

    return 0;
}

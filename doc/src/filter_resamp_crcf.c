//
// Arbitrary resampler
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include <liquid/liquid.h>

#define OUTPUT_FILENAME "figures.gen/filter_resamp_crcf.gnu"

int main() {
    // options
    unsigned int h_len = 13;    // filter semi-length (filter delay)
    float r=0.9f;               // resampling rate (output/input)
    float bw=0.5f;              // resampling filter bandwidth
    float slsl=-60.0f;          // resampling filter sidelobe suppression level
    unsigned int npfb=32;       // number of filters in bank (timing resolution)
    unsigned int n=128;         // number of input samples

    // generate input sequence : windowed sum of complex sinusoids
    unsigned int i;
    float complex x[n];
    for (i=0; i<n; i++) {
        float complex jphi = _Complex_I*2.0f*M_PI*i;
        x[i] = cexpf(jphi*0.04f) + 1.4f*cexpf(jphi*0.07f);
        x[i] *= blackmanharris(i,n);
    }

    // output buffer with extra padding for good measure
    unsigned int y_len = (unsigned int) ceilf(1.1*r*n) + 16;
    float complex y[y_len];

    // create resampler
    resamp_crcf f = resamp_crcf_create(r,h_len,bw,slsl,npfb);
    unsigned int num_written;
    unsigned int k=0;
    for (i=0; i<n; i++) {
        // execute resampler, storing in output buffer
        resamp_crcf_execute(f, x[i], &y[k], &num_written);
        k += num_written;
    }
    // clean up allocated objects
    resamp_crcf_destroy(f);

    // open/initialize output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"# %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"reset\n");
    // TODO : switch terminal types here
    fprintf(fid,"set terminal postscript eps enhanced color solid rounded\n");
    //fprintf(fid,"set xrange [0:%u];\n",n);
    //fprintf(fid,"set yrange [-1:1]\n");
    fprintf(fid,"set size ratio 0.3\n");
    fprintf(fid,"set xlabel 'Sample Index'\n");
    fprintf(fid,"set nokey # disable legned\n");
    fprintf(fid,"set grid xtics ytics\n");
    fprintf(fid,"set pointsize 0.6\n");
    fprintf(fid,"set grid linetype 1 linecolor rgb '#999999' lw 1\n");
    fprintf(fid,"set multiplot layout 2,1 scale 1.0,1.0\n");

    fprintf(fid,"# real\n");
    fprintf(fid,"set ylabel 'Real'\n");
    fprintf(fid,"plot '-' using 1:2 with linespoints pointtype 7 linetype 1 linewidth 1 linecolor rgb '#999999',\\\n");
    fprintf(fid,"     '-' using 1:2 with points pointtype 7 linecolor rgb '#008000'\n");
    // export output
    for (i=0; i<n; i++) {
        //fprintf(fid,"%6u %12.4e %12.4e\n", i, cos(2*M_PI*0.04*i), sin(2*M_PI*0.04*i));
        fprintf(fid,"%6u %12.4e %12.4e\n", i, crealf(x[i]), cimagf(x[i]));
    }
    fprintf(fid,"e\n");

    float t;
    for (i=0; i<k; i++) {
        t = (float)(i) / r - (float)(h_len);
        fprintf(fid,"%12.4e %12.4e %12.4e\n", t, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"e\n");

    fprintf(fid,"# imag\n");
    fprintf(fid,"set ylabel 'Imag'\n");
    fprintf(fid,"plot '-' using 1:3 with linespoints pointtype 7 linetype 1 linewidth 1 linecolor rgb '#999999',\\\n");
    fprintf(fid,"     '-' using 1:3 with points pointtype 7 linecolor rgb '#800000'\n");
    // export output
    for (i=0; i<n; i++) {
        //fprintf(fid,"%6u %12.4e %12.4e\n", i, cos(2*M_PI*0.04*i), sin(2*M_PI*0.04*i));
        fprintf(fid,"%6u %12.4e %12.4e\n", i, crealf(x[i]), cimagf(x[i]));
    }
    fprintf(fid,"e\n");

    for (i=0; i<k; i++) {
        t = (float)(i) / r - (float)(h_len);
        fprintf(fid,"%12.4e %12.4e %12.4e\n", t, crealf(y[i]), cimagf(y[i]));
    }
    fprintf(fid,"e\n");
    fprintf(fid,"unset multiplot\n");

    // close output file
    fclose(fid);

    printf("done.\n");
    return 0;
}

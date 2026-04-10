const char __docstr__[] = "Fading generator example";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "fading_generator_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, h_len, 51,    'p', "doppler filter length", NULL);
    liquid_argparse_add(float,    fd,    0.05f, 'f', "maximum doppler frequency", NULL);
    liquid_argparse_add(float,    K,     2.0f,  'K', "Rice fading factor", NULL);
    liquid_argparse_add(float,    omega, 1.0f,  'O', "mean power", NULL);
    liquid_argparse_add(float,    theta, 0.0f,  't', "angle of arrival", NULL);
    liquid_argparse_add(unsigned, n,     256,   'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // generate filter
    unsigned int i;
    LIQUID_VLA(float, h, h_len);
    liquid_firdes_doppler(h_len,fd,K,theta,h);
    firfilt_rrrf fi = firfilt_rrrf_create(h,h_len);
    firfilt_rrrf fq = firfilt_rrrf_create(h,h_len);

    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n",filename);
    fprintf(fid,"clear all;\nclose all;\n\n");

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%3u) = %12.8e;\n", i+1, h[i]);

    // generate complex fading envelope
    liquid_float_complex x, y;
    float yi, yq;
    float s = sqrtf((omega*K)/(K+1));
    float sig = sqrtf(0.5f*omega/(K+1));
    float t=0.0f;
    for (i=0; i<h_len; i++) {
        crandnf(&x);

        firfilt_rrrf_push(fi, crealf(x));
        firfilt_rrrf_push(fq, cimagf(x));

        t += h[i]*h[i];
    }
    t = sqrtf(t);

    for (i=0; i<n; i++) {
        crandnf(&x);

        firfilt_rrrf_push(fi, crealf(x));
        firfilt_rrrf_push(fq, cimagf(x));

        // compute outputs
        firfilt_rrrf_execute(fi, &yi);
        firfilt_rrrf_execute(fq, &yq);

        yi /= t;
        yq /= t;

        y = (yi*sig + s) + _Complex_I*(yq*sig);

        printf("%4u: r:%8.5f, angle:%5.2f\n", i, cabsf(y), cargf(y));

        fprintf(fid, "y(%4u) = %12.8e + j*%12.8e;\n",i+1,crealf(y),cimagf(y));
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-','LineWidth',2);\n");
    fprintf(fid,"xlabel('Normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"\n\n");

    fprintf(fid,"t=0:length(y)-1;\n");
    fprintf(fid,"R=20*log10(abs(y));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,R,'-','LineWidth',2);\n");
    fprintf(fid,"xlabel('time [samples]');\n");
    fprintf(fid,"ylabel('fading amplitude [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);

    printf("results written to %s\n", filename);

    // clean up objects
    firfilt_rrrf_destroy(fi);
    firfilt_rrrf_destroy(fq);

    printf("done.\n");
    return 0;
}


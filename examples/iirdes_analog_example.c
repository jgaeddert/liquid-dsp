const char __docstr__[] =
"Tests infinite impulse response (IIR) analog filter design."
" While this example seems purely academic as IIR filters"
" used in liquid are all digital, it is important to realize"
" that they are all derived from their analog counterparts."
" This example serves to check the response of the analog"
" filters to ensure they are correct.  The results of design"
" are written to a file.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "iirdes_analog_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,     type, "butter", 't', "filter type: [butter], cheby1, cheby2, ellip, bessel", NULL);
    liquid_argparse_add(unsigned, order,        3, 'n', "filter order", NULL);
    liquid_argparse_add(float,       Ap,     1.0f, 'p', "passband ripple in dB (cheby1, ellip)", NULL);
    liquid_argparse_add(float,       As,    60.0f, 's', "stopband attenuation in dB (cheby2, ellip)", NULL);
    liquid_argparse_add(float,       wc,     1.0f, 'w', "angular passband cut-off frequency", NULL);
    liquid_argparse_parse(argc,argv);

    // filter type
    liquid_iirdes_filtertype ftype = LIQUID_IIRDES_BUTTER;
    if (strcmp(type,"butter")==0)
        ftype = LIQUID_IIRDES_BUTTER;
    else if (strcmp(type,"cheby1")==0)
        ftype = LIQUID_IIRDES_CHEBY1;
    else if (strcmp(type,"cheby2")==0)
        ftype = LIQUID_IIRDES_CHEBY2;
    else if (strcmp(type,"ellip")==0)
        ftype = LIQUID_IIRDES_ELLIP;
    else if (strcmp(type,"bessel")==0)
        ftype = LIQUID_IIRDES_BESSEL;
    else
        return liquid_error(LIQUID_EICONFIG,"unknown filter type '%s'", type);

    // validate input
    if (wc <= 0)
        return liquid_error(LIQUID_EICONFIG,"cutoff frequency out of range");
    if (Ap <= 0)
        return liquid_error(LIQUID_EICONFIG,"pass-band ripple out of range");
    if (As <= 0)
        return liquid_error(LIQUID_EICONFIG,"stop-band ripple out of range");

    // number of analog poles/zeros
    unsigned int npa = order;
    unsigned int nza = 0;

    // complex analog zeros, poles, gain
    LIQUID_VLA(liquid_float_complex, za, order);
    LIQUID_VLA(liquid_float_complex, pa, order);
    liquid_float_complex ka;

    unsigned int i;
    
    unsigned int r = order % 2;
    unsigned int L = (order-r)/2;

    float Gp, Gs;
    float ep = sqrtf( powf(10.0f, Ap / 10.0f) - 1.0f );
    float es = powf(10.0f, -As / 20.0f);

    switch (ftype) {
    case LIQUID_IIRDES_BUTTER:
        printf("Butterworth filter design:\n");
        nza = 0;
        butter_azpkf(order,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY1:
        printf("Cheby-I filter design:\n");
        nza = 0;
        cheby1_azpkf(order,ep,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY2: {
        printf("Cheby-II filter design:\n");
        nza = 2*L;
        float epsilon = powf(10.0f, -As/20.0f);
        cheby2_azpkf(order,epsilon,za,pa,&ka);
        break;
    }
    case LIQUID_IIRDES_ELLIP:
        printf("elliptic filter design:\n");
        nza = 2*L;
        Gp = powf(10.0f, -Ap / 20.0f);
        Gs = powf(10.0f, -As / 20.0f);
        printf("  Gp = %12.8f\n", Gp);
        printf("  Gs = %12.8f\n", Gs);

        // epsilon values
        ep = sqrtf(1.0f/(Gp*Gp) - 1.0f);
        es = sqrtf(1.0f/(Gs*Gs) - 1.0f);

        ellip_azpkf(order,ep,es,za,pa,&ka);
        break;
    case LIQUID_IIRDES_BESSEL:
        printf("Bessel filter design:\n");
        bessel_azpkf(order,za,pa,&ka);
        nza = 0;
        break;
    default:
        fprintf(stderr,"error: %s: unknown filter type\n", argv[0]);
        exit(1);
    }

    // transform zeros, poles, gain
    for (i=0; i<npa; i++) {
        pa[i] *= wc;
        ka *= wc;
    }
    for (i=0; i<nza; i++) {
        za[i] *= wc;
        ka /= wc;
    }

    // print zeros, poles, gain to screen
    for (i=0; i<nza; i++)
        printf("z(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(za[i]), cimagf(za[i]));
    printf("\n");
    for (i=0; i<npa; i++)
        printf("p(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(pa[i]), cimagf(pa[i]));
    printf("\n");
    printf("ka = %12.4e + j*%12.4e;\n", crealf(ka), cimagf(ka));

    // open output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"wc = %12.8f;\n", wc);
    fprintf(fid,"npa = %u;\n", npa);
    fprintf(fid,"nza = %u;\n", nza);
    fprintf(fid,"pa = zeros(1,npa);\n");
    fprintf(fid,"za = zeros(1,nza);\n");
    for (i=0; i<npa; i++)
        fprintf(fid,"pa(%3u) = %16.8e + j*%16.8e;\n", i+1, crealf(pa[i]), cimagf(pa[i]));
    for (i=0; i<nza; i++)
        fprintf(fid,"za(%3u) = %16.8e + j*%16.8e;\n", i+1, crealf(za[i]), cimagf(za[i]));

    fprintf(fid,"k = %16.8e;\n", crealf(ka));

    fprintf(fid,"b = 1;\n");
    fprintf(fid,"for i=1:nza,\n");
    fprintf(fid,"  b = conv(b,[1 za(i)]);\n");
    fprintf(fid,"end;\n");

    fprintf(fid,"a = 1;\n");
    fprintf(fid,"for i=1:npa,\n");
    fprintf(fid,"  a = conv(a,[1 pa(i)]);\n");
    fprintf(fid,"end;\n");

    fprintf(fid,"b = real(b)*k;\n");
    fprintf(fid,"a = real(a);\n");

    fprintf(fid,"w = 10.^[-2:0.002:2]*wc;\n");
    fprintf(fid,"s = j*w;\n");
    fprintf(fid,"h = polyval(b,s) ./ polyval(a,s);\n");
    fprintf(fid,"H = 20*log10(abs(h));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = [0:0.02:1]*2*pi;\n");
    fprintf(fid,"plot(wc*cos(t),wc*sin(t),'-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"     real(pa),imag(pa),'x');\n");
    fprintf(fid,"if nza > 0,\n");
    fprintf(fid,"  hold on; plot(real(za),imag(za),'ob'); hold off;\n");
    fprintf(fid,"  legend('(\\omega_c)','poles','zeros');\n");
    fprintf(fid,"else,\n");
    fprintf(fid,"  legend('(\\omega_c)','poles');\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.2*max([wc abs(pa) abs(za)]));\n");
    //fprintf(fid,"     real(za),imag(za),'x');\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('real');\n");
    fprintf(fid,"ylabel('imag');\n");
    fprintf(fid,"\n");

    // plot group delay
    fprintf(fid,"figure;\n");
    fprintf(fid,"gd = gradient(unwrap(arg(h)))./gradient(w);\n");
    fprintf(fid,"semilogx(w,gd);\n");
    fprintf(fid,"xlabel('Angular frequency, \\omega [rad/s]');\n");
    fprintf(fid,"ylabel('group delay [s]');\n");
    fprintf(fid,"gd_min = min(gd); if gd_min < 0, gd_min=0; end;\n");
    fprintf(fid,"axis([wc/100 wc*100 gd_min 1.1*max(gd)]);\n");
    fprintf(fid,"grid on;\n");

    // plot magnitude response
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  semilogx(w,H,'-'); grid on;\n");
    fprintf(fid,"  axis([wc/100 wc*100 -5 1]);\n");
    fprintf(fid,"  xlabel('Angular frequency, \\omega [rad/s]');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  semilogx(w,H,'-'); grid on;\n");
    fprintf(fid,"  axis([wc/100 wc*100 -100 10]);\n");
    fprintf(fid,"  xlabel('Angular frequency, \\omega [rad/s]');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");

    fclose(fid);
    printf("results written to %s.\n", filename);

    printf("done.\n");
    return 0;
}


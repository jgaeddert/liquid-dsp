//
// iirdes_analog_example.c
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_analog_example.m"

int main() {
    // options
    unsigned int order=7;   // filter order
    float wc=1.0f;          // angular cutoff frequency
    float Ap=3.0f;          // pass-band Ap
    float As=60.0f;         // stop-band attenuation

    // filter type
    liquid_iirdes_filtertype ftype = LIQUID_IIRDES_BESSEL;

    // number of analaog poles/zeros
    unsigned int npa = order;
    unsigned int nza = 0;

    // complex analog zeros, poles, gain
    float complex za[order];
    float complex pa[order];
    float complex ka;

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
        butter_azpkf(order,wc,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY1:
        printf("Cheby-I filter design:\n");
        nza = 0;
        cheby1_azpkf(order,wc,ep,za,pa,&ka);
        break;
    case LIQUID_IIRDES_CHEBY2:
        printf("Cheby-II filter design:\n");
        nza = 2*L;
        float epsilon = powf(10.0f, -As/20.0f);
        cheby2_azpkf(order,wc,epsilon,za,pa,&ka);
        break;
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

        ellip_azpkf(order,wc,ep,es,za,pa,&ka);
        break;
    case LIQUID_IIRDES_BESSEL:
        printf("Bessel filter design:\n");
        bessel_azpkf(order,za,pa,&ka);
        nza = 0;
        break;
    default:
        fprintf(stderr,"error: iirdes_example: unknown filter type\n");
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

    for (i=0; i<npa; i++)
        printf("p(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(pa[i]), cimagf(pa[i]));
    printf("\n");
    for (i=0; i<nza; i++)
        printf("z(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(za[i]), cimagf(za[i]));
    printf("\n");
    printf("ka = %12.4e + j*%12.4e;\n", crealf(ka), cimagf(ka));

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"wc = %12.8f;\n", wc);
    fprintf(fid,"npa = %u;\n", npa);
    fprintf(fid,"nza = %u;\n", nza);
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

    fprintf(fid,"b = real(b);\n");
    fprintf(fid,"a = real(a);\n");

    fprintf(fid,"b = b*k;\n");

    fprintf(fid,"w = 10.^[-2:0.002:2]*wc;\n");
    fprintf(fid,"s = j*w;\n");
    fprintf(fid,"h = polyval(b,s) ./ polyval(a,s);\n");
    fprintf(fid,"H = 20*log10(abs(h));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogx(w,H,'-',wc,-3,'rs'); grid on;\n");
    fprintf(fid,"axis([wc/100 wc*100 -100 10]);\n");
    fprintf(fid,"xlabel('\\omega');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}


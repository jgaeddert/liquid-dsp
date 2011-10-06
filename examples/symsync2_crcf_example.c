//
// symsync2_crcf_example.c
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "symsync2_crcf_example.m"

int main() {
    // options
    unsigned int m=5;
    float beta=0.3f;
    unsigned int num_filters=64;
    unsigned int num_symbols=256;

    float bt=0.1f;      // loop filter bandwidth
    float dt=0.5f;      // fractional sample offset
    unsigned int ds=1;  // additional symbol delay
    
    // use random data or 101010 phasing pattern
    int random_data=1;


    unsigned int k=2;
    unsigned int i, n=0;

    // design interpolating filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    liquid_firdes_rrcos(k,m,beta,dt,h);

    // create interpolator
    interp_crcf q = interp_crcf_create(k,h,h_len);

    // design polyphase filter
    unsigned int H_len = 2*num_filters*k*m + 1;
    float H[H_len];
    liquid_firdes_rrcos(k*num_filters,m,beta,0,H);
    // create symbol synchronizer
    symsync2_crcf d = symsync2_crcf_create(num_filters, H, H_len);
    symsync2_crcf_set_lf_bw(d,bt);

    unsigned int num_samples = k*num_symbols;
    float complex x[num_symbols];
    float complex y[num_samples];
    float complex z[num_samples+10];

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"close all;\nclear all;\n\n");

    fprintf(fid,"k=%u;\n",k);
    fprintf(fid,"m=%u;\n",m);
    fprintf(fid,"beta=%12.8f;\n",beta);
    fprintf(fid,"num_filters=%u;\n",num_filters);
    fprintf(fid,"num_symbols=%u;\n",num_symbols);

    for (i=0; i<num_symbols; i++) {
        if (random_data) {
            // random signal (QPSK)
            x[i]  = rand() % 2 ? 1.0f : -1.0f;
            x[i] += rand() % 2 ? _Complex_I * 1.0f : -_Complex_I*1.0f;
        } else {
            x[i] = (i%2) ? 1.0f + _Complex_I : -1.0f - _Complex_I;  // 101010 phasing pattern
        }
    }

    // run interpolator
    for (i=0; i<num_symbols; i++) {
        interp_crcf_execute(q, x[i], &y[n]);
        n+=k;
    }

    // run symbol synchronizer
    unsigned int num_symbols_sync;
    symsync2_crcf_execute(d, &y[ds], num_samples-ds, z, &num_symbols_sync);

    printf("h(t) :\n");
    for (i=0; i<h_len; i++) {
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);
        fprintf(fid,"h(%3u) = %12.5f;\n", i+1, h[i]);
    }

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++) {
        //printf("  x(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"x(%3u) = %12.5f + j*%12.5f;\n", i+1, crealf(x[i]), cimagf(x[i]));
    }

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        //printf("  y(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"y(%3u) = %12.5f + j*%12.5f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    printf("z(t) :\n");
    for (i=0; i<num_symbols_sync; i++) {
        printf("  z(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(z[i]), cimagf(z[i]));
        fprintf(fid,"z(%3u) = %12.5f + j*%12.5f;\n", i+1, crealf(z[i]), cimagf(z[i]));
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"zp = filter(h,1,y);\n");
#if 0
    fprintf(fid,"figure;\nhold on;\n");
    fprintf(fid,"plot([0:length(x)-1],          real(x),    'ob');\n");
    fprintf(fid,"plot([0:length(y)-1]    -m,    real(y),    '-','Color',[0.8 0.8 0.8]);\n");
    fprintf(fid,"plot([0:length(zp)-1]   -2*m,  real(zp),   '-b');\n");
    fprintf(fid,"plot([0:length(z)-1]    -2*m+1,real(z),    'xr');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"xlabel('symbol index');\n");
    fprintf(fid,"ylabel('symbol/signal');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"legend('sym in','interp','mf','sym out',0);\n");
#endif

    fprintf(fid,"tsym=2:2:length(z);\n");
    fprintf(fid,"p=0.25;\n");
    fprintf(fid,"tsym0=tsym(1:floor(p*length(tsym)));\n");
    fprintf(fid,"tsym1=tsym(ceil(p*length(tsym)):length(tsym));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(z/k,'-','Color',[0.3 0.3 0.3]);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(z(tsym0)/k,'bx');\n");
    fprintf(fid,"plot(z(tsym1)/k,'rx');\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis square; grid on;\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature');\n");
    fprintf(fid,"legend(['z'],['sym (first ' num2str(100*p) '%%)'],['sym (last ' num2str(100*(1-p)) '%%)'],1);\n");
    fclose(fid);

    printf("results written to %s.\n", OUTPUT_FILENAME);

    // clean it up
    interp_crcf_destroy(q);
    symsync2_crcf_destroy(d);
    printf("done.\n");
    return 0;
}

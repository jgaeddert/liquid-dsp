//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframe64gen_example.m"

int main() {
    srand(time(NULL));

    // options
    modulation_scheme ms = MOD_QPSK;
    unsigned int bps = 2;

    unsigned int i;

    // create frame generator
    ofdmframe64gen fg = ofdmframe64gen_create();
    ofdmframe64gen_print(fg);

    // create modem
    modem mod = modem_create(ms,bps);

    // create auto-correlator
    autocorr_cccf ac0 = autocorr_cccf_create(64,16);
    autocorr_cccf ac1 = autocorr_cccf_create(64,64);
    autocorr_cccf ac2 = autocorr_cccf_create(16,64);

    // create cross-correlator
    float complex h[64];
    for (i=0; i<64; i++)
        h[64-i-1] = conjf(ofdmframe64_plcp_Lt[i]);
    firfilt_cccf xcorr = firfilt_cccf_create(h,64);

    float complex x[48];    // data buffer
    float complex y[160];   // framegen output buffer

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"y   = [];\n");
    fprintf(fid,"r0  = [];\n");
    fprintf(fid,"r1  = [];\n");
    fprintf(fid,"r2  = [];\n");

    unsigned int n=0;
    float complex r0, r1, r2, rxy;

    // write short sequence
    ofdmframe64gen_writeshortsequence(fg,y);
    for (i=0; i<160; i++) {
        autocorr_cccf_push(ac0,y[i]);   autocorr_cccf_execute(ac0,&r0);
        autocorr_cccf_push(ac1,y[i]);   autocorr_cccf_execute(ac1,&r1);
        autocorr_cccf_push(ac2,y[i]);   autocorr_cccf_execute(ac1,&r2);
        
        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"r0(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r0),   cimagf(r0));
        fprintf(fid,"r1(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r1),   cimagf(r1));
        fprintf(fid,"r2(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r2),   cimagf(r2));
        firfilt_cccf_push(xcorr,y[i]);
        firfilt_cccf_execute(xcorr,&rxy);
        fprintf(fid,"rxy(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(rxy), cimagf(rxy));
        n++;
    }

    // write long sequence
    ofdmframe64gen_writelongsequence(fg,y);
    for (i=0; i<160; i++) {
        autocorr_cccf_push(ac0,y[i]);   autocorr_cccf_execute(ac0,&r0);
        autocorr_cccf_push(ac1,y[i]);   autocorr_cccf_execute(ac1,&r1);
        autocorr_cccf_push(ac2,y[i]);   autocorr_cccf_execute(ac2,&r2);
        
        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"r0(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r0),   cimagf(r0));
        fprintf(fid,"r1(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r1),   cimagf(r1));
        fprintf(fid,"r2(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r2),   cimagf(r2));
        firfilt_cccf_push(xcorr,y[i]);
        firfilt_cccf_execute(xcorr,&rxy);
        fprintf(fid,"rxy(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(rxy), cimagf(rxy));
        n++;
    }

    // generate data sequence
    unsigned int s;
    for (i=0; i<48; i++) {
        s = modem_gen_rand_sym(mod);
        modem_modulate(mod, s, &x[i]);
    }
    ofdmframe64gen_writesymbol(fg,x,y);
    for (i=0; i<80; i++) {
        autocorr_cccf_push(ac0,y[i]);   autocorr_cccf_execute(ac0,&r0);
        autocorr_cccf_push(ac1,y[i]);   autocorr_cccf_execute(ac1,&r1);
        autocorr_cccf_push(ac2,y[i]);   autocorr_cccf_execute(ac2,&r2);
        
        fprintf(fid,"y(%3u)  = %12.4e + j*%12.4e;\n", n+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"r0(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r0),   cimagf(r0));
        fprintf(fid,"r1(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r1),   cimagf(r1));
        fprintf(fid,"r2(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(r2),   cimagf(r2));
        firfilt_cccf_push(xcorr,y[i]);
        firfilt_cccf_execute(xcorr,&rxy);
        fprintf(fid,"rxy(%3u) = %12.4e + j*%12.4e;\n", n+1, crealf(rxy), cimagf(rxy));
        n++;
    }


    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(length(y)-1);\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");


    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(length(r0)-1);\n");
    fprintf(fid,"plot(t,abs(r0),t,abs(r1),t,abs(r2),t,abs(rxy));\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    ofdmframe64gen_destroy(fg);
    modem_destroy(mod);
    autocorr_cccf_destroy(ac0);
    autocorr_cccf_destroy(ac1);
    autocorr_cccf_destroy(ac2);
    firfilt_cccf_destroy(xcorr);

    printf("done.\n");
    return 0;
}


//
//
//

#include <stdio.h>

#include "liquid.h"

int main() {
    // filter parameters
    unsigned int k=4;
    unsigned int m=3;
    unsigned int n=4;
    float beta = 0.7f;

    // up-sampled filter prototype
    unsigned int h_len = 2*k*m*n + 1;
    float h[h_len];
    design_rrc_filter(k*n,m,beta,0,h);

    // polyphase matched filter
    firpfb_rrrf mf = firpfb_rrrf_create(n,h,h_len);
    firpfb_rrrf_print(mf);

    // polyphase derivative matched filter
    float hp[h_len-1];
    unsigned int i;
    for (i=0; i<h_len-1; i++)
        hp[i] = h[i+1] - h[i];
    firpfb_rrrf dmf = firpfb_rrrf_create(n,hp,h_len-1);
    firpfb_rrrf_print(dmf);

    // memory clean-up
    firpfb_rrrf_destroy(mf);
    firpfb_rrrf_destroy(dmf);
    printf("done.\n");
    return 0;
}


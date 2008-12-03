//
//
//

#include <stdio.h>

#include "../src/filter.h"
#include "../src/firdes.h"

int main() {
    // filter parameters
    unsigned int k=4;
    unsigned int m=3;
    unsigned int n=4;
    float beta = 0.7f;

    //
    unsigned int h_len = 2*k*m*n + 1;
    float h[h_len];
    design_rrc_filter(k*n,m,beta,0,h);
    firpfb f = firpfb_create(n,h,h_len);
    firpfb_print(f);
    firpfb_destroy(f);

    printf("done.\n");
    return 0;
}


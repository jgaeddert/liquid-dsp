//
// firdespm.c
//
// Finite impulse response filter design using Parks-McClellan
// algorithm.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

int main() {
#if 0
    //  N = 24              :   filter length
    //  s = N%2 = 0         :   odd length?
    //  n = (N-s)/2 = 12    :   filter semi-length
    //  r = n + s = 12      :   number of approximating functions
    //  ne = r+1 = 13       :   number of extrema
    //  ne_max = ne+2 = 15  :   maximum number of extrema
    unsigned int n=24;
    float fp = 0.08f;
    float fs = 0.16f;
#else
    //  N = 15              :   filter length
    //  s = N%2 = 1         :   odd length?
    //  n = (N-s)/2 = 7     :   filter semi-length
    //  r = n = 7           :   number of approximating functions
    //  ne = ?              :   number of extrema
    //  ne_max = ne+2 = 9   :   maximum number of extrema
    unsigned int n=15;
    float fp = 0.375/2.0f;
    float fs = 0.25f;

    n = 25;
    fp = 0.08;
    fs = 0.16;

    n = 25;
    fp = 0.1f;
    fs = 0.2f;
#endif
    float K  = 1.0f;

    float h[n];
    firdespm(n,fp,fs,K,h);

    printf("done.\n");
    return 0;
}


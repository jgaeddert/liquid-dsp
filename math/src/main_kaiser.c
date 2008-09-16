//
//
//

#include <stdio.h>
#include "kaiser.h"

int main() {
    unsigned int N=13;
    float beta = 2.0f;

    float kn;
    unsigned int i;

    for (i=0; i<10; i++)
        printf("%u! = %f\n", i, factorialf(i));

    printf("I_0(3) = %f\n", besseli_0(3.0f));

    for (i=0; i<N; i++) {
        kn = kaiser(i,N,beta);
        printf("h(%u) = %f;\n", i, kn);
    }

    float x;
    for (x=-0.5; x<0.5; x+= 0.1f) {
        printf("sinc(%f) = %f\n", x, sincf(x));
    }

    float h[N];
    printf("designing fir filter...\n");
    fir_kaiser_window(N, 0.5f, 40.0f, h);
    printf("complete!\n");

    for (i=0; i<N; i++) {
        printf("h(%u) = %f;\n", i+1, h[i]);
    }

    printf("done.\n");
    return 0;
}


//
// Test random number generator
//

#include <stdio.h>
#include "random.h"

int main() {
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float x, y, m1, m2;

    printf("running tests, %lu trials\n", N);

    // uniform
    m1=0.0f;
    m2=0.0f;
    for (i=0; i<N; i++) {
        x = randf();
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;
    printf("UNIFORM:\tmean: %1.3f\tvar: %1.3f\n", m1, m2);

    // gauss
    m1=0.0f;
    m2=0.0f;
    for (i=0; i<N; i++) {
        randnf(&x,&y);
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;
    printf("GAUSS:  \tmean: %1.3f\tvar: %1.3f\n", m1, m2);

    printf("done.\n");
    return 0;
}


//
// Test random number generator
//

#include <stdio.h>
#include "../src/random.h"

int main() {
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float x, m1, m2;

    printf("running tests, %lu trials\n", N);
    printf("dist       mean   (expected)   var    (expected)\n");
    printf("-----      ----------------    -----------------\n");

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
    printf("uniform: %8.4f (%8.4f) %8.4f (%8.4f)\n", m1, 0.5f, m2, 1/12.0f);

    // gauss
    m1=0.0f;
    m2=0.0f;
    for (i=0; i<N; i++) {
        x = randnf();
        m1 += x;
        m2 += x*x;
    }
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;
    printf("Gauss:   %8.4f (%8.4f) %8.4f (%8.4f)\n", m1, 0.0f, m2, 1.0f);

    // Weibull
    m1=0.0f;
    m2=0.0f;
    for (i=0; i<N; i++) {
        // alpha = 1.0f
        // beta = 2.0f
        // gamma = 0.0f
        x = rand_weibullf(1.0f, 2.0f, 0.0f);
        m1 += x;
        m2 += x*x;
    }
    // m1 = (b/a)^(1/b)*gamma(1+1/b)
    m1 /= (float) N;
    // m2 = (b/a)^(2/b)*(gamma(1+2/b)-(gamma(1+1/b))^2)
    m2 = (m2 / (float)N) - m1*m1;
    printf("weibull: %8.4f (%8.4f) %8.4f (%8.4f)\n", m1, 1.2533f, m2, 0.42920f);

    printf("done.\n");
    return 0;
}


//
// (Fast) discrete cosine transform
//

#include <stdio.h>
#include "liquid.h"

int main() {
    unsigned int n=16;
    float x[n]; // time-domain 'signal'
    float y[n]; // fft(x)
    float z[n]; // ifft(y)

    unsigned int i;
    for (i=0; i<n; i++)
        x[i] = (float)i;

    // execute ffts
    dct(x,y,n);
    idct(y,z,n);

    // normalize inverse
    for (i=0; i<n; i++)
        z[i] *= 2.0f / (float) n;

    // print results
    printf("original signal, x[n]:\n");
    for (i=0; i<n; i++)
        printf("  x[%3u] = %8.4f\n", i, x[i]);

    printf("y[n] = dct( x[n] ):\n");
    for (i=0; i<n; i++)
        printf("  y[%3u] = %8.4f\n", i, y[i]);

    printf("z[n] = idct( y[n] ):\n");
    for (i=0; i<n; i++)
        printf("  z[%3u] = %8.4f\n", i, z[i]);

    printf("done.\n");
    return 0;
}


// file: doc/tutorials/framing_basic_tutorial.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <liquid/liquid.h>

int main() {
    // allocate memory for arrays
    unsigned char header[8];        // data header
    unsigned char payload[64];      // data payload
    float complex y[FRAME64_LEN];   // frame samples

    // CREATE frame generator
    framegen64 fg = framegen64_create();
    framegen64_print(fg);

    // initialize header, payload
    unsigned int i;
    for (i=0; i<8; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // EXECUTE generator and assemble the frame
    framegen64_execute(fg, header, payload, y);

    // print a few of the generated frame samples to the screen
    for (i=0; i<30; i++)
        printf("%3u : %12.8f + j*%12.8f\n", i, crealf(y[i]), cimagf(y[i]));

    // DESTROY objects
    framegen64_destroy(fg);

    printf("done.\n");
    return 0;
}

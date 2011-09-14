// file: doc/listings/iirdes.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int order=5;   // filter order
    float fc = 0.20f;       // cutoff frequency (low-pass prototype)
    float f0 = 0.25f;       // center frequency (band-pass, band-stop)
    float As = 60.0f;       // stopband attenuation [dB]
    float Ap = 1.0f;        // passband ripple [dB]

    // derived values
    unsigned int N = 2*order;   // effective order (double because band-pass)
    unsigned int r = N % 2;     // odd/even order
    unsigned int L = (N-r)/2;   // filter semi-length

    // filter coefficients arrays
    float B[3*(L+r)];
    float A[3*(L+r)];

    // design filter
    liquid_iirdes(LIQUID_IIRDES_ELLIP,
                  LIQUID_IIRDES_BANDPASS,
                  LIQUID_IIRDES_SOS,
                  order,
                  fc, f0, Ap, As,
                  B,  A);

    // print results
    unsigned int i;
    printf("B [%u x 3] :\n", L+r);
    for (i=0; i<L+r; i++)
        printf("  %12.8f %12.8f %12.8f\n", B[3*i+0], B[3*i+1], B[3*i+2]);
    printf("A [%u x 3] :\n", L+r);
    for (i=0; i<L+r; i++)
        printf("  %12.8f %12.8f %12.8f\n", A[3*i+0], A[3*i+1], A[3*i+2]);
}

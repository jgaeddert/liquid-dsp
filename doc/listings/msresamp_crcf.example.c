// file: doc/listings/msresamp_crcf.example.c
#include <liquid/liquid.h>

int main() {
    // options
    float r=0.117f;     // resampling rate (output/input)
    float As=60.0f;     // resampling filter stop-band attenuation [dB]

    // create multi-stage arbitrary resampler object
    msresamp_crcf q = msresamp_crcf_create(r,As);
    msresamp_crcf_print(q);

    unsigned int nx = 400;          // input size
    unsigned int ny = ceilf(nx*r);  // expected output size
    float complex x[nx];            // input buffer
    float complex y[ny];            // output buffer
    unsigned int num_written;       // number of values written to buffer

    // ... initialize input ...

    // execute resampler, storing result in output buffer
    msresamp_crcf_execute(q, x, nx, y, &num_written);

    // ... repeat as necessary ...

    // clean up allocated objects
    msresamp_crcf_destroy(q);
}

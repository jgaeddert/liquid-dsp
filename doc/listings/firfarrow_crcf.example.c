// file: doc/listings/firfarrow_crcf.example.c
#include <liquid/liquid.h>

int main()
{
    // options
    unsigned int h_len=19;  // filter length
    unsigned int Q=5;       // polynomial order
    float fc=0.45f;         // filter cutoff
    float As=60.0f;         // stop-band attenuation [dB]

    // generate filter object
    firfarrow_crcf q = firfarrow_crcf_create(h_len, Q, fc, As);

    // set fractional sample delay
    firfarrow_crcf_setdelay(q, 0.3f);

    float complex x;    // input sample
    float complex y;    // output sample
    
    // execute filter (repeat as necessary)
    {
        firfarrow_crcf_push(q, x);      // push input sample
        firfarrow_crcf_execute(q,&y);   // compute output
    }

    // destroy object
    firfarrow_crcf_destroy(q);
}

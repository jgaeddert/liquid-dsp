// file: doc/listings/eqlms_cccf_blind.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int k=2;           // filter samples/symbol
    unsigned int m=3;           // filter semi-length (symbols)
    float beta=0.3f;            // filter excess bandwidth factor
    float mu=0.100f;            // LMS equalizer learning rate

    // allocate memory for arrays
    float complex * x;          // equalizer input sample buffer
    float complex * y;          // equalizer output sample buffer

    // ...initialize x, y...

    // create LMS equalizer (initialized on square-root Nyquist
    // filter prototype) and set learning rate
    eqlms_cccf q = eqlms_cccf_create_rnyquist(LIQUID_RNYQUIST_RRC, k, m, beta, 0);
    eqlms_cccf_set_bw(q, mu);

    // iterate through equalizer learning
    unsigned int i;
    {
        // push input sample into equalizer and compute output
        eqlms_cccf_push(q, x[i]);
        eqlms_cccf_execute(q, &y[i]);

        // decimate output
        if ( (i%k) == 0 ) {
            // make decision and update internal weights
            float complex d_hat = crealf(y[i]) > 0.0f ? 1.0f : -1.0f;
            eqlms_cccf_step(q, d_hat, y[i]);
        }
    }

    // destroy equalizer object
    eqlms_cccf_destroy(q);
}

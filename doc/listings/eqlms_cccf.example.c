// file: doc/listings/eqlms_cccf.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int n=32;          // number of training symbols
    unsigned int p=10;          // equalizer order
    float mu=0.500f;            // LMS learning rate

    // allocate memory for arrays
    float complex x[n];         // received samples
    float complex d_hat[n];     // output symbols
    float complex d[n];         // traning symbols

    // ...initialize x, d_hat, d...

    // create LMS equalizer and set learning rate
    eqlms_cccf q = eqlms_cccf_create(NULL,p);
    eqlms_cccf_set_bw(q, mu);

    // iterate through equalizer learning
    unsigned int i;
    {
        // push input sample
        eqlms_cccf_push(q, x[i]);

         // compute output sample
        eqlms_cccf_execute(q, &d_hat[i]);

        // update internal weights
        eqlms_cccf_step(q, d[i], d_hat[i]);
    }

    // clean up allocated memory
    eqlms_cccf_destroy(q);
}

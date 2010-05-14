#include <liquid/liquid.h>
// ...
{
    // options
    unsigned int n=32;          // number of training symbols
    unsigned int p=10;          // equalizer order
    float mu=0.500f;            // LMS learning rate

    // allocate memory for arrays
    float complex y[n];         // received symbols
    float complex d[n];         // traning symbols
    float complex w[p];         // weights

    // initialize y, d, w...

    // create LMS equalizer and set learning rate
    eqlms_cccf eq = eqlms_cccf_create(p);
    eqlms_cccf_set_bw(eq, mu);

    // train equalizer on known data set, one sample at a time
    float complex d_hat;    // filtered output estimate
    for (i=0; i<n; i++)
        eqlms_cccf_execute(eqlms, y[i], d[i], &d_hat);
    eqlms_cccf_get_weights(eqlms, w);

    // equivalently, run batch training on initialized weights
    eqlms_cccf_train(eq, w, y, d, ntrain);

    // create filter from equalizer output and apply to signal
    fir_filter_cccf f = fir_filter_cccf_create(w,p);

    // ...

    // clean up allocated memory
    eqlms_cccf_destroy(eq);
    fir_filter_cccf_destroy(f);
}

#include <liquid/liquid.h>

int main() {
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
    eqlms_cccf eq = eqlms_cccf_create(NULL,p);
    eqlms_cccf_set_bw(eq, mu);

    // run batch training on initialized weights
    eqlms_cccf_train(eq, w, y, d, n);

    // create filter from equalizer output and apply to signal
    firfilt_cccf f = firfilt_cccf_create(w,p);

    // ...

    // clean up allocated memory
    eqlms_cccf_destroy(eq);
    firfilt_cccf_destroy(f);
}

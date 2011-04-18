// file: doc/listings/symsync_crcf.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int k=2;           // samples/symbol
    unsigned int m=3;           // filter delay (symbols)
    float beta=0.3f;            // filter excess bandwidth factor
    unsigned int Npfb=32;       // number of polyphase filters in bank
    liquid_rnyquist_type ftype = LIQUID_RNYQUIST_RRC;

    // create symbol synchronizer
    symsync_crcf q = symsync_crcf_create_rnyquist(ftype,k,m,beta,Npfb);

    float complex * x;          // complex input
    float complex * y;          // output buffer
    unsigned int nx;            // number of input samples
    unsigned int num_written;   // number of values written to buffer

    // ... initialize input, output ...

    // execute symbol synchronizer, storing result in output buffer
    symsync_crcf_execute(q, x, nx, y, &num_written);

    // ... repeat as necessary ...

    // clean up allocated objects
    symsync_crcf_destroy(q);
}

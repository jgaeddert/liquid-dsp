// file: doc/listings/fec.example.c
#include <liquid/liquid.h>

int main() {
    unsigned int n = 64;                    // decoded message length (bytes)
    fec_scheme fs = LIQUID_FEC_HAMMING74;   // error-correcting scheme

    // compute encoded message length
    unsigned int k = fec_get_enc_msg_length(fs, n);

    // allocate memory for data arrays
    unsigned char msg_org[n];       // original message
    unsigned char msg_enc[k];       // encoded message
    unsigned char msg_rec[k];       // received message
    unsigned char msg_dec[n];       // decoded message

    // create fec objects
    fec encoder = fec_create(fs,NULL);
    fec decoder = fec_create(fs,NULL);

    // repeat as necessary
    {
        // ... initialize message ...

        // encode message
        fec_encode(encoder, n, msg_org, msg_enc);

        // ... push through channel ...

        // decode message
        fec_decode(decoder, n, msg_rec, msg_dec);
    }

    // clean up objects
    fec_destroy(encoder);
    fec_destroy(decoder);

    return 0;
}


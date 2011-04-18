// file: doc/listings/lifecycle.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int order=4;   // filter order
    float fc=0.1f;          // cutoff frequency
    float f0=0.25f;         // center frequency (bandpass|bandstop)
    float Ap=1.0f;          // pass-band ripple [dB]
    float As=40.0f;         // stop-band attenuation [dB]
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_ELLIP;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_BANDPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;

    // CREATE filter object (and print to stdout)
    iirfilt_crcf myfilter;
    myfilter = iirfilt_crcf_create_prototype(ftype,
                                             btype,
                                             format,
                                             order,
                                             fc, f0,
                                             Ap, As);
    iirfilt_crcf_print(myfilter);

    // allocate memory for data arrays
    unsigned int n=128; // number of samples
    float complex x[n]; // input samples array
    float complex y[n]; // output samples array

    // run filter
    unsigned int i;
    for (i=0; i<n; i++) {
        // initialize input
        x[i] = randnf() + _Complex_I*randnf();

        // EXECUTE filter (repeat as many times as desired)
        iirfilt_crcf_execute(myfilter, x[i], &y[i]);
    }

    // DESTROY filter object
    iirfilt_crcf_destroy(myfilter);
}

// file: doc/listings/firdespm.example.c
#include <liquid/liquid.h>

int main() {
    // define filter length, type, number of bands
    unsigned int n=55;
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    unsigned int num_bands = 4;

    // band edge description [size: num_bands x 2]
    float bands[8]   = {0.0f,   0.1f,   // 1    first pass-band
                        0.15f,  0.3f,   // 0    first stop-band
                        0.33f,  0.4f,   // 0.1  second pass-band
                        0.42f,  0.5f};  // 0    second stop-band

    // desired response [size: num_bands x 1]
    float des[4]     = {1.0f, 0.0f, 0.1f, 0.0f};

    // relative weights [size: num_bands x 1]
    float weights[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // in-band weighting functions [size: num_bands x 1]
    liquid_firdespm_wtype wtype[4] = {LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT};

    // allocate memory for array and design filter
    float h[n];
    firdespm_run(n,num_bands,bands,des,weights,wtype,btype,h);
}


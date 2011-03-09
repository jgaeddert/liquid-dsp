#include <liquid/liquid.h>

int main() {
    unsigned int n=55;  // filter length
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;
    unsigned int num_bands = 4;
    float bands[8]   = {0.0f,   0.1f,   // 1
                        0.15f,  0.3f,   // 0
                        0.33f,  0.4f,   // 0.1
                        0.42f,  0.5f};  // 0
    float des[4]     = {1.0f, 0.0f, 0.1f, 0.0f};
    float weights[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    liquid_firdespm_wtype wtype[4] = {LIQUID_FIRDESPM_FLATWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT,
                                      LIQUID_FIRDESPM_EXPWEIGHT};
    float h[n];
    firdespm_run(n,bands,des,weights,num_bands,btype,wtype,h);
}


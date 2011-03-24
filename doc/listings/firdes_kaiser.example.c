#include <liquid/liquid.h>

int main() {
    // options
    float fc=0.15f;         // filter cutoff frequency
    float ft=0.05f;         // filter transition
    float As=60.0f;         // stop-band attenuation [dB]
    float mu=0.0f;          // fractional timing offset

    // estimate required filter length and generate filter
    unsigned int h_len = estimate_req_filter_len(ft,As);
    float h[h_len];
    firdes_kaiser_window(h_len,fc,As,mu,h);
}


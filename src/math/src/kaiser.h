//
// Finite impulse response filter design
//

// ln( gamma(z) )
float lngammaf(float _z);

// gamma(z)
float gammaf(float _z);

// n!
float factorialf(unsigned int _n);

//
float besseli_0(float _z);

//
float kaiser(unsigned int _n, unsigned int _N, float _beta);

// sin(pi x) / (pi x)
float sincf(float _x);

void fir_kaiser_window(unsigned int _n, float _fc, float _atten_dB, float *_h);

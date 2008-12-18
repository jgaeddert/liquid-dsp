//
// Useful mathematical formulae
//

#ifndef __LIQUID_MATH_H__
#define __LIQUID_MATH_H__

// ln( gamma(z) )
float lngammaf(float _z);

// gamma(z)
float gammaf(float _z);

// n!
float factorialf(unsigned int _n);

// Bessel function of the first kind
float besselj_0(float _z);

// Modified Bessel function of the first kind
float besseli_0(float _z);

// sin(pi x) / (pi x)
float sincf(float _x);

// 
// Windowing functions
//

// Kaiser window
float kaiser(unsigned int _n, unsigned int _N, float _beta);

// Hamming window
float hamming(unsigned int _n, unsigned int _N);

// Hann window
float hann(unsigned int _n, unsigned int _N);

// Blackman-harris window
float blackmanharris(unsigned int _n, unsigned int _N);


#endif // __LIQUID_MATH_H__

//
// Finite impulse response filter design
//

#ifndef __LIQUID_FIRDES_H__
#define __LIQUID_FIRDES_H__

// Design FIR filter using window method
//  _n      : filter length (odd)
//  _fc     : filter cutoff (0 < _fc < 1)
//  _slsl   : sidelobe suppression level (_slsl < 0)
//  _h      : output coefficient buffer
void fir_design_windowed_sinc(unsigned int _n, float _fc, float _slsl, float *_h);

//
void fir_design_halfband_windowed_sinc(float * _h, unsigned int _n);

// Design FIR using kaiser window
//  _n      : filter length
//  _fc     : cutoff frequency
//  _slsl   : sidelobe suppression level (dB attenuation)
//  _h      : output coefficient buffer
void fir_kaiser_window(unsigned int _n, float _fc, float _slsl, float *_h);

// Design FIR doppler filter
//  _n      : filter length
//  _fd     : normalized doppler frequency (0 < _fd < 0.5)
//  _K      : Rice fading factor (K >= 0)
//  _theta  : LoS component angle of arrival
//  _h      : output coefficient buffer
void fir_design_doppler(unsigned int _n, float _fd, float _K, float _theta, float *_h);

// Design optimum FIR root-nyquist filter
//  _n      : filter length
//  _k      : samples/symbol (_k > 1)
//  _slsl   : sidelobe suppression level
void fir_design_optim_root_nyquist(unsigned int _n, unsigned int _k, float _slsl, float *_h);

// Design root-Nyquist raised-cosine filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void design_rrc_filter(
    unsigned int _k,
    unsigned int _m,
    float _beta,
    float _dt, 
    float * _h
);

// 
// Windowing functions
//
float kaiser(unsigned int _n, unsigned int _N, float _beta);

//
// IIR filter design
//

void butterf(unsigned int _n, float * _a);

// Chebyshev type-I filter design
//  _n  :   filter order
//  _ep :   epsilon, passband ripple
//  _b  :   numerator coefficient array (length 1)
//  _a  :   denominator coefficient array (length _n+1)
void cheby1f(unsigned int _n, float _ep, float * _b, float * _a);

#endif // __LIQUID_FIRDES_H__

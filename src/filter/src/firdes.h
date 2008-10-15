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

#endif // __LIQUID_FIRDES_H__

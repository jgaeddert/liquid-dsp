//
// windowing functions
//

#ifndef __LIQUID_WINDOW_H__
#define __LIQUID_WINDOW_H__

//
float kaiser(unsigned int _n, unsigned int _N, float _beta);

void fir_kaiser_window(unsigned int _n, float _fc, float _atten_dB, float *_h);

#endif // __LIQUID_WINDOW_H__

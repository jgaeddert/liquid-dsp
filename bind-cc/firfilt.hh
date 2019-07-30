// firfilt c++ bindings
#ifndef __FIRFILT_HH__
#define __FIRFILT_HH__

#include <complex>
#include <liquid/liquid.h>

class firfilt
{
  public:
    // kaiser
    firfilt(unsigned int _n=51, float _fc=0.25, float _As=60.0f, float _mu=0.0f)
        { q = firfilt_crcf_create_kaiser(_n, _fc, _As, _mu); }

    ~firfilt();

    std::complex<float> execute(std::complex<float> _x)
    {
        std::complex<float> y;
        firfilt_crcf_push(q, _x);
        firfilt_crcf_execute(q, &y);
        return y;
    }

  private:
    firfilt_crcf q;
};

#endif //__FIRFILT_HH__

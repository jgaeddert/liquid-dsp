
liquid-dsp C++ and Python Bindings
==================================

## Installation and Dependencies ##

Building python bindings depends on
[pybind11](https://pybind11.readthedocs.io/en/stable/),
the `python3` development libraries, and
a compatible C++14 compiler.

Once these dependencies are installed, you can build the liquid-dsp python
library with

    make python

From python3 simply use `import liquid as dsp`.

## Build Details

While C is indeed stricly a subset of C++, sometimes having a class structure
is more convenient than using C-style strucs. These bindings do two things:

  1. Wrap the C-style functionality into a set of header-only C++ class libraries
  1. Bind these C++ classes into python3

The original C example can be re-written in C++ as follows:

```c++
// get in, manipulate data, get out
#include <liquid/firinterp.h>
int main() {
    unsigned int M  = 4;     // interpolation factor
    unsigned int m  = 12;    // filter delay [symbols]
    float        As = 60.0f; // filter stop-band attenuation [dB]

    // create interpolator from prototype
    liquid::firinterp_crcf interp(M,m,As);
    std::complex<float> x = 1.0f;  // input sample
    std::complex<float> y[M];      // interpolated output buffer

    // repeat on input sample data as needed
    {
        interp.execute(x, y);
    }
    return 0;
}
```

and in python3 as:

```python
import liquid as dsp
import numpy as np

# create the interpolator
interp = dsp.firinterp("kaiser", M=4, m=12, As=60.)

# create pre-allocated buffer
buf = np.zeros(shape=(4,), dtype=np.csingle)

# run on a single sample
interp.execute(1.0, buf)
```

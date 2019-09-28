#!/usr/bin/env python3
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp

#filt = dsp.firfilt(np.arange(10,dtype=np.single)) # create filter from coefficients array
#filt = dsp.firfilt(n=13, fc=0.1)                  # create prototype low-pass Kaiser filter
filt = dsp.firfilt(ftype=7, k=2, m=3)             # create prototype (root) Nyquist filter

filt.display()
buf = np.zeros((13,), dtype=np.csingle)
buf[0] = 1.
filt.execute(buf)
print(buf)


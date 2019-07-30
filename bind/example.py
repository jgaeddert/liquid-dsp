#!/usr/bin/env python3
import numpy as np
import liquid as dsp

filt = dsp.firfilt(n=13)
buf = np.zeros((13,), dtype=np.csingle)
buf[0] = 1.
filt.execute(buf)
print(buf)


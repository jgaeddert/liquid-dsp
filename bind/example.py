#!/usr/bin/env python3
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp

filt = dsp.firfilt(ftype=7, m=2, k=4)
filt.display()
buf = np.zeros((13,), dtype=np.csingle)
buf[0] = 1.
filt.execute(buf)
print(buf)


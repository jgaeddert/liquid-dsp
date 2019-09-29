#!/usr/bin/env python3
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

m = 100 # filter semi-length

#filt = dsp.firfilt(np.arange(10,dtype=np.single)) # create filter from coefficients array
filt = dsp.firfilt(h_len=2*m+1, fc=0.03)                  # create prototype low-pass Kaiser filter
#filt = dsp.firfilt(ftype=7, k=2, m=3)             # create prototype (root) Nyquist filter
#filt.display()

# compute impulse response
buf = np.zeros((2*m+1,), dtype=np.csingle)
buf[0] = 1.
filt.execute(buf)

# plot response
fig, ax = plt.subplots()
ax.plot(np.linspace(-m,m,2*m+1), buf)
ax.set_xlabel('Delay [samples]')
ax.set_ylabel('Impulse Response')
ax.grid(True, zorder=5)
plt.show()


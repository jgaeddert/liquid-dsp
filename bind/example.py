#!/usr/bin/env python3
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

m    = 100 # filter semi-length
nfft = 2400

filt = dsp.firfilt("lowpass", n=2*m+1, fc=0.05)
#filt = dsp.firfilt("dcblock", m=m, As=40)
filt.display()

n = filt.get_length()

# compute impulse response
buf = np.zeros((n,), dtype=np.csingle)
buf[0] = 1.
filt.execute(buf)

# compute filter response in dB
psd = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(buf, nfft))))

# plot impulse and spectral responses
fig, ax = plt.subplots(2,figsize=(8,8))
ax[0].plot(np.linspace(-m,m,2*m+1), buf)
ax[0].set_xlabel('Delay [samples]')
ax[0].set_ylabel('Impulse Response')
ax[0].grid(True, zorder=5)
ax[1].plot(np.arange(nfft)/nfft-0.5, psd)
ax[1].set_xlabel('Normalized Frequency [f/F_s]')
ax[1].set_ylabel('Power Spectral Density [dB]')
ax[1].grid(True, zorder=5)
plt.show()


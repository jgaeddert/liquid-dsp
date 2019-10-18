#!/usr/bin/env python3
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

# generate coefficients and pass them to filter object
m = 40      # filter semi-length
fc= 0.1     # filter cut-off frequency
t = np.arange(2*m+1) - m
h = np.sinc(fc*t) * np.cos(0.5*np.pi*t/m)**2
filt = dsp.firfilt(h)

# design prototype filter
#filt = dsp.firfilt("lowpass", n=151, fc=0.07)
#filt = dsp.firfilt("firdespm", n=151, fc=0.07)
#filt = dsp.firfilt("rkaiser", k=8, m=12, beta=0.25)
#filt = dsp.firfilt("rect", n=5)
#filt = dsp.firfilt("dcblock", m=20, As=40)
#filt = dsp.firfilt("notch", m=20, As=40, f0=0.1)
filt.display()

# compute impulse response
n = filt.get_length()
buf = np.zeros((n,), dtype=np.csingle)
buf[0] = 1.
filt.execute(buf)

# compute filter response in dB
nfft = 2400
psd = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(buf, nfft))))

# plot impulse and spectral responses
fig, ax = plt.subplots(2,figsize=(8,8))
ax[0].plot(np.arange(n)-n//2, buf)
ax[0].set_xlabel('Delay [samples]')
ax[0].set_ylabel('Impulse Response')
ax[0].grid(True, zorder=5)
ax[1].plot(np.arange(nfft)/nfft-0.5, psd)
ax[1].set_xlabel('Normalized Frequency [f/F_s]')
ax[1].set_ylabel('Power Spectral Density [dB]')
ax[1].grid(True, zorder=5)
plt.show()


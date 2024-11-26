#!/usr/bin/env python3
'''demonstrate finite impulse response filter object'''
import argparse
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# generate coefficients and pass them to filter object
#m = 40      # filter semi-length
#fc= 0.1     # filter cut-off frequency
#t = np.arange(2*m+1) - m
#h = np.sinc(fc*t) * np.cos(0.5*np.pi*t/m)**2
#filt = dsp.firfilt(h)

# design prototype filter
filt = dsp.firfilt("lowpass", n=151, fc=0.07, scale=0.14)
#filt = dsp.firfilt("firdespm", n=151, fc=0.07, scale=0.14)
#filt = dsp.firfilt("rkaiser", k=8, m=12, beta=0.25)
#filt = dsp.firfilt("rect", n=5)
#filt = dsp.firfilt("dcblock", m=20, As=40)
#filt = dsp.firfilt("notch", m=20, As=40, f0=0.1)
#filt.scale = 2*0.07
print(filt)

# get impulse response, adjusting for filter scale
h = filt.coefficients * filt.scale

# compute filter response in dB
nfft = 2400
psd = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(h, nfft))))

# plot impulse and spectral responses
fig, (ax1, ax2) = plt.subplots(2,figsize=(8,8))
n = filt.length
ax1.plot(np.arange(n)-n//2, np.real(h))
ax1.set_xlabel('Delay [samples]')
ax1.set_ylabel('Impulse Response')
ax1.grid(True, zorder=5)
ax2.plot(np.arange(nfft)/nfft-0.5, psd)
ax2.set_xlabel('Normalized Frequency [f/F_s]')
ax2.set_ylabel('Power Spectral Density [dB]')
ax2.set(xlim=(-0.5,0.5))
ax2.grid(True, zorder=5)
if not args.nodisplay:
    plt.show()


#!/usr/bin/env python3
'''demonstrate finite impulse response Hilbert transform'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# options
m, As, num_samples = 6, 60., 240

# generate input signal (windowed tone)
buf_0 = np.concatenate(
    ( np.sin(2*np.pi*0.212121*np.arange(num_samples))*np.hanning(num_samples),
      np.zeros(4*m)
     ))

# create object
q = dsp.firhilb(m, As)
print(q)

# decimate: convert to complex-valued signal
buf_1 = q.execute_decim(buf_0)

# interpolate: convert back to real-valued signal
q.reset()
buf_2 = q.execute_interp(buf_1)

# plot results
t0 = np.arange(len(buf_0))
t1 = np.arange(len(buf_1))/2

# plot impulse and spectral responses
fig, ax = plt.subplots(3,2,figsize=(12,8))
ax[0,0].plot(t0, buf_0)
ax[1,0].plot(t1, np.real(buf_1), t1, np.imag(buf_1))
ax[2,0].plot(t0, buf_2)

nfft = 1200
f = np.arange(nfft)/nfft - 0.5
for a,buf in zip(ax[:,1], (buf_0,buf_1,buf_2)):
    a.plot(f, 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(buf,nfft)))))
    a.set_xlabel('Normalized Frequency')
    a.set_ylabel('PSD [dB]')
    a.grid(True, zorder=5)

if not args.nodisplay:
    plt.show()


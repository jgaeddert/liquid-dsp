#!/usr/bin/env python3
'''demonstrate framesync64 detection using class with callback function'''
import argparse
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# generate a frame
fg      = dsp.fg64()
header  = np.random.randint(256, size=fg.header_len,  dtype=np.uint8)
payload = np.random.randint(256, size=fg.payload_len, dtype=np.uint8)
#payload = np.arange(16, dtype=np.uint8)
#payload = 'hello, world!'
frame   = fg.execute(header, payload)

def callback(context,header,payload,stats):
    print('frame detected!')
    print(payload)
    #print(''.join([chr(c) for c in payload]))
    context.update(stats)

# create frame synchronizer and receive frame
context = {'syms':np.zeros((0,))}
fs = dsp.fs64(callback,context)
fs.execute(frame)

# compute spectral response
nfft = 2400
psd = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(frame, nfft))))

# plot frame and its spectrum
fig, ax = plt.subplots(2,figsize=(8,8))
t = np.arange(fg.frame_len)
ax[0].plot(t, np.real(frame), t, np.imag(frame))
ax[0].set_xlabel('Delay [samples]')
ax[0].set_ylabel('Frame Samples')
ax[0].grid(True, zorder=5)
ax[1].plot(np.arange(nfft)/nfft-0.5, psd)
ax[1].set_xlabel('Normalized Frequency [f/F_s]')
ax[1].set_ylabel('Power Spectral Density [dB]')
ax[1].grid(True, zorder=5)

fig, ax = plt.subplots(1,figsize=(8,8))
ax.plot(np.real(context['syms']),np.imag(context['syms']),'x')
ax.set_xlabel('real')
ax.set_ylabel('imag')
plt.xlim((-1.2,1.2))
plt.ylim((-1.2,1.2))
ax.grid(True)

if not args.nodisplay:
    plt.show()


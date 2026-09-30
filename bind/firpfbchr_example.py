#!/usr/bin/env python3
'''demonstrate rational rate channelizers'''
import argparse
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

# parse command-line arguments
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# options
channels    = 8
decim       = 5
m           = 5
As          = 60
num_symbols = 80

# derived values
num_samples = num_symbols * decim
delay       = 50
n           = num_samples - delay # 'usable' number of samples

# generate signal (chirp)
buf_0 = np.zeros((num_samples,), dtype=np.csingle)
buf_0[:n] = np.hanning(n) * np.exp(-0.2j*np.arange(n) + 0.2j/n*np.arange(n)**2)

# create channelizer
q = dsp.firpfbchr(channels, decim, m, As)

# compute channelized output
buf_1 = q.execute(buf_0)
print('input', buf_0.shape)
print('output', buf_1.shape)

# plot results
t = np.arange(num_samples)
fig, ax = plt.subplots(1,figsize=(10,5))
ax.plot(t, buf_0.real, t, buf_0.imag)
ax.set(xlabel='Time [samples]',ylabel='Input Chirp')
ax.grid(True, zorder=5)

# plot output time series for each channel
tc = np.arange(num_symbols)
fig, ax = plt.subplots(channels,1,figsize=(8,8))
for i in range(channels):
    ax[i].plot(tc, buf_1[:,i].real, tc, buf_1[:,i].imag)
    ax[i].grid(True, zorder=5)
    ax[i].set(ylabel='Ch. %u' % (i), ylim=(-decim/2,decim/2))
    if i < channels-1:
        ax[i].set(xticklabels=[]) 
    else:
        ax[i].set(xlabel='Time [samples,decimated]')

if not args.nodisplay:
    plt.show()


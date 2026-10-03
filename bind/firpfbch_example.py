#!/usr/bin/env python3
'''demonstrate maximally-decimated channelizer'''
import argparse
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

# parse command-line arguments
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# options
num_channels= 8
m           = 5
As          = 60
num_symbols = 80

# derived values
num_samples = num_symbols * num_channels
delay       = 50
n           = num_samples - delay # 'usable' number of samples

# generate signal (chirp)
buf_0 = np.zeros((num_samples,), dtype=np.csingle)
buf_0[:n] = np.hanning(n) * np.exp(-0.7j*np.arange(n) + 0.7j/n*np.arange(n)**2)

# create analysis channelizer
q = dsp.firpfbcha(num_channels, m, As)

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
fig, ax = plt.subplots(num_channels,1,figsize=(8,8))
for i in range(num_channels):
    ax[i].plot(tc, buf_1[:,i].real, tc, buf_1[:,i].imag)
    ax[i].grid(True, zorder=5)
    ax[i].set(ylabel='Ch. %u' % (i))
    if i < num_channels-1:
        ax[i].set(xticklabels=[]) 
    else:
        ax[i].set(xlabel='Time [samples,decimated]')

if not args.nodisplay:
    plt.show()


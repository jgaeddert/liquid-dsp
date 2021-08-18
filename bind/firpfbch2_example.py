#!/usr/bin/env python3
'''demonstrate (almost) perfect reconstruction channelizers'''
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

# options
M, m, As, num_symbols = 120, 2, 60., 20

# derived values
M2          = M // 2
num_samples = num_symbols * M2
delay       = 2*M*m - M2 + 1  # end-to-end delay
n           = num_samples - delay # 'usable' number of samples

# generate signal (chirp)
buf_0 = np.zeros((num_samples,), dtype=np.csingle)
buf_1 = np.zeros((num_samples,), dtype=np.csingle)
buf_0[:n] = np.hamming(n) * np.exp(-0.2j*np.arange(n) + 0.2j/n*np.arange(n)**2)

# create channelizers
qa = dsp.firpfbch2(0, M, m, As)
qs = dsp.firpfbch2(1, M, m, As)
psd = np.zeros((M,),dtype=np.single)

for i in range(num_symbols):
    # run analysis: convert M/2 time samples into M freq samples
    tmp = qa.execute(buf_0[(i*M2):((i+1)*M2)])
    # accumulate PSD estimate
    psd += np.abs(tmp)**2
    # run synthesis: convert M freq samples into M/2 time samples
    buf_1[(i*M2):((i+1)*M2)] = qs.execute(tmp)

# plot results
t = np.arange(num_samples)
f = np.arange(M)/M - 0.5
fig, (ax0, ax1, ax2) = plt.subplots(3,figsize=(8,8))
ax0.plot(t, np.real(buf_0), t, np.imag(buf_0))
ax0.set_ylabel('Input')
ax1.plot(t, np.real(buf_1), t, np.imag(buf_1))
ax1.set_ylabel('Output')
ax2.plot(f, 20*np.log10(np.fft.fftshift(psd)))
ax2.set_ylabel('PSD [dB]')
ax2.set(xlim=(-0.5,0.5))
for ax in (ax0,ax1,ax2):
    ax.grid(True, zorder=5)
plt.show()


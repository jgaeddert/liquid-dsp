#!/usr/bin/env python3
'''demonstrate symbol stream tracking'''
import argparse
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

num_samples = 4e6   # total samples to observe
n           = 4000  # block size
k           = 3     # samples per symbol

# create generator
gen  = dsp.symstreamr(ms="qpsk", bw=1/k)
filt = dsp.firfilt(np.array((1,0,0,-0.4),dtype=np.single))
sync = dsp.symtrack(ms="qpsk", k=k)
print(sync)

# create power spectral density estimator
psd = dsp.spgram(nfft=600,wlen=400,delay=10)

while psd.num_samples_total < num_samples:
    # generate original signal stream
    buf = gen.generate(n)
    # add multi-path channel
    buf = filt.execute(buf)
    # add noise
    buf += np.random.randn(2*n).astype(np.single).view(np.csingle)*0.1/np.sqrt(2)
    # run through synchronizer
    syms = sync.execute(buf)
    # update spectrum plot
    psd.execute(buf)

# plot results
fig,(ax0,ax1) = plt.subplots(1,2,figsize=(12,6))

# get spectrum plot and display
Sxx,f = psd.get_psd()
ax0.plot(f,Sxx)
ax0.set_xlabel('Normalized Frequency [f/Fs]')
ax0.set_ylabel('Time [samples]')
ax0.grid(True, zorder=5)
ax0.set(xlim=(-0.5,0.5))
ax0.set_xticks(np.linspace(-0.5,0.5,11))

# show output constellation (last buffer)
ax1.plot(np.real(syms),np.imag(syms),'x')
ax1.set_xlabel('real')
ax1.set_ylabel('imag')
ax1.set(xlim=(-1.2,1.2),ylim=(-1.2,1.2))

if not args.nodisplay:
    plt.show()


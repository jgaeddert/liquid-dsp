#!/usr/bin/env python3
'''demonstrate spectral periodogram estimation object'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

def noise(n,nstd=0.1):
    return nstd*np.random.randn(2*n).astype(np.single).view(np.csingle)*0.70711

num_samples = 240e3

psd = dsp.spgram(nfft=600,wlen=400,delay=10)

filt = dsp.firfilt("lowpass", n=151, fc=0.10, scale=0.14)

buf_len = 2400
while psd.num_samples_total < num_samples:
    buf = filt.execute(noise(buf_len,1e-2)) + noise(buf_len,1e-3)

    # run some noise through
    psd.execute(buf)

# get spectrum plot and display
print(psd)
Sxx,f = psd.get_psd(fs=20e6, fc=460e6)
print('Sxx:',Sxx.shape,'f:',f.shape)

fig,ax = plt.subplots(1,figsize=(8,8))
ax.plot(f*1e-6,Sxx)
ax.set_xlabel('Frequency [MHz]')
ax.set_ylabel('Time [samples]')
ax.grid(True, zorder=5)
ax.set(xlim=(450,470), ylim=(-65,-30))
ax.set_xticks(np.linspace(450,470,11))
if not args.nodisplay:
    plt.show()


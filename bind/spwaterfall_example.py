#!/usr/bin/env python3
'''demonstrate spectral periodogram waterfall estimation object'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

num_samples = 20e3

def noise(n,nstd=0.1):
    return nstd*np.random.randn(2*n).astype(np.single).view(np.csingle)*0.70711

# generate frame generator
fg = dsp.fg64()
n  = fg.frame_len

# generate random signals in noise
psd   = dsp.spwaterfall(nfft=600,time=800,wlen=400,delay=1)
while psd.num_samples_total < num_samples:
    # run some noise through
    psd.execute(noise(n))

    # generate frame with random payload
    psd.execute(fg.execute() + noise(n))

    # run some noise through
    psd.execute(noise(n))

# get spectrum plot and display
print(psd)
Sxx,t,f = psd.get_psd(fs=20e6, fc=460e6)
print('Sxx:',Sxx.shape,'t:',t.shape,'f:',f.shape)

fig,ax = plt.subplots(1,figsize=(8,8))
ax.pcolormesh(f*1e-6,t*1e3,Sxx.T,shading='auto')
ax.set_xlabel('Frequency [MHz]')
ax.set_ylabel('Time [ms]')
if not args.nodisplay:
    plt.show()


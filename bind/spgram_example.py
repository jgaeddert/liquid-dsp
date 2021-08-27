#!/usr/bin/env python3
import sys
sys.path.extend(['.','..'])
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

def noise(n,nstd=0.1):
    return nstd*np.random.randn(2*n).astype(np.single).view(np.csingle)*0.70711

num_samples = 240e3

psd = dsp.spgram(nfft=600,wlen=400,delay=10)

filt = dsp.firfilt("lowpass", n=151, fc=0.07, scale=0.14)

buf_len = 2400
while psd.num_samples_total < num_samples:
    buf = filt.execute(noise(buf_len,1e-2)) + noise(buf_len,1e-3)

    # run some noise through
    psd.execute(buf)

# get spectrum plot and display
print(psd)
Sxx,f = psd.get_psd()
print('Sxx:',Sxx.shape,'f:',f.shape)

fix,ax = plt.subplots(1,figsize=(8,8))
ax.plot(f,Sxx)
ax.set_xlabel('Normalized Frequency [f/Fs]')
ax.set_ylabel('Time [samples]')
ax.grid(True, zorder=5)
ax.set(xlim=(-0.5,0.5), ylim=(-65,-30))
plt.show()


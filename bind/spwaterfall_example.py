#!/usr/bin/env python3
import sys
sys.path.extend(['.','..'])
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

num_samples = 20e3

def noise(n,nstd=0.1):
    return nstd*np.random.randn(2*n).astype(np.single).view(np.csingle)*0.70711

# generate frame generator
fg = dsp.fg64()
n  = fg.get_frame_length()

# generate random signals in noise
psd   = dsp.spwaterfall(nfft=600,time=800,wlen=400,delay=1)
frame = np.zeros((n,), dtype=np.csingle)
while psd.num_samples_total < num_samples:
    # run some noise through
    psd.execute(noise(n))

    # generate frame with random payload
    fg.execute(frame)
    psd.execute(frame + noise(n))

    # run some noise through
    psd.execute(noise(n))

# get spectrum plot and display
print(psd)
Sxx,t,f = psd.get_psd()
print('Sxx:',Sxx.shape,'t:',t.shape,'f:',f.shape)

fix,ax = plt.subplots(1,figsize=(8,8))
ax.pcolormesh(f,t,Sxx.T,shading='auto')
plt.show()


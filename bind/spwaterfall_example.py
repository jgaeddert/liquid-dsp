#!/usr/bin/env python3
import sys
sys.path.extend(['.','..'])
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

num_samples = 20e3

def add_noise(v,nstd=0.1):
    return v + nstd*np.random.randn(2*len(v)).astype(np.single).view(np.csingle)*0.7071067811865476

# generate frame generator
fg = dsp.fg64()
n  = fg.get_frame_length()

# generate random signals in noise
psd   = dsp.spwaterfall(nfft=600,time=800,wlen=400,delay=1)
frame = np.zeros((n,), dtype=np.csingle)
while psd.num_samples_total < num_samples:
    header  = np.random.randint(256, size= 8)
    payload = np.random.randint(256, size=64)
    fg.execute(header, payload, frame)

    psd.execute(add_noise(frame))

    # add some space in between
    for i in range(4):
        frame *= 0
        psd.execute(add_noise(frame))

# get spectrum plot and display
print(psd)
print(psd.nfft, psd.time, psd.window_len, psd.delay, psd.wtype)
Sxx,t,f = psd.get_psd()
print(Sxx.shape,t.shape,f.shape)

fix,ax = plt.subplots(1,figsize=(8,8))
ax.pcolormesh(f,t,Sxx.T,shading='auto')
plt.show()


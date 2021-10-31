#!/usr/bin/env python3
'''test ofdmflexframe detection using class with callback function'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

def mycallback(context,header,payload,stats):
    print('frame detected!')
    print(payload)

# generate a frame
fg      = dsp.ofdmflexframetx(M=1024,cp=16,taper=12)
fg.assemble(payload=np.arange(200,dtype=np.uint8))
print(fg)

# create frame synchronizer
fs = dsp.ofdmflexframerx(M=1024, cp=16, taper=12, callback=mycallback)
print(fg)

# compute spectral response
psd = dsp.spgram(nfft=2400)

# run in blocks
block_len = 200
while fg.assembled:
    buf = fg.generate(block_len)
    fs.execute(buf)
    psd.execute(buf)

# plot frame and its spectrum
fig, ax = plt.subplots(1,figsize=(10,6))
S, f = psd.get_psd()
ax.plot(f, S)
ax.set_xlabel('Normalized Frequency [f/F_s]')
ax.set_ylabel('Power Spectral Density [dB]')
ax.set(xlim=(-0.5,0.5))
ax.grid(True, zorder=5)
if not args.nodisplay:
    plt.show()


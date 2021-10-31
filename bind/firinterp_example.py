#!/usr/bin/env python3
'''demonstrate finite impulse response interpolator object'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# options
M, m, As, num_symbols = 4, 6, 60., 40

# design from prototype
interp = dsp.firinterp(M, m, As)
print(interp)

# generate random symbols and interpolate
modmap  = np.array((1+1j,1-1j,-1+1j,-1-1j))
symbols = np.random.choice(modmap,num_symbols).astype(np.csingle)
samples = interp.execute(symbols)

# plot results
t0 = np.arange(num_symbols)
t1 = np.arange(num_symbols*M)/M - m

# plot impulse and spectral responses
fig, (axi, axq) = plt.subplots(2,figsize=(8,8))
for ax,func,label in ((axi,np.real,'Real'),(axq,np.imag,'Imag')):
    ax.plot(t1, func(samples))
    ax.plot(t0, func(symbols), 'o')
    ax.set_xlabel('Time [samples]')
    ax.set_ylabel(label)
    ax.grid(True, zorder=5)
if not args.nodisplay:
    plt.show()


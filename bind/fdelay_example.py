#!/usr/bin/env python3
'''demonstrate fractional sample delay object'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# create delay object
fdelay = dsp.fdelay(nmax=400)
fdelay.delay = 17.73
print(fdelay)

# run sample through and get impulse response
n = 50
x = np.zeros(n,dtype=np.csingle)
x[0] = 1
y = fdelay.execute(x)

# plot results
fig, (ax1, ax2) = plt.subplots(2,figsize=(8,8))
ax1.plot(np.arange(n),np.real(x),'-o')
ax1.set_xlabel('Time [samples]')
ax1.set_ylabel('Input')
ax1.set(xlim=(-1,11),ylim=(-0.2,1.1))
ax1.grid(True, zorder=5)
ax2.plot(np.arange(n) - fdelay.m, np.real(y),'-o',
         [fdelay.delay, fdelay.delay],[-0.25,1.25],'--r')
ax2.set_xlabel('Time [samples]')
ax2.set_ylabel('Output')
ax2.legend(('Output','Target Delay'))
ax2.set(xlim=(fdelay.delay-6, fdelay.delay+6),ylim=(-0.2,1.1))
ax2.grid(True, zorder=5)
if not args.nodisplay:
    plt.show()


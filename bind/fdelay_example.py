#!/usr/bin/env python3
import sys
sys.path.extend(['.','..'])
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

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
plt.show()


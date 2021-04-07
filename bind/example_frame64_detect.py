#!/usr/bin/env python3
'''test framesync64 detection using class with callback function'''
import sys
sys.path.append('.')
import numpy as np
import liquid as dsp
import matplotlib.pyplot as plt

class performance:
    def __init__(self):
        self.fg = dsp.fg64()
        self.fs = dsp.fs64()

    def run(self, snr, num_trials):
        n     = self.fg.get_frame_length()
        frame = np.zeros((n,), dtype=np.csingle)
        nstd  = 10.0**(-snr/20)
        self.fs.reset()
        self.fs.reset_framedatastats()
        for i in range(num_trials):
            # generate frame with random header, payload
            self.fg.execute(frame)
            frame += np.random.randn(2*n).astype(np.single).view(np.csingle)*nstd * np.sqrt(0.5)
            self.fs.execute(frame)

        return self.fs.framedatastats

# sweep performance over snr
sim         = performance()
num_trials  = 1200
snr         = np.linspace(-9,6,16)
num_detects = np.zeros(len(snr))
num_valid   = np.zeros(len(snr))
for i,s in enumerate(snr):
    num_detects[i], num_valid[i], payloads, _bytes = sim.run(s, num_trials)
    print('%8.1f %6u %6u' % (s, num_detects[i], num_valid[i]))

# plot results
fig, ax = plt.subplots(1,figsize=(8,8))
ax.semilogy(snr, 1-num_detects/num_trials,'-o', snr, 1-num_valid/num_trials,'-o')
ax.set_xlabel('SNR [dB]')
ax.set_ylabel('Probability of Error')
ax.legend(('Detection','Valid Payload'))
ax.grid(True, zorder=5)
plt.show()

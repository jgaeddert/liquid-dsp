#!/usr/bin/env python3
'''test ofdmflexframe detection using class with callback function'''
import argparse, sys
sys.path.extend(['.','..'])
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

class performance:
    def __init__(self,M=1024,cp=16,taper=12):
        self.fg = dsp.ofdmflexframetx(M,cp,taper)
        self.fs = dsp.ofdmflexframerx(M,cp,taper)

    def run(self, snr, num_trials):
        buf_len = 200
        nstd  = 10.0**(-snr/20)
        self.fs.reset()
        self.fs.reset_framedatastats()
        for i in range(num_trials):
            # generate frame with random header, payload
            self.fg.assemble(payload=np.zeros(1200,dtype=np.uint8))
            while self.fg.assembled:
                buf = self.fg.generate(buf_len)
                buf += np.random.randn(2*buf_len).astype(np.single).view(np.csingle)*nstd * np.sqrt(0.5)
                self.fs.execute(buf)

        return self.fs.framedatastats

# sweep performance over snr
sim         = performance()
num_trials  = 1000
snr         = np.linspace(-5,10,16)
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
if not args.nodisplay:
    plt.show()

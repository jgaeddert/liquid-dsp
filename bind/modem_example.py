#!/usr/bin/env python3
'''demonstrate linear modulation and demodulation'''
import argparse
import liquid as dsp, numpy as np, matplotlib.pyplot as plt
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

# set up options and create modem
modscheme   = "qpsk"
SNRdB       = 30.0
num_symbols = 9600
#modem       = dsp.modem(modscheme)
constellation = np.exp((2j+0.15)*np.pi*np.arange(32)/24)
modem = dsp.modem(constellation)
M           = 1 << modem.bits_per_symbol
print(modem)

# modulate some random data
data = np.random.randint(M, size=num_symbols,  dtype=np.uint8)
#syms = modem.modulate(data)
syms = np.array(tuple(modem.modulate(i) for i in data),dtype=np.csingle)

# add some noise for fun
nstd = 10**(-SNRdB/20)
syms += np.random.randn(2*num_symbols).astype(np.single).view(np.csingle)*nstd * np.sqrt(0.5)

# demodulate and count errors
demod = np.array(tuple(modem.demodulate(s) for s in syms))
errors = sum( demod != data )

# plot constellation
fig, ax = plt.subplots(1,figsize=(8,8))
ax.plot(np.real(syms),np.imag(syms),'x')
ax.set_xlabel('real')
ax.set_ylabel('imag')
ax.set(xlim=(-1.5,1.5), ylim=(-1.5,1.5))
ax.grid(True)
ax.set_title('%s, SNR: %.1f dB, errors: %u/%u' % (modem.scheme, SNRdB, errors, num_symbols))

if not args.nodisplay:
    plt.show()


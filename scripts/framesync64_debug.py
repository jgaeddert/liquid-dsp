#!/usr/bin/env python3
'''plot frame sync debug files'''
import argparse, sys
import numpy as np, matplotlib.pyplot as plt

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('-nodisplay', action='store_true', help='disable display')
args = p.parse_args()

filename = 'framesync64_12345678.dat'

# open file and read values
fid         = open(filename,'rb')
buf         = np.fromfile(fid, count=1440, dtype=np.csingle)
tau_hat     = np.fromfile(fid, count=   1, dtype=np.single)
dphi_hat    = np.fromfile(fid, count=   1, dtype=np.single)
phi_hat     = np.fromfile(fid, count=   1, dtype=np.single)
gamma_hat   = np.fromfile(fid, count=   1, dtype=np.single)
evm         = np.fromfile(fid, count=   1, dtype=np.single)
payload_rx  = np.fromfile(fid, count= 630, dtype=np.csingle)
payload_sym = np.fromfile(fid, count= 600, dtype=np.csingle)
payload_dec = np.fromfile(fid, count=  72, dtype=np.int8)

# compute filter response in dB
nfft = 2400
f = np.arange(nfft)/nfft-0.5
psd = 20*np.log10(np.abs(np.fft.fftshift(np.fft.fft(buf, nfft))))

# plot impulse and spectral responses
fig, _ax = plt.subplots(2,2,figsize=(8,8))
ax = _ax.flatten()
t = np.arange(len(buf))
ax[0].plot(t,np.real(buf), t,np.imag(buf))
ax[0].set_title('Raw I/Q Samples')
ax[1].plot(f,psd)
ax[1].set(xlim=(-0.5,0.5))
ax[1].set_title('Power Spectral Density')
ax[2].plot(np.real(payload_rx),  np.imag(payload_rx),  '.')
ax[2].set_title('RX Payload Syms')
ax[3].plot(np.real(payload_sym), np.imag(payload_sym), '.')
ax[3].set_title('Synchronized Payload Syms')
for _ax in ax[2:]:
    _ax.set(xlim=(-1.3,1.3), ylim=(-1.3,1.3))
for _ax in ax:
    _ax.grid(True)
fig.suptitle('frame64, tau:%.6f, dphi:%.6f, phi:%.6f, rssi:%.3f dB, evm:%.3f' % \
    (tau_hat, dphi_hat, phi_hat, 20*np.log10(gamma_hat), evm))
if not args.nodisplay:
    plt.show()


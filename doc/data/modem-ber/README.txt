#
# liquid-dsp modem bit error rate data
#
This directory contains bit error rate (BER) data for the many different
modulation schemes available in liquid, including QAM, PSK, APSK, ASK,
and many arbitrary schemes. Each file in this directory contains
simulation data run over many trials by stepping the signal-to-noise
ratio (SNR) and accumulating the bit error rate over many packets. The
header of each file gives some useful information about each simulation
including the modulation scheme, modulation depth (bits/symbol), and
the stopping criteria for the simulation. Each simulation is run for a
minimum number of bit trials (on the order of 100,000) and a minimum
number of bit errors (usually around 500). The simulation will stop
when any one of the following criteria is met:

  1. the maximum SNR is reached
  2. the maximum number of bit trials is reached (millions)
  3. the minimum BER limit is reached (usually 1e-6)

The data are organized by SNR in rows with the explicit number of bit
trials, errors, etc. listed for each simulation. Also included are the
effective Eb/N0 value (bit energy to noise ratio), packet errors, and
packet error probability.

Also included is "modem_snr.dat" which includes the required SNR (and
Eb/N0) for each modulation scheme to reach a bit error rate of 10^-5.

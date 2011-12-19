=========================================================================

  doc/data/

=========================================================================

This directory contains some useful data relevant to signal processing.

doc/data/fec-ber
    This directory contains bit error rate (BER) simulations for all the 
    forward error-correction (FEC) schemes with both soft- and 
    hard-decision BPSK demodulation. Simulations were conducted with a 
    minimum of 1,000 bit errors and a minimum of 4,000,000 bit trials by 
    stepping the signal-to-noise ratio (SNR) until a BER threshold was 
    met.

doc/data/modem-ber
    This directory contains bit error rate (BER) simulations for all the 
    linear modulation schemes available in liquid-dsp (QAM, PSK, etc.).
    Simulations were conducted with a minimum of 700 bit errors and a 
    minimum of 320,000 bit trials by stepping the signal-to-noise ratio 
    (SNR) until a BER threshold was met. Also included is 
    "modem_snr.dat" which includes the required SNR and Eb/N0 for each 
    modulation scheme to reach a BER of 10^-5.

doc/data/modem-phase-error
    This directory contains phase error estimation data for all the 
    modulation schemes available in liquid-dsp. 

doc/data/msequence
    This directory contains all maximal-length sequence generator
    polynomials of length m=2 (n=3) through m=15 (n=32767).  Each file
    is unique to the size of the shift register (m) and lists each
    generator polynomial's hexadecimal, octal, and binary
    representation.  The values are suitable for use directly in
    liquid's msequence initialization methods and do not need shifting,
    reordering, or additional manipulation.  For more information on
    m-sequence generation, refer to the section on the 'sequence' module
    in the guide.

doc/data/ofdmflexframe
    This directory contains the performance curves for the ofdmflexframe 
    synchronizer in the presence of additive Gauss white noise.


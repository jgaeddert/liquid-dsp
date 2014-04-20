
GMSK frame structure
====================

The gmskframegen and gmskframesync object facilite sending packets over
the air with Gauss minimum-shift keying (GMSK). This modulation scheme
is a special case of binary frequency-shift keying (BFSK) which uses
the minimum frequency separation between tones and uses Gauss pulse
shaping to reduce its occupied bandwidth. Its spectral efficiency is
nominally 1 b/s/Hz, and the framing structure allows any length payload
up to 65,535 bytes with two levels of forward error-correction coding.

    signal
      ^
      |     .+---------+-----+--------+----//---+.
      |    / |         |     |        |         | \
      |  r/u | phasing | p/n | header | payload | r\d
      |__/   |         |     |        |         |   \__
      +-------------------------------------------------> time

section             # bits          Description
ramp up (r/u)       8               phasing (1010...) with ramp up
phasing pattern     40              10101010...
p/n sequence        64              BPSK P/N sequence (m-sequence, g=0x005b)
header              208             includes payload length, fec schemes,
                                    crc, and 8 bytes of user-defined data,
                                    all encoded with the Hamming(12,8)
                                    block code
payload             [variable]      data payload using arbitrary forward
                                    error correction
ramp down (r\d)     8               phasing (1010...) with ramp down

The approximate spectral efficiency of the frame is determined by the
following equation:

       payload*8/rate         1
  ---------------------- * --------  bits/second/Hertz
   328 + payload*8/rate     1 + BT

where 'payload' is the length of the payload (in bytes), 'rate' is
the rate of the pair of forward error-correction codes, and 'BT' is
the bandwidth-time constant of the GMSK signaling. Note that 328 is
the number of bits of overhead associated with the frame (e.g. sending
the header, etc.)

For example, if the payload is 1024 bytes encoded with a half-rate
convolutional code, and modulated with a bandwdith-time constant of
BT=0.5, the approximate spectral efficiency is

     1024*8/(1/2)         1
  ------------------ * --------- = 0.654 b/s/Hz
  328 + 1024*8/(1/2)    1 + 0.5


======================================
 Flexible frame structure
======================================

signal
  ^
  |     .+---------+-----+--------+----//---+.
  |    / |         |     |        |         | \
  |  r/u | phasing | p/n | header | payload | r\d
  |__/   |         |     |        |         |   \__
  +-------------------------------------------------> time

new header options:
    header      check   fec1        bytes   bpsk syms   1% FER SNR
    [[14 + 5 + crc16] + h128 ]  >   32  >   256         3.0 dB /  2.9 dB soft
    [[14 + 5 + crc24] + g2412]  >   45  >   360          -  dB / -
    [[14 + 5 + crc24] + v27  ]  >   46  >   368          -  dB / -0.3 dB soft
    [[14 + 5 + crc24] + v39  ]  >   69  >   552          -  dB / -1.1 dB soft
    
    concatenated code:
    14 + 6 + 4 = 24 + secded7264 > 27 + h84 > 54
    [[[14 + 6 + crc32] + secded7264] + h84] > 54  > 432  -  dB /  0.2 dB soft

section             # syms          Description
ramp up (r/u)       0+              phasing (1010...) with ramp up
phasing pattern     0+              10101010...
p/n sequence        64              BPSK P/N sequence (m-sequence, g=0x005b)
header              256
                    # bytes
    crc             2               cyclic redundancy check
    packet fec      2               payload fec (inner/outer), check
    mod scheme/bps  1               modulation scheme, depth
    payload_len     2               # bytes in payload (up to 65535)
    user data       14              space for user-defined data
    total           19
    encoded         32              Hamming(12,8)
payload             [variable]      data payload using arbitrary linear
                                    modulation
ramp down (r\d)     0+              phasing (1010...) with ramp down

The header includes a mandatory 32-bit crc with 1/2 rate forward error-
correction code totaling 256 bits (32 bytes) of encoded data.


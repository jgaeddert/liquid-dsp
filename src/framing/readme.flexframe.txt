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


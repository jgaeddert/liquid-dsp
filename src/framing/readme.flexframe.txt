======================================
 Flexible frame structure
======================================

signal
  ^
  |     .+---------+-----+--------+---------+.
  |    / |         |     |        |         | \
  |  r/u | phasing | p/n | header | payload | r\d
  |__/   |         |     |        |         |   \__
  +-------------------------------------------------> time

section             # syms          Description
ramp up (r/u)       0+              phasing (1010...) with ramp up
phasing pattern     0+              10101010...
p/n sequence        64              BPSK P/N sequence
header              256
                    # bytes
    crc             4               cyclic redundancy check
    mod scheme/bps  1               modulation scheme, depth
    payload_len     2               # bytes in payload (up to 65536)
    user data       8               space for user-defined data
    total           15
    encoded         32              h74
payload             [variable]      data payload using arbitrary linear
                                    modulation
ramp down (r\d)     0+              phasing (1010...) with ramp down

The header includes a mandatory 32-bit crc with 1/2 rate forward error-
correction code totaling 256 bits (32 bytes) of encoded data.


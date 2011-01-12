======================================
 frame-64 description
======================================

signal
  ^
  |     .+-------+-------+--------+----//---+.
  |    / |       |       |        |         | \
  |  r/u |phasing|  p/n  | header | payload | r\d
  |__/   |       |       |        |         |   \__
  +-------------------------------------------------> time

section             length          Description
ramp-up             16 symbols      ramp/up phasing pattern
phasing pattern     64 symbols      BPSK phasing pattern (+1,-1...)
p/n sequence        64 symbols      BPSK p/n sequence (m-sequence)
header              84 symbols      12-byte packet header, 16-bit crc,
                                    encoded with the Hamming(12,8) FEC,
                                    modulated with QPSK
payload             396 symbols     64-byte packet header, 16-bit crc,
                                    encoded with the Hamming(12,8) FEC,
                                    modulated with QPSK
ramp-dn             16 symbols      ramp down

total:              640 symbols

Both the header and payload include a mandatory 16-bit crc with the
2/3-rate Hamming(12,8) forward error-correction code.


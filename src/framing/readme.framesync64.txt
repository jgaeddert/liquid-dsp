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
ramp-up             12 symbols      ramp/up phasing pattern
phasing pattern     64 symbols      BPSK phasing pattern (+1,-1...)
p/n sequence        64 symbols      BPSK p/n sequence (m-sequence, g=0x0043)
header              84 symbols      12-byte packet header, 16-bit crc,
                                    encoded with the Hamming(12,8) FEC,
                                    modulated with QPSK
payload             396 symbols     64-byte packet header, 16-bit crc,
                                    encoded with the Hamming(12,8) FEC,
                                    modulated with QPSK
ramp-dn             12 symbols      ramp down
settling            8  symbols      settling time for interpolator

total:              640 symbols

interpolated:       1280 samples    interpolated using half-rate square-
                                    root Nyquist pulse-shaping filter

Both the header and payload include a mandatory 16-bit crc with the
2/3-rate Hamming(12,8) forward error-correction code.


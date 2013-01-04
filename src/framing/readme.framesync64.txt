
frame64 description
===================

    signal
      ^
      |     .+-------+---------------------------+.
      |    / |       |                           | \
      |  r/u |  p/n  |           payload         | r\d
      |__/   |       |                           |   \__
      +--------------------------------------------------> time

    section             length          Description
    ramp-up             3 symbols       filter ramp/up
    p/n sequence        64 symbols      BPSK p/n sequence (m-sequence, g=0x0043)
    payload             552 symbols     64-byte packet payload, 32-bit crc,
                                        encoded with the Golay(24,12) FEC,
                                        (138 bytes encoded)
                                        modulated with QPSK
    ramp-dn             3 symbols       filter ramp\down
    total:              622 symbols

    interpolated:       1244 samples    interpolated using half-rate square-
                                        root Nyquist pulse-shaping filter at
                                        2 samples/symbol

## Revised ##

    signal
      ^
      |     .+-------+---------------------------+.
      |    / |       |                           | \
      |  r/u |  p/n  |    header/payload         | r\d
      |__/   |       |                           |   \__
      +--------------------------------------------------> time

    138 -> 150
    552 -> 600

  * payload:    [[[ 8-byte header        ][  64-byte payload    ]+crc24] +golay(24,12)] -> 150 -> 600 -> 670
    payload:    [[[ 8-byte header + crc24][  64-byte payload    ]+crc24] +golay(24,12)] -> 156 -> 624 -> 694

    section             length          Description
    ramp-up             3 symbols       filter ramp/up
    p/n sequence        64 symbols      BPSK p/n sequence (m-sequence, g=0x0043)
    header/payload      600 symbols     64-byte packet payload with 8-byte header,
                                        24-bit crc, encoded with the Golay(24,12) FEC,
                                        (150 bytes encoded)
                                        modulated with QPSK
    ramp-dn             3 symbols       filter ramp\down
    total:              670 symbols

    interpolated:       1340 samples    interpolated using half-rate square-
                                        root Nyquist pulse-shaping filter at
                                        2 samples/symbol

## Description ##



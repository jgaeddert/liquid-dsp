
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
    payload             552 symbols     64-byte packet header, 32-bit crc,
                                        encoded with the Golay(24,12) FEC,
                                        modulated with QPSK
    ramp-dn             3 symbols       filter ramp\down
    total:              622 symbols

    interpolated:       1244 samples    interpolated using half-rate square-
                                        root Nyquist pulse-shaping filter at
                                        2 samples/symbol


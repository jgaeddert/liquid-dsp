
frame64 description
-------------------

## Structure ##

    signal
      ^
      |     .+-------+---------------------------+.
      |    / |       |                           | \
      |  r/u |  p/n  |    header/payload         | r\d
      |__/   |       |                           |   \__
      +--------------------------------------------------> time

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

The frame structure is simply a 64-bit pseudo-random p/n sequence
modulated with BPSK followed by a QPSK payload. The payload has a fixed
length of 64 bytes (with an 8-byte 'header'), a 24-bit cyclic redudancy
check, and encoded with a half-rate Golay(24,12) block code. The entire
frame uses a square-root Nyquist pulse shape with an excess bandwidth
of 0.5 and interpolated at two samples per symbol. The resulting frame
length is exactly 1340 samples.


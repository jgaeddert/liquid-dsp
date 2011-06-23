======================================
 OFDM flexible framing structure
======================================

signal
  ^
  |   +----+----+----+----+----+-//-+----+----+-//-+----+
  |   |    |    |    |    |              |              |
  |   | S0 | S0 | .. | S1 |  ..header..  | ..payload..  |
  |___|    |    |    |    |              |              |__
  +---------------------------------------------------------> time


             spectral null     ^   pilot subcarrier
                  /            |    /
     *----*----*    *----*----+----*----*----*----*----*
     |         |    |         |                        |
     |         |    |         |      ...               |
  ___/         \____/         |                        \___
  +---------------------------+----------------------------+ freq
-Fs/2                         0                         +Fs/2


OFDM frame gen/sync parameters (must match on both TX and RX side):
    N       :   Total number of subcarriers
    N0      :   number of null subcarriers
    Np      :   number of pilot subcarriers
    Nd      :   number of data subcarriers
    p*      :   subcarrier allocation

    cplen   :   cyclic prefix length (samples)
    num_S0  :   number of S0 symbols
    num_S1  :   number of S1 symbols (must be 1, non-negotiable)

Notes:
    * The length of each data symbol is equal to the number of
      subcarriers plus the cyclic prefix length (e.g. if N=64 and
      cplen=16, each data symbol is 80 samples long).
    * The S0 symbols do NOT have a cyclic prefix; this is
      necessary to preserve continuity between symbols. Unlike 802.11a
      (and derivative protocols) in which a fixed cyclic prefix has
      been designed into the system, the flexibility of liquid-dsp's
      OFDM framing structure requires that no prefix is added to
      maintain temporal continuity.

section     # samples       Description/purpose

    S0      N*num_S0        Initial preamble for coarse carrier
                            frequency, phase, and timing offset
                            estimates

    S1      N+cplen         Secondary preamble for timing disambiguation
                            and equalization

    header  [variable]      Framing descriptor
                            user-defined: 8 bytes
                            internal    : 6 bytes
                            encoded with Hamming(12,8), 16-bit CRC: 24 bytes
                            modulated with QPSK : 96 modem symbols
                            ...

    payload [variable]      user-defined length...

TODO:
    * Iteratively improve initial carrier frequency, phase, timing
      offset by averaging over S0 symbols (until S1 symbol is found)
    * Make cyclic prefix length discoverable on the receiver by
      observing phase slope of S1


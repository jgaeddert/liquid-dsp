=========================================================================

  OFDM framing structure

=========================================================================

signal           preamble                       payload
^       |<-------------------->|<---------------------------------->|
|       |                      |                                    |
|       +----+----+----+--+----+--+----+--+----+--+----+-//-+--+----+
|       |    |    |    |//|    |//|    |//|    |//|    |    |//|    |
|       |    |    |    |//|    |//|    |//|    |//|    |    |//|    |
|       | S0 | S0 | .. |//| S1 |//| P0 |//| P1 |//| P2 | .. |//| Px |
|       |    |    |    |//|    |//|    |//|    |//|    |    |//|    |
|_______|    |    |    |//|    |//|    |//|    |//|    |    |//|    |
+----------------------------------------------------------------------->
                                                                       time


               spectral null      ^    pilot subcarrier
                    /             |    /
     *----*----*        *----*----+---*----*----*----*----*----*
     |         |        |         |                            |
     |         |        |         |          ...               |
  ___/         \________/         |                            \___
  +-------------------------------+--------------------------------+ freq
-Fs/2                             0                             +Fs/2


Overview:
    The ofdmframe family of objects (generator and synchronizer)
    realize a simple way to load data onto an OFDM physical layer
    system. OFDM has several benefits over traditional 'narrowband'
    communications systems.
    These objects allow the user to abstractly specify the number of
    subcarriers, their assignment (null/pilot/data), forward
    error-correction and modulation scheme.
    Furthermore, the framing structure includes a provision for a brief
    user-defined header which can be used for source/destination
    address, packet identifier, etc.

Operational description:
    The structure of the frame consists of three main components: the
    preamble, the header, and the payload.
    
  Preamble
    The preamble consists of two types of phasing symbols: the S0 and S1
    sequences. The S0 symbols are necessary for coarse carrier frequency
    and timing offsets while the S1 sequence is used for fine timing
    acquisition and equalizer gain estimation.  The transmitter
    generates multiple S0 symbols (minimally 2, but usually 3 or more)
    and just a single S1 symbol. This aligns the receiver's timing to
    that of the transmitter, signalling the start of the header.

OFDM frame gen/sync parameters (must match on both TX and RX side):
    M       :   Total number of subcarriers
    p*      :   subcarrier allocation
    cplen   :   cyclic prefix length (samples)

    M0      :   number of null subcarriers
    Mp      :   number of pilot subcarriers
    Md      :   number of data subcarriers

    num_S0  :   number of S0 symbols
    num_S1  :   number of S1 symbols (must be 1, non-negotiable)

Notes:
    * The length of each data symbol is equal to the number of
      subcarriers plus the cyclic prefix length (e.g. if N=64 and
      cplen=16, each data symbol is 80 samples long) with the exception
      of the S0 training symbols which are simply the number of
      subcarriers.
    * The S0 symbols do NOT have a cyclic prefix; this is
      necessary to preserve continuity between symbols. Unlike 802.11a
      (and derivative protocols) in which a fixed cyclic prefix has
      been designed into the system, the flexibility of liquid-dsp's
      OFDM framing structure requires that no prefix is added to
      maintain temporal continuity.

Details:
    section num samples     Description/purpose

    S0      M*num_S0        Initial preamble for coarse carrier
                            frequency, phase, and timing offset
                            estimates

    S1      M+cplen         Secondary preamble for timing disambiguation
                            and equalization

    payload M+cplen         payload symbols


=========================================================================
  Synchronizer: Description of Operation
=========================================================================


signal                              FFT input
^                  |<---------------------------------------->|
|                  |                                          |
|       +------------+------------------------------------------+-------
|       |////////////|                                          |//////
|       |//        //|                                          |//
|       |// cyclic //|                  OFDM                    |// ...
|       |// prefix //|                 symbol                   |//
|       |//        //|                                          |//
|_______|////////////|                                          |//////
+------------------+-+------------------------------------------+------->
                   | |                                          |      time
                   | +- true start of symbol                    |
                   |                                            +- start of
                   +- timing backoff (e.g. 2 samples)             next symbol

Talking points:
    * FFT input backoff by one or two samples to ensure no overlap
      with next symbol

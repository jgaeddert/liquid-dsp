============================================
 fbasc : filterbank audio synthesizer codec
============================================

The fbasc audio codec implements an AAC-like compression algorithm, using the
modified discrete cosine transform as a loss-less channelizer.  The resulting
channelized data are then quantized based on their spectral energy levels and
then packed into a frame which the decoder can then interpret. The result is a
lossy encoder (as a result of quantization) whose compression/quality levels
can be easily varied.

                +------+ -->           -->  +-------+
                |      | -->           -->  |       |
   original     |      | -->  M-band   -->  |       |     reconstructed
     time   ->  | MDCT |  .  quantizer  .   | iMDCT | ->      time
    series      |      |  .             .   |       |        series
                |      |  .             .   |       |
                +------+ -->           -->  +-------+

Specifically, fbasc uses sub-band coding to allocate quantization bits to each
channel in order to minimize distortion of the reconstructed signal. Sub-bands
with higher variance (signal 'energy') are assigned more bits.  This is the
heart of the codec, which exploits several components typical of audio signals
and aspects of human hearing and perception:
    1.  The majority of audio signals (including music and voice) have a
        strong time-frequency localization; that is, they only occupy a small
        fraction of audible frequencies for a short duration.  This is
        particularly true for voiced signals (e.g. vowel sounds).
    2.  The human ear (and brain) tends to be quite forgiving of spectral
        compression and often cannot easily distinguish between neighboring
        frequency components.

There are several benefits to using fbasc over other compression algorithms
such as CVSD (see src/audio/readme.cvsd.txt) and auto-regressive models, the
main being that the algorithm is theoretically lossless (i.e. perfect
reconstruction) as the bit rate increases.  As a result, the codec is limited
only by the quantization noise on each channel.

Here are some useful definitions, as used in the fbasc code:
                   __________  __________
                  /   MDCT   \/          \
                 /   window  /\          ...
            ____/       ____/  \____                                time
    frame:  [----s0----][----s1----][----s2----][----s3----] ... --->
                        |          |
                      ->|          |<- symbol (length = M samples)

MDCT - the modified discrete cosine transform is a lapped discrete cosine
    transform which uses a special windowing function to ensure perfect
    reconstruction on its inverse. The transform operates on 2*M time-domain
    samples (overlapped by M) to produce M frequency-domain samples.
    Conversely, the inverse MDCT accepts M frequency-domain samples and
    produces 2*M time-domain samples which are windowed and then overlapped to
    reconstruct the original signal.  For convenience, we may refer to M
    time-domain samples as a 'symbol.'

symbol - one block of M time-domain samples upon which the MDCT operates.

channel - one of the M frequency-domain components as a result of applying the
    MDCT.  This is somewhat equivalent to a discrete Fourier transform 'bin.'
    Note than M is equal to the number of channels in analysis.

frame - a set of MDCT symbols upon which the fbasc codec runs its analysis.
    Because the codec uses time-frequency localization for its encoding, it is
    necessary for the codec to gain enough statistical information about the
    original signal without losing temporal stationarity. The codec typically
    operates on several symbols, however, the exact number depends on the
    application.

Interface
    fbasc_create()
        creates an fbasc encoder/decoder object, allocating memory as
        necessary, and computing internal parameters appropriately.
    fbasc_destroy() 
        destroys an fbasc encoder/decoder object, freeing internally-allocated
        memory.
    fbasc_encode()
        encode a frame of data, storing the header and frame data separately.
        This separation allows the user to use different forward
        error-correction codes (if desired) to protect the header differently
        than the rest of the frame.  It is important to keep the two together,
        however, as the header is a description of how to decode the frame.
    fbasc_decode()
        decodes a frame of data, generating the reconstructed time series.

Useful properties:
    1.  Because of the nature of the MDCT, frames will overlap by M samples
        (one symbol).  This introduces a reconstruction delay of M samples,
        noticeable at the decoder.

The header contains the following data:
    id      name                # bytes
    --      ----------          ---------
    fid     frame id            2
    g0      nominal gain        1
    bk      bit allocation      num_channels / 2
    gk      gain allocation     num_channels / 2
    --      ----------          ---------
            total:              num_channels + 3

Miscellaneous information

Example:

create() parameters:
    num channels    =   64  (samples/symbol)
    samples/frame   =   512
    bytes/frame     =   256
    ---------------------------
derived values:
    symbols/frame   =   [samples/frame] / [samples/symbol]  =   8
    bytes/symbol    =   [bytes/frame]   / [symbols/frame]   =   32

Each symbol must be encoded with an even number of bytes.


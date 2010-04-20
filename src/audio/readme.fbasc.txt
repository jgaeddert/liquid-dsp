============================================
 fbasc : filterbank audio synthesizer codec
============================================

The fbasc audio codec implements an AAC-like compression algorithm, using the
modified discrete cosine transform as a loss-less channelizer.  The resulting
channelized data are then quantized based on their spectral energy levels and
then packed into a frame which the decoder can then interpret. The result is a
lossy encoder (as a result of quantization) whose compression/quality levels
can be easily varied.

How does it work? The codec exploits several components typical of audio
signals and aspects of human hearing:
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
reconstruction) as the bit rate increases.

Here are some useful definitions, as used in the fbasc code:

MDCT - the modified discrete cosine transform is a lapped discrete cosine
    transform which uses a special windowing function to ensure perfect
    reconstruction on its inverse. The transform operates on 2*M time-domain
    samples (overlapped by M) to produce M frequency-domain samples.
    Conversely, the inverse MDCT accepts M frequency-domain samples and
    produces 2*M time-domain samples which are windowed and then overlapped to
    reconstruct the original signal.

channel - one of the M frequency-domain components as a result of applying the
    MDCT.  This is somewhat equivalent to a discrete Fourier transform 'bin.'

symbol - one block of M time-domain samples upon which the MDCT operates.

frame - a set of MDCT symbols upon which the fbasc codec runs its analysis.
    Because the codec uses time-frequency localization for its encoding, it is
    necessary for the codec to gain enough statistical information about the
    original signal without losing temporal stationarity. The codec typically
    operates on several symbols, however, the exact number depends on the
    application.


The header contains the following data:
    id      name                # bytes
    --      ----------          ---------
    fid     frame id            2
    g0      nominal gain        1
    bk      bit allocation      num_channels : [num_channels/2]




liquid-dsp examples
===================

This directory contains all the examples for interfacing the liquid modules.

 * `agc_crcf_example.c`:
    Automatic gain control example demonstrating its transient response.

 * `agc_crcf_qpsk_example.c`:
    Automatic gain control test for data signals with fluctuating signal
    levels.  QPSK modulation introduces periodic random zero-crossings which
    gives instantaneous amplitude levels near zero.  This example tests the
    response of the AGC to these types of signals.

 * `ampmodem_example.c`:
    Tests simple modulation/demodulation of the ampmodem (analog amplitude
    modulator/demodulator) with noise, carrier phase, and carrier frequency
    offsets.

 * `asgramcf_example.c`:
    ASCII spectrogram example for complex inputs. This example demonstrates
    the functionality of the ASCII spectrogram. A sweeping complex sinusoid
    is generated and the resulting spectral periodogram is printed to the
    screen.

 * `asgramf_example.c`:
    ASCII spectrogram example for real-valued input. This example demonstrates
    the functionality of the ASCII spectrogram for real-valued input siganls.
    A cosine signal with time-varying frequency is generated and the resulting
    spectral periodogram is printed to the screen. Because the time signal has
    no complex component, its spectrum is symmetric.

 * `autocorr_cccf_example.c`:
    This example demonstrates the autocorr (auto-correlation) object
    functionality.  A random time-domain sequence is generated which exhibits
    time-domain repetitions (auto-correlation properties), for example:
    abcdabcdabcd....abcd.  The sequence is pushed through the autocorr object,
    and the results are written to an output file. The command-line arguments
    allow the user to experiment with the sequence length, number of sequence
    repetitions, and properties of the auto-correlator, as well as signal-to-
    noise ratio.

 * `bpacketsync_example.c`:

 * `bpresync_example.c`:
    This example demonstrates the binary pre-demodulator synchronizer. A random
    binary sequence is generated, modulated with BPSK, and then interpolated.
    The resulting sequence is used to generate a bpresync object which in turn
    is used to detect a signal in the presence of carrier frequency and timing
    offsets and additive white Gauss noise.

 * `bsequence_example.c`:
    This example demonstrates the interface to the bsequence (binary sequence)
    object.  The bsequence object acts like a buffer of bits which are stored
    and manipulated efficiently in memory.

 * `bufferf_example.c`:

 * `cgsolve_example.c`:
    Solve linear system of equations `Ax = b` using the conjugate-
    gradient method where A is a symmetric positive-definite matrix.
    Compare speed to matrixf_linsolve() for same system.

 * `chromosome_example.c`:

 * `compand_cf_example.c`:

 * `compand_example.c`:
    This example demonstrates the interface to the compand function
    (compression, expansion).  The compander is typically used with the
    quantizer to increase the dynamic range of the converter, particularly for
    low-level signals.  The transfer function is computed (empirically) and
    printed to the screen.

 * `complementary_codes_example.c`:
    This example demonstrates how to generate complementary binary codes
    in liquid.  A pair of codes is generated using the bsequence
    interface, their auto-correlations are computed, and the result is
    summed and printed to the screen.  The results are also printed to
    an output file, which plots the sequences and their
    auto-correlations.

    SEE ALSO: `bsequence_example.c`
              `msequence_example.c`

 * `cpfskmodem_example.c`:

 * `conversion_example.c`:
    
    This example demonstrates conversion from complex baseband to a real-valued
    signal, and then down-conversion back to complex baseband while removing the
    negative image.
    
     STEP 1: A signal is generated at complex baseband consisting of narrow-band
             filtered noise and an offset tone (to show asymmetry in the transmit
             spectrum).
    
     STEP 2: The signal is mixed up to a carrier 'fc' (relative to the sampling
             frequency) and the real-component of the result is retained. This is
             the DAC output. The spectrum of this signal has two images: one at
             +fc, the other at -fc.
    
     STEP 3: The DAC output is mixed back down to complex baseband and the lower
             image is (mostly) filtered off. Reminants of the lower frequency
             component are still visible due to the wide-band and low-order
             filter on the receiver. The received complex baseband signal also
             has a reduction in power by 2 because half the signal's energy (the
             negative image) is filtered off.
    
 * `crc_example.c`:
    Cyclic redundancy check (CRC) example.  This example demonstrates how a
    CRC can be used to validate data received through un-reliable means (e.g.
    a noisy channel).  A CRC is, in essence, a strong algebraic error
    detection code that computes a key on a block of data using base-2
    polynomials.  Also available is a checksum for data validation.

    SEE ALSO: `fec_example.c`

 * `cvsd_example.c`:
    Continuously-variable slope delta example, sinusoidal input. This example
    demonstrates the CVSD audio encoder interface, and its response to a
    sinusoidal input.  The output distortion ratio is computed, and the
    time-domain results are written to a file.

 * `dds_cccf_example.c`:
    Direct digital synthesizer example.  This example demonstrates
    the interface to the direct digital synthesizer.  A baseband
    pulse is generated and then efficiently up-converted
    (interpolated and mixed up) using the DDS object.  The resulting
    signal is then down-converted (mixed down and decimated) using
    the same DDS object.  Results are written to a file.

 * `dotprod_cccf_example.c`:
    This example demonstrates the interface to the complex
    floating-point dot product object (dotprod_cccf).

 * `dotprod_rrrf_example.c`:
    This example demonstrates the interface to the floating-point dot
    product object (dotprod_rrrf).

 * `eqlms_cccf_blind_example.c`:
    This example tests the least mean-squares (LMS) equalizer (EQ) on a
    signal with an unknown modulation and carrier frequency offset. That
    is, the equalization is done completely blind of the modulation
    scheme or its underlying data set. The error estimate assumes a
    constant modulus linear modulation scheme. This works surprisingly
    well even more amplitude-modulated signals, e.g. 'qam16'.

 * `eqlms_cccf_block_example.c`:
    This example tests the least mean-squares (LMS) equalizer (EQ) on a
    signal with an unknown modulation and carrier frequency offset.
    Equalization is performed blind on a block of samples and the reulting
    constellation is output to a file for plotting.

 * `eqlms_cccf_decisiondirected_example.c`:
    Tests least mean-squares (LMS) equalizer (EQ) on a signal with a known
    linear modulation scheme, but unknown data. The equalizer is updated
    using decision-directed demodulator output samples.

 * `eqlms_cccf_example.c`:
 * `eqrls_cccf_example.c`:

 * `error_handling_example.c`:
    This example demonstrates error handling in liquid.

 * `fct_example.c`:

 * `fec_example.c`:
    This example demonstrates the interface for forward error-correction (FEC)
    codes.  A buffer of data bytes is encoded and corrupted with several
    errors.  The decoder then attempts to recover the original data set.  The
    user may select the FEC scheme from the command-line interface.

    SEE ALSO: `crc_example.c`
              `checksum_example.c`
              `packetizer_example.c`

 * `fec_soft_example.c`:
    This example demonstrates the interface for forward error-correction
    (FEC) codes with soft-decision decoding.  A buffer of data bytes is
    encoded before the data are corrupted with at least one error and
    noise. The decoder then attempts to recover the original data set
    from the soft input bits.  The user may select the FEC scheme from
    the command-line interface.

    SEE ALSO: `fec_example.c`
              `packetizer_soft_example.c`

 * `fft_example.c`:
    This example demonstrates the interface to the fast discrete Fourier
    transform (FFT).

    SEE ALSO: `mdct_example.c`
              `fct_example.c`

  * `fftfilt_crcf_example.c`

    Complex FFT-based finite impulse response filter example. This example
    demonstrates the functionality of firfilt by designing a low-order 
    prototype and using it to filter a noisy signal.  The filter coefficients
    are  real, but the input and output arrays are complex. The filter order
    and cutoff frequency are specified at the beginning, and the result is
    compared to the regular corresponding firfilt_crcf output.

    SEE ALSO: `firfilt_crcf_example.c`

 * `firdecim_crcf_example.c`:
    This example demonstrates the interface to the firdecim (finite
    impulse response decimator) family of objects.
    Data symbols are generated and then interpolated according to a
    finite impulse response square-root Nyquist filter.  The resulting
    sequence is then decimated with the same filter, matched to the
    interpolator.

    SEE ALSO: `firinterp_crcf_example.c`

 * `firdes_kaiser_example.c`:
    This example demonstrates finite impulse response filter design using a
    Kaiser window.
    
    SEE ALSO: `firdespm_example.c`

 * `firdespm_example.c`:
    This example demonstrates finite impulse response filter design using the
    Parks-McClellan algorithm.

    SEE ALSO: `firdes_kaiser_example.c`

 * `firfarrow_rrrf_sine_example.c`:

 * `firfilt_rrrf_example.c`:

 * `firfilt_cccf_example.c`:
    This example demonstrates the finite impulse response (FIR) filter
    with complex coefficients as a cross-correlator between transmitted
    and received sequences.

 * `firfilt_crcf_example.c`:
    Complex finite impulse response filter example. Demonstrates the 
    functionality of firfilt by designing a low-order prototype and using it 
    to filter a noisy signal.  The filter coefficients are real, but the 
    input and output arrays are complex. The filter order and cutoff 
    frequency are specified at the beginning.

 * `firhilb_decim_example.c`:
    Hilbert transform: 2:1 real-to-complex decimator.  This example
    demonstrates the functionality of firhilb (finite impulse response Hilbert
    transform) decimator which converts a real time series into a complex one
    with half the number of samples.  The input is a real-valued sinusoid of N
    samples. The output is a complex-valued sinusoid of N/2 samples.

    SEE ALSO: `firhilb_interp_example.c`

 * `firhilb_example.c`:

 * `firhilb_interp_example.c`:
    Hilbert transform: 1:2 complex-to-real interpolator.  This example
    demonstrates the functionality of firhilb (finite impulse response Hilbert
    transform) interpolator which converts a complex time series into a real
    one with twice the number of samples.  The input is a complex-valued
    sinusoid of N samples. The output is a real-valued sinusoid of 2*N
    samples.

    SEE ALSO: `firhilb_decim_example.c`

 * `firpfbch2_crcf_example.c`:
    Example of the finite impulse response (FIR) polyphase filterbank
    (PFB) channelizer with an output rate of 2 Fs / M as an (almost)
    perfect reconstructive system.

 * `firinterp_crcf_example.c`:
    This example demonstrates the interp object (interpolator)
    interface. Data symbols are generated and then interpolated
    according to a finite impulse response Nyquist filter.

 * `firpfbch_crcf_analysis_example.c`:
    Example of the analysis channelizer filterbank. The input signal is
    comprised of several signals spanning different frequency bands. The
    channelizer downconverts each to baseband (maximally decimated), and
    the resulting spectrum of each is plotted.

 * `firpfbch_crcf_example.c`:
    Finite impulse response (FIR) polyphase filter bank (PFB)
    channelizer example.  This example demonstrates the functionality of
    the polyphase filter bank channelizer and how its output is
    mathematically equivalent to a series of parallel down-converters
    (mixers/decimators). Both the synthesis and analysis filter banks
    are presented.

 * `firpfbch_crcf_synthesis_example.c`:
    Example of the synthesis channelizer filterbank.  Random symbols are
    generated and loaded into the bins of the channelizer and the
    time-domain signal is synthesized.  Subcarriers around the band
    edges are disabled as well as those near 0.25 to demonstrate the
    synthesizer's ability to efficiently notch the spectrum. The results
    are printed to a file for plotting.

 * `flexframesync_example.c`:
    This example demonstrates the basic interface to the flexframegen and
    flexframesync objects used to completely encapsulate raw data bytes
    into frame samples (nearly) ready for over-the-air transmission. A
    14-byte header and variable length payload are encoded into baseband
    symbols using the flexframegen object.  The resulting symbols are
    interpolated using a root-Nyquist filter and the resulting samples are
    then fed into the flexframesync object which attempts to decode the
    frame. Whenever frame is found and properly decoded, its callback
    function is invoked.

 * `flexframesync_reconfig_example.c`:
    Demonstrates the reconfigurability of the flexframegen and
    flexframesync objects.

 * `framesync64_example.c`:
    This example demonstrates the interfaces to the framegen64 and
    framesync64 objects used to completely encapsulate data for
    over-the-air transmission.  A 24-byte header and 64-byte payload are
    encoded, modulated, and interpolated using the framegen64 object.
    The resulting complex baseband samples are corrupted with noise and
    moderate carrier frequency and phase offsets before the framesync64
    object attempts to decode the frame.  The resulting data are
    compared to the original to validate correctness.

    SEE ALSO: `flexframesync_example.c`

 * `freqmodem_example.c`:

 * `fskmodem_example.c`:
    This example demonstrates the M-ary frequency-shift keying
    (MFSK) modem in liquid. A message signal is modulated and the
    resulting signal is recovered using a demodulator object.

 * `gasearch_example.c`:

 * `gasearch_knapsack_example.c`:

 * `gmskmodem_example.c`:

 * `gradsearch_example.c`:

 * `gradsearch_datafit_example.c`:
    Fit 3-parameter curve to sampled data set in the minimum
    mean-squared error sense.

 * `iirdes_analog_example.c`:
    Tests infinite impulse response (IIR) analog filter design. While this
    example seems purely academic as IIR filters used in liquid are all
    digital, it is important to realize that they are all derived from their
    analog counterparts. This example serves to check the response of the
    analog filters to ensure they are correct.  The results of design are
    written to a file.

    SEE ALSO: `iirdes_example.c`
              `iirfilt_crcf_example.c`

 * `iirdes_example.c`:
    Tests infinite impulse response (IIR) digital filter design.

    SEE ALSO: `iirdes_analog_example.c`
              `iirfilt_crcf_example.c`

 * `iirdes_pll_example.c`:
    This example demonstrates 2nd-order IIR phase-locked loop filter
    design with a practical simulation.

    SEE ALSO: `nco_pll_example.c`
              `nco_pll_modem_example.c`

 * `iirfilt_cccf_example.c`:
    Complex infinite impulse response filter example. Demonstrates the
    functionality of iirfilt with complex coefficients by designing a
    filter with specified parameters and then filters noise.

 * `iirfilt_crcf_example.c`:
    Complex infinite impulse response filter example. Demonstrates the
    functionality of iirfilt by designing a low-order prototype (e.g.
    Butterworth) and using it to filter a noisy signal.  The filter
    coefficients are real, but the input and output arrays are complex.  The
    filter order and cutoff frequency are specified at the beginning.

 * `iirinterp_crcf_example.c`:
    This example demonstrates the iirinterp object (IIR interpolator)
    interface.

 * `interleaver_example.c`:
    This example demonstrates the functionality of the liquid interleaver
    object.  Interleavers serve to distribute  grouped bit errors evenly
    throughout a block of data. This aids certain forward error-correction
    codes in correcting bit errors.  In this example, data bits are
    interleaved and de-interleaved; the resulting sequence is validated to
    match the original.

    SEE ALSO: `packetizer_example.c`

 * `interleaver_scatterplot_example.c`:

 * `interleaver_soft_example.c`:

 * `kbd_window_example.c`:

 * `lpc_example.c`:
    This example demonstrates linear prediction in liquid. An input signal
    is generated which exhibits a strong temporal correlation. The linear
    predictor generates an approximating all-pole filter which minimizes
    the squared error between the prediction and the actual output.

 * `matched_filter_example.c`:

 * `math_lngamma_example.c`:
    Demonstrates accuracy of lngamma function.

 * `mdct_example.c`:

 * `modem_arb_example.c`:
    This example demonstrates the functionality of the arbitrary modem, a
    digital modulator/demodulator object with signal constellation points
    chosen arbitrarily.  A simple bit-error rate simulation is then run to
    test the performance of the modem.  The results are written to a file.

    SEE ALSO: `modem_example.c`

 * `modem_example.c`:
    This example demonstrates the digital modulator/demodulator (modem) object.
    Data symbols are modulated into complex samples which are then demodulated
    without noise or phase offsets.  The user may select the modulation scheme
    via the command-line interface.

    SEE ALSO: `modem_arb_example.c`

 * `modem_soft_example.c`:
    This example demonstrates soft demodulation of linear
    modulation schemes.

 * `modular_arithmetic_example.c`:
    This example demonstrates some modular arithmetic functions.

 * `msequence_example.c`:
    This example demonstrates the auto-correlation properties of a
    maximal-length sequence (m-sequence).  An m-sequence of a certain length
    is used to generate two binary sequences (buffers) which are then
    cross-correlated.  The resulting correlation produces -1 for all values
    except at index zero, where the sequences align.

    SEE ALSO: `bsequence_example.c`

 * `msourcecf_example.c`:
    This example demonstrates generating multiple signal sources simultaneously
    for testing using the msource (multi-source) family of objects.

 * `msresamp_crcf_example.c`:
    Demonstration of the multi-stage arbitrary resampler.

 * `msresamp2_crcf_example.c`:
    Demonstration of the multi-stage half-band resampler.

 * `nco_example.c`:
    This example demonstrates the most basic functionality of the
    numerically-controlled oscillator (NCO) object.

    SEE ALSO: `nco_pll_example.c`
              `nco_pll_modem_example.c`

 * `nco_pll_example.c`:
    This example demonstrates how the use the nco/pll object
    (numerically-controlled oscillator with phase-locked loop) interface for
    tracking to a complex sinusoid.  The loop bandwidth, phase offset, and
    other parameter can be specified via the command-line interface.

    SEE ALSO: `nco_example.c`
              `nco_pll_modem_example.c`

 * `nco_pll_modem_example.c`:
    This example demonstrates how the nco/pll object (numerically-controlled
    oscillator with phase-locked loop) can be used for carrier frequency
    recovery in digital modems.  The modem type, SNR, and other parameters are
    specified via the command-line interface.

    SEE ALSO: `nco_example.c`
              `nco_pll_example.c`

 * `nyquist_filter_example.c`:
 * `ofdmflexframesync_example.c`:
 * `ofdmframegen_example.c`:
 * `ofdmframesync_example.c`:

 * `packetizer_example.c`:
    Demonstrates the functionality of the packetizer object.  Data are encoded
    using two forward error-correction schemes (an inner and outer code)
    before data errors are introduced.  The decoder then tries to recover the
    original data message.

    SEE ALSO: `fec_example.c`
              `crc_example.c`

 * `packetizer_soft_example.c`:
    This example demonstrates the functionality of the packetizer object
    for soft-decision decoding.  Data are encoded using two forward error-
    correction schemes (an inner and outer code) before noise and data
    errors are added. The decoder then tries to recover the original data
    message. Only the outer code uses soft-decision decoding.

    SEE ALSO: `fec_soft_example.c`
              `packetizer_example.c`

 * `pll_example.c`:
    Demonstrates a basic phase-locked loop to track the phase of a
    complex sinusoid.

 * `poly_findroots_example.c`:

 * `polyfit_example.c`:
    Test polynomial fit to sample data.

    SEE ALSO: `polyfit_lagrange_example.c`

 * `polyfit_lagrange_example.c`:
    Test exact polynomial fit to sample data using Lagrange interpolating
    polynomials.

    SEE ALSO: `polyfit_example.c`

 * `qdetector_cccf_example.c`:
    This example demonstrates the functionality of the qdetector object
    to detect an arbitrary signal in time in the presence of noise,
    carrier frequency/phase offsets, and fractional-sample timing
    offsets.

 * `qpacketmodem_example.c`:
    This example demonstrates the basic packet modem encoder/decoder
    operation. A packet of data is encoded and modulated into symbols,
    channel noise is added, and the resulting packet is demodulated
    and decoded.

 * `qnsearch_example.c`:

 * `quantize_example.c`:

 * `random_histogram_example.c`:
    This example tests the random number generators for different
    distributions.

 * `repack_bytes_example.c`:
    This example demonstrates the repack_bytes() interface by packing a
    sequence of three 3-bit symbols into five 2-bit symbols.  The
    results are printed to the screen.  Because the total number of bits
    in the input is 9 and not evenly divisible by 2, the last of the 5
    output symbols has a zero explicitly padded to the end.

  * `resamp2_cccf_example.c`
    This example demonstrates the halfband resampler cenetered at the
    quarter sample rate to split the signal into positive and negative
    frequency bands. Two distinct narrow-band signals are generated; one
    at a positive frequency and one at a negative frequency. The resamp2
    object is run as a filter to separate the two about the zero-
    frequency center point.

 * `resamp2_crcf_example.c`:
    This example demonstrates the halfband resampler running as both an
    interpolator and a decimator. A narrow-band signal is first
    interpolated by a factor of 2, and then decimated. The resulting RMS
     error between the final signal and original is computed and printed
    to the screen.

 * `resamp2_crcf_decim_example.c`:
    Halfband decimator.  This example demonstrates the interface to the
    decimating halfband resampler.  A low-frequency input sinusoid is
    generated and fed into the decimator two samples at a time, producing one
    output at each iteration.  The results are written to an output file.

    SEE ALSO: `resamp2_crcf_interp_example.c`
              `decim_rrrf_example.c`

 * `resamp2_crcf_filter_example.c`:
    Halfband (two-channel) filterbank example. This example demonstrates
    the analyzer/synthesizer execute() methods for the resamp2_xxxt
    family of objects.

    NOTE: The filterbank is not a perfect reconstruction filter; a
          significant amount of distortion occurs in the transition band
          of the half-band filters.

 * `resamp2_crcf_interp_example.c`:
    Halfband interpolator.  This example demonstrates the interface to the
    interpolating halfband resampler.  A low-frequency input sinusoid is
    generated and fed into the interpolator one sample at a time, producing
    two outputs at each iteration.  The results are written to an output file.

    SEE ALSO: `resamp2_crcf_decim_example.c`
              `interp_crcf_example.c`

 * `resamp_crcf_example.c`:

 * `scramble_example.c`:
    Data-scrambling example.  Physical layer synchronization of received
    waveforms relies on independent and identically distributed underlying
    data symbols.  If the message sequence, however, is '00000....' and the
    modulation scheme is BPSK, the synchronizer probably won't be able to
    recover the symbol timing.  It is imperative to increase the entropy of
    the data for this to happen.  The data scrambler routine attempts to
    'whiten' the data sequence with a bit mask in order to achieve maximum
    entropy.  This example demonstrates the interface.

 * `smatrix_example.c`:

 * `spgramcf_example.c`:
   Spectral periodogram example with complex inputs.

 * `spgramf_example.c`:
   Spectral periodogram example with real inputs.

 * `symsync_crcf_example.c`:
    This example demonstrates the basic principles of the symbol timing
    recovery family of objects, specifically symsync_crcf. A set of random
    QPSK symbols are generated and interpolated with a timing offset. The
    resulting signal is run through the symsync_crcf object which applies a
    matched filter and recovers timing producing a clean constellation.

 * `symsync_crcf_full_example.c`:
    This example extends that of `symsync_crcf_example.c` by including options
    for simulating a timing rate offset in addition to just a timing phase
    error. The resulting output file shows not just the constellation but the
    time domain sequence as well as the timing phase estimate over time.

 * `symsync_crcf_kaiser_example.c`:
    This is a simplified example of the symync family of objects to show how
    symbol timing can be recovered after the matched filter output.

  * `symtrack_cccf_example.c`:
    
    This example demonstrates how to recover data symbols using the symtrack
    object. A stream of modulated and interpolated symbols are generated using
    the symstream object. The resulting samples are passed through a channel
    to add various impairments. The symtrack object recovers timing, carrier,
    and other information imparted by the channel and returns data symbols
    ready for demodulation.

 * `wdelayf_example.c`:

 * `windowf_example.c`:
    This example demonstrates the functionality of a window buffer (also
    known as a circular or ring buffer) of floating-point values.
    Values are written to and read from the buffer using several
    different methods.

    SEE ALSO: `bufferf_example.c`
              `wdelayf_example.c`


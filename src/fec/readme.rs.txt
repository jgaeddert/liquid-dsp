===================================
 Reed-Solomon codes
===================================

This document gives several notes on the usage of Reed-Solomon (RS) codes in
liquid.  The fec object for RS codes allows for any length input sequence to
be encoded.  This is accomplished by splitting the uncoded input sequence into
a discrete number of blocks of nearly equal length.  Each block is then padded
with zeros (internally) before the encoding process.

For example, the 8-bit (255,223) code adds 32 parity symbols to an uncoded
input message of length 223 symbols. To encode messages of lengths less than
223, the input is padded with zeros until its length is exactly 223.  The 32
parity symbols are then computed and appended to the end. Internally, libfec
does this efficiently and seamlessly.  However, there is no simply way to
encode messages of lengths larger than 223 (for this 8-bit example).

Let us assume that we want to encode a message of 1024 8-bit symbols.
Unfortuantely 223 does not evenly divide 1024, so we cannot simply break the
original message into blocks that neatly fit our codec.  As an alternative, we
could split the message into four blocks of 223 and one block of 132, viz.
    1024 = (4)*223 + 132
At first glance this is a tractable solution, however it has several inherent
disadvantages:
    1.  libfec uses an internal object to handle padding for blocks less than
        223, so if we want to take advantage of its efficient implementation
        we will need to either create two RS libfec objects (one for blocks of
        lenght 223 and one for blocks of length 132) or re-create the object
        every time.  As an alternative we could just use one RS object which
        operates on blocks of length 223 and just manually pad the data for
        the block of length 132, however this can be cumbersome and
        inefficient.
    2.  Because the same number of parity symbols (in this case, 32) are
        applied to each block during the encoding process, blocks of shorter
        lengths will have more error-correction capabilities.  In this case,
        the blocks of length 223 will have weaker error protection than those
        of length 132 and the resulting total error protection will be
        degraded.
For these reasons, liquid splits the input message into (nearly) equally-sized
blocks.  For our example, we need a minimum of five blocks because
    ceil(1024 / 223) = ceil(4.5919) = 5
We would like to split the message into five blocks whose lenghts are of
perfect equality, but unfortunately five does not evenly divide 1024.  The
closest we can get is four blocks of 205 and one block of 204, viz.
    1024 = (4)*205 + 204
As a result, each block has (nearly) the same error protection, and just one
RS libfec object can be used.  It is important to remember that the last block
needs to be padded by just one zero before encoding.

References:
    [Karn:2002] Karn, P. "libfec," http://www.ka9q.net/code/fec/


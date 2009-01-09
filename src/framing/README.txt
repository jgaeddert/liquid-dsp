======================================
 Generic packet header
======================================

section             # bits          Description
phasing pattern     8 - 1024 / 8
p/n sequence        32
header              128 (256 enc)
    crc             32              Cyclic redundancy check
    src0            8               Ultimate source node id
    src1            8               Immediate source node id
    dst0            8               Ultimate destination node id
    dst1            8               Immediate destination node id
    mod scheme,bps  8               Modulation scheme (including bits/symbol)
    outer int/fec   3/5             Outer FEC and interleaving scheme
    inner int/fec   3/5             Inner FEC and interleaving scheme
    protocol        8               Protocol ID
    msg len/bytes   16              Uncoded packet length (bytes)
    num packets     16              Number of packets in frame

The header includes a mandatory 32-bit crc with 1/2 rate forward error-
correction code totaling 256 bits (32 bytes) of encoded data.


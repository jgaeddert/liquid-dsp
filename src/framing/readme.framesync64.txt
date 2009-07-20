======================================
 frame-64 description
======================================

section             length          Description
ramp-up             64 symbols      BPSK phasing pattern
phasing pattern     64 symbols
p/n sequence        64 symbols      BPSK p/n sequence (m-sequence)
header              256 symbols     QPSK packet header
    header data     24 bytes        header payload
    payload crc     4 bytes         data payload cyclic redundancy check
    header crc      4 bytes         header cyclic redundancy check
payload             512 symbols     QPSK data payload
ramp-dn             64 symbols      ramp down

The header includes a mandatory 32-bit crc with 1/2 rate forward error-
correction code totaling 256 bits (32 bytes) of encoded data.

